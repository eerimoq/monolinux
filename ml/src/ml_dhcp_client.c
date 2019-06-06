/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Monolinux project.
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ml/ml.h"

#define BOOTP_MESSAGE_TYPE_BOOT_REQUEST           1
#define BOOTP_MESSAGE_TYPE_BOOT_RESPONSE          2

#define HARDWARE_TYPE_ETHERNET                    1
#define HARDWARE_ADDRESS_LENGTH                   6
#define BOOT_FLAGS                                0x8000
#define MAGIC_COOKIE_DHCP                         0x63825363

/* DHCP option identifiers. */
#define OPTION_SUBNET_MASK                        1
#define OPTION_ROUTER                             2
#define OPTION_DOMAIN_NAME_SERVER                 6
#define OPTION_HOST_NAME                          12
#define OPTION_REQUSETED_IP_ADDRESS               50
#define OPTION_IP_ADDRESS_LEASE_TIME              51
#define OPTION_DHCP_MESSAGE_TYPE                  53
#define OPTION_DHCP_SERVER_IDENTIFIER             54
#define OPTION_PARAMETER_REQUEST_LIST             55
#define OPTION_MAXIMUM_DHCP_MESSAGE_SIZE          57
#define OPTION_RENEWAL_TIME_INTERVAL              58
#define OPTION_REBINDING_TIME_INTERVAL            59
#define OPTION_END                                255

/* DHCP message types. */
#define MESSAGE_TYPE_DISCOVER                     1
#define MESSAGE_TYPE_OFFER                        2
#define MESSAGE_TYPE_REQUEST                      3
#define MESSAGE_TYPE_DECLINE                      4
#define MESSAGE_TYPE_ACK                          5
#define MESSAGE_TYPE_NAK                          6
#define MESSAGE_TYPE_RELEASE                      7
#define MESSAGE_TYPE_INFORM                       8

#define WAIT_FOREVER                              -1

#define SERVER_PORT                               67
#define CLIENT_PORT                               68

#define OPTIONS_OFFSET                            240

/* Socket indexes in the poll list. */
#define SOCK_IX                                   0
#define RENEW_IX                                  1
#define REBIND_IX                                 2
#define RESP_IX                                   3
#define INIT_IX                                   4

#define MAXIMUM_PACKET_SIZE                       1024

struct option_u8_t {
    uint8_t value;
    bool valid;
};

struct option_u32_t {
    uint32_t value;
    bool valid;
};

struct options_t {
    struct option_u8_t message_type;
    struct option_u32_t subnet_mask;
    struct option_u32_t lease_time;
    struct option_u32_t renewal_time;
    struct option_u32_t rebinding_time;
    struct option_u32_t server_ip_address;
};

static uint32_t inet_checksum_begin(void)
{
  return (0);
}

static uint32_t inet_checksum_acc(uint32_t acc, const uint16_t *buf_p, size_t size)
{
    while (size > 1) {
        acc += htons(*buf_p);
        buf_p++;
        size -= 2;
    }

    if (size > 0) {
        acc += (htons(*buf_p) & 0xff00);
    }

    return (acc);
}

static uint16_t inet_checksum_end(uint32_t acc)
{
    acc = (acc >> 16) + (acc & 0xffffUL);

    if ((acc & 0xffff0000UL) != 0) {
        acc = (acc >> 16) + (acc & 0xffffUL);
    }

    return (~acc);
}

static uint16_t inet_checksum(const void *buf_p, size_t size)
{
    uint32_t acc;

    acc = inet_checksum_begin();
    acc = inet_checksum_acc(acc, buf_p, size);

    return (inet_checksum_end(acc));
}

static const char *packet_type_str(uint8_t packet_type)
{
    const char *res_p;

    switch (packet_type) {

    case ml_dhcp_client_packet_type_offer_t:
        res_p = "OFFER";
        break;

    case ml_dhcp_client_packet_type_ack_t:
        res_p = "ACK";
        break;

    case ml_dhcp_client_packet_type_nak_t:
        res_p = "NAK";
        break;

    case ml_dhcp_client_packet_type_none_t:
        res_p = "NONE";
        break;

    default:
        res_p = "UNKNOWN";
    }

    return (res_p);
}

static const char *message_type_str(uint8_t message_type)
{
    const char *res_p;

    switch (message_type) {

    case MESSAGE_TYPE_DISCOVER:
        res_p = "DISCOVER";
        break;

    case MESSAGE_TYPE_OFFER:
        res_p = "OFFER";
        break;

    case MESSAGE_TYPE_REQUEST:
        res_p = "REQUEST";
        break;

    case MESSAGE_TYPE_DECLINE:
        res_p = "DECLINE";
        break;

    case MESSAGE_TYPE_ACK:
        res_p = "ACK";
        break;

    case MESSAGE_TYPE_NAK:
        res_p = "NAK";
        break;

    case MESSAGE_TYPE_RELEASE:
        res_p = "RELEASE";
        break;

    case MESSAGE_TYPE_INFORM:
        res_p = "INFORM";
        break;

    default:
        res_p = "UNKNOWN";
    }

    return (res_p);
}

static const char *state_string(enum ml_dhcp_client_state_t state)
{
    const char *res_p;

    switch (state) {

    case ml_dhcp_client_state_init_t:
        res_p = "INIT";
        break;

    case ml_dhcp_client_state_selecting_t:
        res_p = "SELECTING";
        break;

    case ml_dhcp_client_state_requesting_t:
        res_p = "REQUESTING";
        break;

    case ml_dhcp_client_state_bound_t:
        res_p = "BOUND";
        break;

    case ml_dhcp_client_state_renewing_t:
        res_p = "RENEWING";
        break;

    default:
        res_p = "UNKNOWN";
        break;
    }

    return (res_p);
}

static bool is_offer(struct ml_dhcp_client_t *self_p)
{
    return (self_p->packet_type == ml_dhcp_client_packet_type_offer_t);
}

static bool is_ack(struct ml_dhcp_client_t *self_p)
{
    return (self_p->packet_type == ml_dhcp_client_packet_type_ack_t);
}

static bool is_nak(struct ml_dhcp_client_t *self_p)
{
    return (self_p->packet_type == ml_dhcp_client_packet_type_nak_t);
}

static bool is_response_timeout(struct ml_dhcp_client_t *self_p)
{
    return (self_p->response_timer_expired);
}

static bool is_renewal_timeout(struct ml_dhcp_client_t *self_p)
{
    return (self_p->renewal_timer_expired);
}

static bool is_rebinding_timeout(struct ml_dhcp_client_t *self_p)
{
    return (self_p->rebinding_timer_expired);
}

static bool is_init_timeout(struct ml_dhcp_client_t *self_p)
{
    return (self_p->init_timer_expired);
}

static int unpack_option_u8(struct option_u8_t *option_p,
                            const uint8_t *buf_p,
                            uint8_t size)
{
    if (size != 1) {
        return (-1);
    }

    option_p->value = buf_p[0];
    option_p->valid = true;

    return (0);
}

static int unpack_option_u32(struct option_u32_t *option_p,
                             const uint8_t *buf_p,
                             uint8_t size)
{
    if (size != 4) {
        return (-1);
    }

    option_p->value = ((buf_p[0] << 24)
                       | (buf_p[1] << 16)
                       | (buf_p[2] << 8)
                       | (buf_p[3] << 0));
    option_p->valid = true;

    return (0);
}

static int unpack_option(struct ml_dhcp_client_t *self_p,
                         struct options_t *options_p,
                         uint8_t option,
                         const uint8_t *buf_p,
                         uint8_t length)
{
    int res;

    switch (option) {

    case OPTION_SUBNET_MASK:
        res = unpack_option_u32(&options_p->subnet_mask, buf_p, length);
        break;

    case OPTION_DHCP_MESSAGE_TYPE:
        res = unpack_option_u8(&options_p->message_type, buf_p, length);
        break;

    case OPTION_IP_ADDRESS_LEASE_TIME:
        res = unpack_option_u32(&options_p->lease_time, buf_p, length);
        break;

    case OPTION_RENEWAL_TIME_INTERVAL:
        res = unpack_option_u32(&options_p->renewal_time, buf_p, length);
        break;

    case OPTION_REBINDING_TIME_INTERVAL:
        res = unpack_option_u32(&options_p->rebinding_time, buf_p, length);
        break;

    case OPTION_DHCP_SERVER_IDENTIFIER:
        res = unpack_option_u32(&options_p->server_ip_address, buf_p, length);
        break;

    default:
        ML_DEBUG("Ignoring DHCP option %d.", option);
        res = 0;
        break;
    }

    return (res);
}

static int unpack_all_options(struct ml_dhcp_client_t *self_p,
                              struct options_t *options_p,
                              int fd)
{
    ssize_t res;
    uint8_t option;
    uint8_t length;
    uint8_t buf[255];

    memset(options_p, 0, sizeof(*options_p));

    while (true) {
        res = read(fd, &option, sizeof(option));

        if (res != sizeof(option)) {
            return (-1);
        }

        if (option == OPTION_END) {
            break;
        }

        res = read(fd, &length, sizeof(length));

        if (res != sizeof(length)) {
            return (-1);
        }

        res = read(fd, &buf[0], length);

        if (res != length) {
            return (-1);
        }

        res = unpack_option(self_p, options_p, option, &buf[0], length);

        if (res != 0) {
            return (res);
        }
    }

    return (0);
}

static int unpack_options(struct ml_dhcp_client_t *self_p,
                          struct options_t *options_p,
                          const uint8_t *buf_p,
                          size_t size)
{
    int fd;
    int res;

    res = -1;
    fd = memfd_create("packet", 0);

    if (fd != -1) {
        if (write(fd, buf_p, size) == (ssize_t)size) {
            if (lseek(fd, 0, SEEK_SET) != -1) {
                res = unpack_all_options(self_p, options_p, fd);
            }
        }

        close(fd);
    }

    return (res);
}

static void unpack_message_type_offer(struct ml_dhcp_client_t *self_p,
                                      struct options_t *options_p,
                                      const uint8_t *buf_p)
{
    self_p->offer.ip_address = ((buf_p[0] << 24)
                                | (buf_p[1] << 16)
                                | (buf_p[2] << 8)
                                | (buf_p[3] << 0));

    if (!options_p->server_ip_address.valid) {
        return;
    }

    if (!options_p->lease_time.valid) {
        return;
    }

    if (options_p->lease_time.value < 1) {
        return;
    }

    self_p->server.ip_address = options_p->server_ip_address.value;
    self_p->offer.lease_time = options_p->lease_time.value;
    self_p->packet_type = ml_dhcp_client_packet_type_offer_t;
}

static void unpack_message_type_ack(struct ml_dhcp_client_t *self_p)
{
    self_p->packet_type = ml_dhcp_client_packet_type_ack_t;
}

static void unpack_message_type_nak(struct ml_dhcp_client_t *self_p)
{
    self_p->packet_type = ml_dhcp_client_packet_type_nak_t;
}

static void unpack_packet(struct ml_dhcp_client_t *self_p,
                          const uint8_t *buf_p,
                          size_t size)
{
    int res;
    struct options_t options;

    if (self_p->is_packet_socket) {
        if (size < 20) {
            return;
        }

        /* ToDo: IPv4 validation. */

        if (size < 28) {
            return;
        }

        /* ToDo: UDP validation. */

        buf_p += 28;
        size -= 28;
    }

    if (size <= OPTIONS_OFFSET) {
        return;
    }

    if (buf_p[0] != BOOTP_MESSAGE_TYPE_BOOT_RESPONSE) {
        return;
    }

    if (buf_p[1] != HARDWARE_TYPE_ETHERNET) {
        return;
    }

    res = memcmp(&buf_p[28],
                 &self_p->self.mac_address[0],
                 sizeof(self_p->self.mac_address));

    if (res != 0) {
        return;
    }

    res = unpack_options(self_p,
                         &options,
                         &buf_p[OPTIONS_OFFSET],
                         size - OPTIONS_OFFSET);

    if (res != 0) {
        return;
    }

    if (!options.message_type.valid) {
        return;
    }

    switch (options.message_type.value) {

    case MESSAGE_TYPE_OFFER:
        unpack_message_type_offer(self_p, &options, &buf_p[16]);
        break;

    case MESSAGE_TYPE_ACK:
        unpack_message_type_ack(self_p);
        break;

    case MESSAGE_TYPE_NAK:
        unpack_message_type_nak(self_p);
        break;

    default:
        ML_DEBUG("Invalid packet type %d.", options.message_type);
        break;
    }
}

static void init_pollfd(struct pollfd *fd_p, int fd)
{
    fd_p->fd = fd;
    fd_p->events = POLLIN;
}

static int setup_packet_socket(struct ml_dhcp_client_t *self_p)
{
    int res;
    int sock;
    int yes;
    struct sockaddr_ll addr;

    res = -1;

    /* Linux adds ethernet header. */
    sock = socket(AF_PACKET, SOCK_DGRAM, 0);

    if (sock == -1) {
        res = -1;
        goto err1;
    }

    /* Receive IP packets from a single interface. */
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_IP);
    addr.sll_ifindex = self_p->interface.index;

    res = bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    if (res != 0) {
        goto err2;
    }

    /* Allow broadcast. */
    yes = 1;
    res = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

    if (res != 0) {
        goto err2;
    }

    self_p->is_packet_socket = true;
    init_pollfd(&self_p->fds[SOCK_IX], sock);

    return (0);

err2:
    ml_close(sock);

err1:
    ML_ERROR("Packet socket setup failed.");

    return (res);
}

static int setup_udp_socket(struct ml_dhcp_client_t *self_p)
{
    int res;
    int sock;
    struct sockaddr_in addr;

    res = -1;
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        res = -1;
        goto err1;
    }

    /* Receive IP packets only from a single interface. */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLIENT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    res = bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    if (res != 0) {
        goto err2;
    }

    self_p->is_packet_socket = false;
    init_pollfd(&self_p->fds[SOCK_IX], sock);

    return (0);

err2:
    ml_close(sock);

err1:
    ML_ERROR("UDP socket setup failed.");

    return (res);
}

static void configure_interface(struct ml_dhcp_client_t *self_p)
{
    int res;
    char ip_address[16];

    snprintf(&ip_address[0],
             sizeof(ip_address),
             "%u.%u.%u.%u",
             (self_p->offer.ip_address >> 24) & 0xff,
             (self_p->offer.ip_address >> 16) & 0xff,
             (self_p->offer.ip_address >> 8) & 0xff,
             (self_p->offer.ip_address >> 0) & 0xff);

    ML_INFO("Configuring '%s' with ip address '%s'.",
            self_p->interface.name_p,
            &ip_address[0]);

    res = ml_network_interface_configure(self_p->interface.name_p,
                                         &ip_address[0],
                                         "255.255.255.0");

    if (res != 0) {
        ML_WARNING("Failed to configure '%s' with ip address '%s'.",
                   self_p->interface.name_p,
                   &ip_address[0]);
    }
}

static bool is_timeout(struct pollfd *fd_p)
{
    bool timeout;
    ssize_t size;
    uint64_t value;

    timeout = false;

    if (fd_p->revents & POLLIN) {
        size = ml_read(fd_p->fd, &value, sizeof(value));

        if (size == sizeof(value)) {
            timeout = true;
        }
    }

    return (timeout);
}

static void update_events(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[MAXIMUM_PACKET_SIZE];
    ssize_t size;

    self_p->packet_type = ml_dhcp_client_packet_type_none_t;

    if (self_p->fds[SOCK_IX].revents & POLLIN) {
        size = ml_read(self_p->fds[SOCK_IX].fd, &buf[0], sizeof(buf));

        if (size > 0) {
            unpack_packet(self_p, &buf[0], size);
        }

        ML_DEBUG("Received %s packet.", packet_type_str(self_p->packet_type));
    }

    self_p->renewal_timer_expired = is_timeout(&self_p->fds[RENEW_IX]);
    self_p->rebinding_timer_expired = is_timeout(&self_p->fds[REBIND_IX]);
    self_p->response_timer_expired = is_timeout(&self_p->fds[RESP_IX]);
    self_p->init_timer_expired = is_timeout(&self_p->fds[INIT_IX]);

    ML_DEBUG("Timers status:");
    ML_DEBUG("  RenewalTimerExpired:   %s",
             ml_bool_str(self_p->renewal_timer_expired));
    ML_DEBUG("  RebindingTimerExpired: %s",
             ml_bool_str(self_p->rebinding_timer_expired));
    ML_DEBUG("  ResponseTimerExpired:  %s",
             ml_bool_str(self_p->response_timer_expired));
    ML_DEBUG("  InitTimerExpired:      %s",
             ml_bool_str(self_p->init_timer_expired));
}

static int set_timer(int fd, time_t seconds, long nanoseconds)
{
    struct itimerspec timeout;

    memset(&timeout, 0, sizeof(timeout));
    timeout.it_value.tv_sec = seconds;
    timeout.it_value.tv_nsec = nanoseconds;

    return (timerfd_settime(fd, 0, &timeout, NULL));
}

static int set_renewal_timer(struct ml_dhcp_client_t *self_p)
{
    return (set_timer(self_p->fds[RENEW_IX].fd, 50, 0));
}

static int set_rebinding_timer(struct ml_dhcp_client_t *self_p)
{
    return (set_timer(self_p->fds[REBIND_IX].fd, 60, 0));
}

static int set_response_timer(struct ml_dhcp_client_t *self_p)
{
    return (set_timer(self_p->fds[RESP_IX].fd, 5, 0));
}

static void cancel_rebinding_timer(struct ml_dhcp_client_t *self_p)
{
    set_timer(self_p->fds[REBIND_IX].fd, 0, 0);
}

static void cancel_response_timer(struct ml_dhcp_client_t *self_p)
{
    set_timer(self_p->fds[RESP_IX].fd, 0, 0);
}

static int set_init_timer(struct ml_dhcp_client_t *self_p,
                          time_t seconds,
                          long nanoseconds)
{
    return (set_timer(self_p->fds[INIT_IX].fd, seconds, nanoseconds));
}

static bool send_packet(struct ml_dhcp_client_t *self_p,
                        uint8_t *buf_p,
                        size_t size)
{
    bool ok;
    ssize_t res;
    struct sockaddr_in addr;

    ok = false;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(self_p->server.ip_address);
    addr.sin_port = htons(SERVER_PORT);

    ML_DEBUG("Sending %s packet.", message_type_str(buf_p[OPTIONS_OFFSET + 2]));

    res = sendto(self_p->fds[SOCK_IX].fd,
                 buf_p,
                 size,
                 0,
                 (struct sockaddr *)&addr,
                 sizeof(addr));

    if (res == (ssize_t)size) {
        res = set_response_timer(self_p);

        if (res != -1) {
            ok = true;
        }
    }

    return (ok);
}

static bool broadcast_packet(struct ml_dhcp_client_t *self_p,
                             uint8_t *buf_p,
                             size_t size)
{
    bool ok;
    ssize_t res;
    struct sockaddr_ll addr;

    ok = false;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_IP);
    addr.sll_ifindex = self_p->interface.index;

    ML_DEBUG("Broadcasting %s packet.", message_type_str(buf_p[28 + OPTIONS_OFFSET + 2]));

    res = sendto(self_p->fds[SOCK_IX].fd,
                 buf_p,
                 size,
                 0,
                 (struct sockaddr *)&addr,
                 sizeof(addr));

    if (res == (ssize_t)size) {
        res = set_response_timer(self_p);

        if (res != -1) {
            ok = true;
        }
    }

    return (ok);
}

static void pack_ip_header(uint8_t *buf_p, size_t size)
{
    uint16_t checksum;

    buf_p[0] = ((4 << 4) | 5);
    buf_p[2] = (uint8_t)(size >> 8);
    buf_p[3] = (uint8_t)size;
    buf_p[5] = 0x11;
    buf_p[8] = 255;
    buf_p[9] = 17;
    buf_p[16] = 255;
    buf_p[17] = 255;
    buf_p[18] = 255;
    buf_p[19] = 255;
    checksum = inet_checksum(&buf_p[0], 20);
    buf_p[10] = (uint8_t)(checksum >> 8);
    buf_p[11] = (uint8_t)checksum;
}

static void pack_udp_header(uint8_t *buf_p, size_t size)
{
    buf_p[0] = (uint8_t)(CLIENT_PORT >> 8);
    buf_p[1] = (uint8_t)CLIENT_PORT;
    buf_p[2] = (uint8_t)(SERVER_PORT >> 8);
    buf_p[3] = (uint8_t)SERVER_PORT;
    buf_p[4] = (uint8_t)(size >> 8);
    buf_p[5] = (uint8_t)size;
}

static bool broadcast_discover(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[604];
    uint32_t transaction_id;

    memset(&buf[0], 0, sizeof(buf));
    pack_ip_header(&buf[0], sizeof(buf));
    pack_udp_header(&buf[20], 584);

    /* DHCP packet. */
    transaction_id = 0x32493678;
    buf[28] = BOOTP_MESSAGE_TYPE_BOOT_REQUEST;
    buf[29] = HARDWARE_TYPE_ETHERNET;
    buf[30] = HARDWARE_ADDRESS_LENGTH;
    buf[32] = (transaction_id >> 24);
    buf[33] = (transaction_id >> 16);
    buf[34] = (transaction_id >> 8);
    buf[35] = (transaction_id >> 0);
    buf[38] = (uint8_t)(BOOT_FLAGS >> 8);
    buf[39] = (uint8_t)(BOOT_FLAGS >> 0);
    memcpy(&buf[56],
           &self_p->self.mac_address[0],
           sizeof(self_p->self.mac_address));
    buf[264] = (uint8_t)(MAGIC_COOKIE_DHCP >> 24);
    buf[265] = (uint8_t)(MAGIC_COOKIE_DHCP >> 16);
    buf[266] = (uint8_t)(MAGIC_COOKIE_DHCP >> 8);
    buf[267] = (uint8_t)(MAGIC_COOKIE_DHCP >> 0);
    buf[268] = OPTION_DHCP_MESSAGE_TYPE;
    buf[269] = 1;
    buf[270] = MESSAGE_TYPE_DISCOVER;
    buf[271] = OPTION_MAXIMUM_DHCP_MESSAGE_SIZE;
    buf[272] = 2;
    buf[273] = (uint8_t)(MAXIMUM_PACKET_SIZE >> 8);
    buf[274] = (uint8_t)(MAXIMUM_PACKET_SIZE >> 0);
    buf[275] = OPTION_PARAMETER_REQUEST_LIST;
    buf[276] = 3;
    buf[277] = 1;
    buf[278] = 6;
    buf[279] = 3;
    buf[280] = OPTION_END;

    return (broadcast_packet(self_p, &buf[0], sizeof(buf)));
}

static bool broadcast_request(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[604];
    uint32_t transaction_id;

    memset(&buf[0], 0, sizeof(buf));
    pack_ip_header(&buf[0], sizeof(buf));
    pack_udp_header(&buf[20], 584);

    /* DHCP packet. */
    transaction_id = 0x32493678;
    buf[28] = BOOTP_MESSAGE_TYPE_BOOT_REQUEST;
    buf[29] = HARDWARE_TYPE_ETHERNET;
    buf[30] = HARDWARE_ADDRESS_LENGTH;
    buf[32] = (transaction_id >> 24);
    buf[33] = (transaction_id >> 16);
    buf[34] = (transaction_id >> 8);
    buf[35] = (transaction_id >> 0);
    buf[38] = (uint8_t)(BOOT_FLAGS >> 8);
    buf[39] = (uint8_t)(BOOT_FLAGS >> 0);
    memcpy(&buf[56],
           &self_p->self.mac_address[0],
           sizeof(self_p->self.mac_address));
    buf[264] = (uint8_t)(MAGIC_COOKIE_DHCP >> 24);
    buf[265] = (uint8_t)(MAGIC_COOKIE_DHCP >> 16);
    buf[266] = (uint8_t)(MAGIC_COOKIE_DHCP >> 8);
    buf[267] = (uint8_t)(MAGIC_COOKIE_DHCP >> 0);
    buf[268] = OPTION_DHCP_MESSAGE_TYPE;
    buf[269] = 1;
    buf[270] = MESSAGE_TYPE_REQUEST;
    buf[271] = OPTION_DHCP_SERVER_IDENTIFIER;
    buf[272] = 4;
    buf[273] = (self_p->server.ip_address >> 24);
    buf[274] = (self_p->server.ip_address >> 16);
    buf[275] = (self_p->server.ip_address >> 8);
    buf[276] = (self_p->server.ip_address >> 0);
    buf[277] = OPTION_REQUSETED_IP_ADDRESS;
    buf[278] = 4;
    buf[279] = (self_p->offer.ip_address >> 24);
    buf[280] = (self_p->offer.ip_address >> 16);
    buf[281] = (self_p->offer.ip_address >> 8);
    buf[282] = (self_p->offer.ip_address >> 0);
    buf[283] = OPTION_IP_ADDRESS_LEASE_TIME;
    buf[284] = 4;
    buf[285] = (self_p->offer.lease_time >> 24);
    buf[286] = (self_p->offer.lease_time >> 16);
    buf[287] = (self_p->offer.lease_time >> 8);
    buf[288] = (self_p->offer.lease_time >> 0);
    buf[289] = OPTION_END;

    return (broadcast_packet(self_p, &buf[0], sizeof(buf)));
}

static bool send_request(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[576];
    uint32_t transaction_id;

    memset(&buf[0], 0, sizeof(buf));
    transaction_id = 0x32493678;
    buf[0] = BOOTP_MESSAGE_TYPE_BOOT_REQUEST;
    buf[1] = HARDWARE_TYPE_ETHERNET;
    buf[2] = HARDWARE_ADDRESS_LENGTH;
    buf[4] = (transaction_id >> 24);
    buf[5] = (transaction_id >> 16);
    buf[6] = (transaction_id >> 8);
    buf[7] = (transaction_id >> 0);
    buf[10] = (uint8_t)(BOOT_FLAGS >> 8);
    buf[11] = (uint8_t)(BOOT_FLAGS >> 0);
    memcpy(&buf[28],
           &self_p->self.mac_address[0],
           sizeof(self_p->self.mac_address));
    buf[236] = (uint8_t)(MAGIC_COOKIE_DHCP >> 24);
    buf[237] = (uint8_t)(MAGIC_COOKIE_DHCP >> 16);
    buf[238] = (uint8_t)(MAGIC_COOKIE_DHCP >> 8);
    buf[239] = (uint8_t)(MAGIC_COOKIE_DHCP >> 0);
    buf[240] = OPTION_DHCP_MESSAGE_TYPE;
    buf[241] = 1;
    buf[242] = MESSAGE_TYPE_REQUEST;
    buf[243] = OPTION_DHCP_SERVER_IDENTIFIER;
    buf[244] = 4;
    buf[245] = (self_p->server.ip_address >> 24);
    buf[246] = (self_p->server.ip_address >> 16);
    buf[247] = (self_p->server.ip_address >> 8);
    buf[248] = (self_p->server.ip_address >> 0);
    buf[249] = OPTION_REQUSETED_IP_ADDRESS;
    buf[250] = 4;
    buf[251] = (self_p->offer.ip_address >> 24);
    buf[252] = (self_p->offer.ip_address >> 16);
    buf[253] = (self_p->offer.ip_address >> 8);
    buf[254] = (self_p->offer.ip_address >> 0);
    buf[255] = OPTION_IP_ADDRESS_LEASE_TIME;
    buf[256] = 4;
    buf[257] = (self_p->offer.lease_time >> 24);
    buf[258] = (self_p->offer.lease_time >> 16);
    buf[259] = (self_p->offer.lease_time >> 8);
    buf[260] = (self_p->offer.lease_time >> 0);
    buf[261] = OPTION_END;

    return (send_packet(self_p, &buf[0], sizeof(buf)));
}

static void change_state(struct ml_dhcp_client_t *self_p,
                         enum ml_dhcp_client_state_t state)
{
    ML_INFO("State change from %s to %s.",
            state_string(self_p->state),
            state_string(state));

    self_p->state = state;
}

static void enter_init(struct ml_dhcp_client_t *self_p)
{
    cancel_response_timer(self_p);
    cancel_rebinding_timer(self_p);
    set_init_timer(self_p, 10, 0);
    ml_close(self_p->fds[SOCK_IX].fd);
    setup_packet_socket(self_p);
    change_state(self_p, ml_dhcp_client_state_init_t);
}

static void enter_selecting(struct ml_dhcp_client_t *self_p)
{
    change_state(self_p, ml_dhcp_client_state_selecting_t);
}

static void enter_requesting(struct ml_dhcp_client_t *self_p)
{
    change_state(self_p, ml_dhcp_client_state_requesting_t);
}

static void enter_bound(struct ml_dhcp_client_t *self_p)
{
    cancel_response_timer(self_p);
    set_renewal_timer(self_p);
    set_rebinding_timer(self_p);
    configure_interface(self_p);
    ml_close(self_p->fds[SOCK_IX].fd);
    change_state(self_p, ml_dhcp_client_state_bound_t);

    if (setup_udp_socket(self_p) != 0) {
        enter_init(self_p);
    }
}

static void enter_renewing(struct ml_dhcp_client_t *self_p)
{
    change_state(self_p, ml_dhcp_client_state_renewing_t);
}

static void process_events_init(struct ml_dhcp_client_t *self_p)
{
    if (is_init_timeout(self_p)) {
        if (broadcast_discover(self_p)) {
            enter_selecting(self_p);
        } else {
            set_init_timer(self_p, 10, 0);
        }
    }
}

static void process_events_selecting(struct ml_dhcp_client_t *self_p)
{
    if (is_offer(self_p)) {
        if (broadcast_request(self_p)) {
            enter_requesting(self_p);
        } else {
            enter_init(self_p);
        }
    } else if (is_response_timeout(self_p)) {
        enter_init(self_p);
    }
}

static void process_events_requesting(struct ml_dhcp_client_t *self_p)
{
    if (is_ack(self_p)) {
        enter_bound(self_p);
    } else if (is_nak(self_p)) {
        enter_init(self_p);
    } else if (is_response_timeout(self_p)) {
        enter_init(self_p);
    }
}

static void process_events_bound(struct ml_dhcp_client_t *self_p)
{
    if (is_renewal_timeout(self_p)) {
        if (send_request(self_p)) {
            enter_renewing(self_p);
        } else {
            enter_init(self_p);
        }
    }
}

static void process_events_renewing(struct ml_dhcp_client_t *self_p)
{
    if (is_ack(self_p)) {
        enter_bound(self_p);
    } else if (is_nak(self_p)) {
        enter_init(self_p);
    } else if (is_response_timeout(self_p)) {
        enter_init(self_p);
    } else if (is_rebinding_timeout(self_p)) {
        /* Just go to init for now. */
        enter_init(self_p);
    }
}

static int setup_timer(struct ml_dhcp_client_t *self_p,
                       int index)
{
    int fd;

    fd = timerfd_create(CLOCK_REALTIME, 0);

    if (fd != -1) {
        init_pollfd(&self_p->fds[index], fd);
        fd = 0;
    }

    return (fd);
}

static int init(struct ml_dhcp_client_t *self_p)
{
    int res;

    res = ml_network_interface_index(self_p->interface.name_p,
                                     &self_p->interface.index);

    if (res != 0) {
        goto err1;
    }

    res = setup_packet_socket(self_p);

    if (res != 0) {
        goto err1;
    }

    res = setup_timer(self_p, RENEW_IX);

    if (res != 0) {
        goto err2;
    }

    res = setup_timer(self_p, REBIND_IX);

    if (res != 0) {
        goto err3;
    }

    res = setup_timer(self_p, RESP_IX);

    if (res != 0) {
        goto err4;
    }

    res = setup_timer(self_p, INIT_IX);

    if (res != 0) {
        goto err5;
    }

    return (res);

 err5:
    ml_close(self_p->fds[RESP_IX].fd);

 err4:
    ml_close(self_p->fds[REBIND_IX].fd);

 err3:
    ml_close(self_p->fds[RENEW_IX].fd);

 err2:
    ml_close(self_p->fds[SOCK_IX].fd);

 err1:

    return (res);
}

static void destroy(struct ml_dhcp_client_t *self_p)
{
    size_t i;

    for (i = 0; i < membersof(self_p->fds); i++) {
        ml_close(self_p->fds[i].fd);
    }
}

static void process_events(struct ml_dhcp_client_t *self_p)
{
    switch (self_p->state) {

    case ml_dhcp_client_state_init_t:
        process_events_init(self_p);
        break;

    case ml_dhcp_client_state_selecting_t:
        process_events_selecting(self_p);
        break;

    case ml_dhcp_client_state_requesting_t:
        process_events_requesting(self_p);
        break;

    case ml_dhcp_client_state_bound_t:
        process_events_bound(self_p);
        break;

    case ml_dhcp_client_state_renewing_t:
        process_events_renewing(self_p);
        break;

    default:
        break;
    }
}

static void *client_main(void *arg_p)
{
    int res;
    struct ml_dhcp_client_t *self_p;

    self_p = (struct ml_dhcp_client_t *)arg_p;

    while (true) {
        res = poll(&self_p->fds[0], membersof(self_p->fds), WAIT_FOREVER);

        if (res <= 0) {
            ML_INFO("Poll returned %d.", res);
            break;
        }

        update_events(self_p);
        process_events(self_p);
    }

    return (NULL);
}

void ml_dhcp_client_init(struct ml_dhcp_client_t *self_p,
                         const char *interface_name_p,
                         int log_mask)
{
    self_p->interface.name_p = interface_name_p;
    self_p->state = ml_dhcp_client_state_init_t;
    self_p->self.mac_address[0] = 1;
    self_p->self.mac_address[1] = 2;
    self_p->self.mac_address[2] = 3;
    self_p->self.mac_address[3] = 4;
    self_p->self.mac_address[4] = 5;
    self_p->self.mac_address[5] = 6;
    self_p->server.ip_address = 0x01020304;
    self_p->offer.ip_address = 0;
    self_p->offer.lease_time = 0;
    ml_log_object_init(&self_p->log_object,
                       "dhcp_client",
                       log_mask);
}

int ml_dhcp_client_start(struct ml_dhcp_client_t *self_p)
{
    int res;

    ML_INFO("Starting on interface '%s'.", self_p->interface.name_p);

    res = init(self_p);

    if (res == 0) {
        set_init_timer(self_p, 0, 1);

        res = pthread_create(&self_p->pthread, NULL, client_main, self_p);

        if (res != 0) {
            destroy(self_p);
        }
    }

    return (res);
}

void ml_dhcp_client_stop(struct ml_dhcp_client_t *self_p)
{
    destroy(self_p);
}

int ml_dhcp_client_join(struct ml_dhcp_client_t *self_p)
{
    return (pthread_join(self_p->pthread, NULL));
}
