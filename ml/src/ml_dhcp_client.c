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
#include <netinet/ip.h>
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
#define MAGIC_COOKIE_DHCP_0                       0x63
#define MAGIC_COOKIE_DHCP_1                       0x82
#define MAGIC_COOKIE_DHCP_2                       0x53
#define MAGIC_COOKIE_DHCP_3                       0x63

/* DHCP option identifiers. */
#define OPTION_SUBNET_MASK                        1
#define OPTION_ROUTER                             3
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

#define MAGIC_COOKIE_DHCP_OFFSET                  236
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

struct option_in_addr_t {
    struct in_addr value;
    bool valid;
};

struct options_t {
    struct option_u8_t message_type;
    struct option_in_addr_t subnet_mask;
    struct option_in_addr_t gateway;
    struct option_in_addr_t dns;
    struct option_u32_t lease_time;
    struct option_u32_t renewal_time;
    struct option_u32_t rebinding_time;
    struct option_in_addr_t server_ip_address;
};

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

static int unpack_option_in_addr(struct option_in_addr_t *option_p,
                                 const uint8_t *buf_p,
                                 uint8_t size)
{
    if (size != 4) {
        return (-1);
    }

    option_p->value.s_addr = ntohl((buf_p[0] << 24)
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
        res = unpack_option_in_addr(&options_p->subnet_mask, buf_p, length);
        break;

    case OPTION_ROUTER:
        res = unpack_option_in_addr(&options_p->gateway, buf_p, length);
        break;

    case OPTION_DOMAIN_NAME_SERVER:
        if (length > 4) {
            length = 4;
        }

        res = unpack_option_in_addr(&options_p->dns, buf_p, length);
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
        res = unpack_option_in_addr(&options_p->server_ip_address, buf_p, length);
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
                              FILE *file_p)
{
    ssize_t res;
    int option;
    int length;
    uint8_t buf[255];

    memset(options_p, 0, sizeof(*options_p));

    while (true) {
        option = fgetc(file_p);

        if (option == EOF) {
            return (-1);
        }

        if (option == OPTION_END) {
            break;
        }

        length = fgetc(file_p);

        if (length == EOF) {
            return (-1);
        }

        res = fread(&buf[0], length, 1, file_p);

        if (res != 1) {
            return (-1);
        }

        res = unpack_option(self_p, options_p, option, &buf[0], length);

        if (res != 0) {
            return (res);
        }
    }

    ML_INFO("Options:");

    if (options_p->message_type.valid) {
        ML_INFO("  Message Type:      %u", options_p->message_type.value);
    }

    if (options_p->subnet_mask.valid) {
        ML_INFO("  Subnet Mask:       %s",
                inet_ntoa(options_p->subnet_mask.value));
    }

    if (options_p->gateway.valid) {
        ML_INFO("  Gateway:           %s", inet_ntoa(options_p->gateway.value));
    }

    if (options_p->dns.valid) {
        ML_INFO("  DNS:               %s", inet_ntoa(options_p->dns.value));
    }

    if (options_p->lease_time.valid) {
        ML_INFO("  Lease Time:        %u", options_p->lease_time.value);
    }

    if (options_p->renewal_time.valid) {
        ML_INFO("  Renewal Time:      %u", options_p->renewal_time.value);
    }

    if (options_p->rebinding_time.valid) {
        ML_INFO("  Rebinding Time:    %u", options_p->rebinding_time.value);
    }

    if (options_p->server_ip_address.valid) {
        ML_INFO("  Server IP Address: %s",
                inet_ntoa(options_p->server_ip_address.value));
    }

    return (0);
}

static int unpack_options(struct ml_dhcp_client_t *self_p,
                          struct options_t *options_p,
                          const uint8_t *buf_p,
                          size_t size)
{
    FILE *file_p;
    int res;

    res = -1;
    file_p = fmemopen((uint8_t *)buf_p, size, "r");

    if (file_p != NULL) {
        res = unpack_all_options(self_p, options_p, file_p);
        fclose(file_p);
    }

    return (res);
}

static void unpack_message_type_offer(struct ml_dhcp_client_t *self_p,
                                      struct options_t *options_p,
                                      const uint8_t *buf_p)
{
    if (!(options_p->server_ip_address.valid
          && options_p->lease_time.valid
          && (options_p->lease_time.value >= 1)
          && options_p->subnet_mask.valid
          && options_p->gateway.valid
          && options_p->dns.valid)) {
        return;
    }

    self_p->ip_address.s_addr = ntohl((buf_p[0] << 24)
                                      | (buf_p[1] << 16)
                                      | (buf_p[2] << 8)
                                      | (buf_p[3] << 0));
    self_p->server.ip_address = options_p->server_ip_address.value;
    self_p->subnet_mask = options_p->subnet_mask.value;
    self_p->gateway = options_p->gateway.value;
    self_p->dns = options_p->dns.value;
    self_p->lease_time = options_p->lease_time.value;
    self_p->renewal_interval = (self_p->lease_time / 2);
    self_p->rebinding_time = (self_p->lease_time / 2 + 10);
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

static bool is_ip_valid(const uint8_t *buf_p, size_t size)
{
    return ((size >= 20)
            && (ml_inet_checksum(&buf_p[0], 20) == 0)
            && (buf_p[0] == ((IPVERSION << 4) | 5))
            && (buf_p[9] == IPPROTO_UDP)
            && (buf_p[16] == 255)
            && (buf_p[17] == 255)
            && (buf_p[18] == 255)
            && (buf_p[19] == 255));
}

static bool is_udp_valid(const uint8_t *buf_p, size_t size)
{
    /* ToDo: Validate checksum. */
    return ((size >= 28)
            && (buf_p[0] == (uint8_t)(SERVER_PORT >> 8))
            && (buf_p[1] == (uint8_t)SERVER_PORT)
            && (buf_p[2] == (uint8_t)(CLIENT_PORT >> 8))
            && (buf_p[3] == (uint8_t)CLIENT_PORT));
}

static bool is_packet_valid(const uint8_t *buf_p, size_t size)
{
    return (is_ip_valid(buf_p, size) && is_udp_valid(&buf_p[20], size));
}

static void unpack_packet(struct ml_dhcp_client_t *self_p,
                          const uint8_t *buf_p,
                          size_t size)
{
    int res;
    struct options_t options;

    if (self_p->is_packet_socket) {
        if (!is_packet_valid(buf_p, size)) {
            return;
        }

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
                 &self_p->interface.mac_address[0],
                 sizeof(self_p->interface.mac_address));

    if (res != 0) {
        return;
    }

    if ((buf_p[MAGIC_COOKIE_DHCP_OFFSET] != MAGIC_COOKIE_DHCP_0)
        || (buf_p[MAGIC_COOKIE_DHCP_OFFSET + 1] != MAGIC_COOKIE_DHCP_1)
        || (buf_p[MAGIC_COOKIE_DHCP_OFFSET + 2] != MAGIC_COOKIE_DHCP_2)
        || (buf_p[MAGIC_COOKIE_DHCP_OFFSET + 3] != MAGIC_COOKIE_DHCP_3)) {
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
    char subnet_mask[16];
    char gateway[16];

    strcpy(&ip_address[0], inet_ntoa(self_p->ip_address));
    strcpy(&subnet_mask[0], inet_ntoa(self_p->subnet_mask));
    strcpy(&gateway[0], inet_ntoa(self_p->gateway));

    ML_INFO(
        "Configuring interface '%s' with ip address %s, "
        "subnet mask %s and gateway %s.",
        self_p->interface.name_p,
        &ip_address[0],
        &subnet_mask[0],
        &gateway[0]);

    res = ml_network_interface_configure(self_p->interface.name_p,
                                         &ip_address[0],
                                         &subnet_mask[0]);

    if (res != 0) {
        ML_WARNING("Failed to configure '%s' with ip address '%s'.",
                   self_p->interface.name_p,
                   &ip_address[0]);
    }

    res = ml_network_interface_add_route(self_p->interface.name_p,
                                         &gateway[0]);

    if (res != 0) {
        ML_WARNING("Failed to add route %s to interface '%s'.",
                   &gateway[0],
                   self_p->interface.name_p);
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

    if (self_p->fds[SOCK_IX].revents & POLLERR) {
        ML_WARNING("Packet/UDP socket error. Interface likely down.");
        sleep(1);
    } else if (self_p->fds[SOCK_IX].revents & POLLIN) {
        size = ml_read(self_p->fds[SOCK_IX].fd, &buf[0], sizeof(buf));

        if (size > 0) {
            unpack_packet(self_p, &buf[0], size);
        }

        ML_INFO("Received %s packet.", packet_type_str(self_p->packet_type));
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
    return (set_timer(self_p->fds[RENEW_IX].fd,
                      self_p->renewal_interval,
                      0));
}

static int set_rebinding_timer(struct ml_dhcp_client_t *self_p)
{
    return (set_timer(self_p->fds[REBIND_IX].fd,
                      self_p->rebinding_time,
                      0));
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
                        const uint8_t *buf_p,
                        size_t size)
{
    bool ok;
    ssize_t res;
    struct sockaddr_in addr;

    ok = false;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = self_p->server.ip_address;
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
    addr.sll_halen = 6;
    memset(&addr.sll_addr[0], 0xff, addr.sll_halen);

    ML_DEBUG("Broadcasting %s packet.",
             message_type_str(buf_p[28 + OPTIONS_OFFSET + 2]));

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
    uint8_t identifier;

    buf_p[0] = ((IPVERSION << 4) | 5);
    buf_p[2] = (uint8_t)(size >> 8);
    buf_p[3] = (uint8_t)size;
    identifier = 0x11;
    buf_p[5] = identifier;
    buf_p[8] = MAXTTL;
    buf_p[9] = IPPROTO_UDP;
    buf_p[16] = 255;
    buf_p[17] = 255;
    buf_p[18] = 255;
    buf_p[19] = 255;
    checksum = ml_inet_checksum(&buf_p[0], 20);
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

static void pack_dhcp_fixed(uint8_t *buf_p,
                            uint8_t *mac_address_p,
                            uint8_t message_type)
{
    uint32_t transaction_id;

    transaction_id = 0x32493678;
    buf_p[0] = BOOTP_MESSAGE_TYPE_BOOT_REQUEST;
    buf_p[1] = HARDWARE_TYPE_ETHERNET;
    buf_p[2] = HARDWARE_ADDRESS_LENGTH;
    buf_p[4] = (transaction_id >> 24);
    buf_p[5] = (transaction_id >> 16);
    buf_p[6] = (transaction_id >> 8);
    buf_p[7] = (transaction_id >> 0);
    buf_p[10] = (uint8_t)(BOOT_FLAGS >> 8);
    buf_p[11] = (uint8_t)(BOOT_FLAGS >> 0);
    memcpy(&buf_p[28], mac_address_p, 6);
    buf_p[236] = MAGIC_COOKIE_DHCP_0;
    buf_p[237] = MAGIC_COOKIE_DHCP_1;
    buf_p[238] = MAGIC_COOKIE_DHCP_2;
    buf_p[239] = MAGIC_COOKIE_DHCP_3;
    buf_p[240] = OPTION_DHCP_MESSAGE_TYPE;
    buf_p[241] = 1;
    buf_p[242] = message_type;
}

static void pack_dhcp_request(uint8_t *buf_p,
                              struct ml_dhcp_client_t *self_p)
{
    uint32_t value;

    buf_p[0] = OPTION_DHCP_SERVER_IDENTIFIER;
    buf_p[1] = 4;
    value = htonl(self_p->server.ip_address.s_addr);
    buf_p[2] = (value >> 24);
    buf_p[3] = (value >> 16);
    buf_p[4] = (value >> 8);
    buf_p[5] = (value >> 0);
    buf_p[6] = OPTION_REQUSETED_IP_ADDRESS;
    buf_p[7] = 4;
    value = htonl(self_p->ip_address.s_addr);
    buf_p[8] = (value >> 24);
    buf_p[9] = (value >> 16);
    buf_p[10] = (value >> 8);
    buf_p[11] = (value >> 0);
    buf_p[12] = OPTION_IP_ADDRESS_LEASE_TIME;
    buf_p[13] = 4;
    buf_p[14] = (self_p->lease_time >> 24);
    buf_p[15] = (self_p->lease_time >> 16);
    buf_p[16] = (self_p->lease_time >> 8);
    buf_p[17] = (self_p->lease_time >> 0);
    buf_p[18] = OPTION_END;
}

static bool broadcast_discover(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[604];

    memset(&buf[0], 0, sizeof(buf));
    pack_ip_header(&buf[0], sizeof(buf));
    pack_udp_header(&buf[20], 584);
    pack_dhcp_fixed(&buf[28],
                    &self_p->interface.mac_address[0],
                    MESSAGE_TYPE_DISCOVER);
    buf[271] = OPTION_MAXIMUM_DHCP_MESSAGE_SIZE;
    buf[272] = 2;
    buf[273] = (uint8_t)(MAXIMUM_PACKET_SIZE >> 8);
    buf[274] = (uint8_t)(MAXIMUM_PACKET_SIZE >> 0);
    buf[275] = OPTION_PARAMETER_REQUEST_LIST;
    buf[276] = 3;
    buf[277] = OPTION_SUBNET_MASK;
    buf[278] = OPTION_DOMAIN_NAME_SERVER;
    buf[279] = OPTION_ROUTER;
    buf[280] = OPTION_END;

    return (broadcast_packet(self_p, &buf[0], sizeof(buf)));
}

static bool broadcast_request(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[604];

    memset(&buf[0], 0, sizeof(buf));
    pack_ip_header(&buf[0], sizeof(buf));
    pack_udp_header(&buf[20], 584);
    pack_dhcp_fixed(&buf[28],
                    &self_p->interface.mac_address[0],
                    MESSAGE_TYPE_REQUEST);
    pack_dhcp_request(&buf[271], self_p);

    return (broadcast_packet(self_p, &buf[0], sizeof(buf)));
}

static bool send_request(struct ml_dhcp_client_t *self_p)
{
    uint8_t buf[576];

    memset(&buf[0], 0, sizeof(buf));
    pack_dhcp_fixed(&buf[0],
                    &self_p->interface.mac_address[0],
                    MESSAGE_TYPE_REQUEST);
    pack_dhcp_request(&buf[243], self_p);

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

    res = ml_network_interface_mac_address(self_p->interface.name_p,
                                           &self_p->interface.mac_address[0]);

    if (res != 0) {
        goto err1;
    }

    ML_INFO("Interface information:");
    ML_INFO("  MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
            self_p->interface.mac_address[0],
            self_p->interface.mac_address[1],
            self_p->interface.mac_address[2],
            self_p->interface.mac_address[3],
            self_p->interface.mac_address[4],
            self_p->interface.mac_address[5]);
    ML_INFO("  Index:       %d", self_p->interface.index);

    res = setup_packet_socket(self_p);

    if (res != 0) {
        goto err1;
    }

    res = setup_timer(self_p, RENEW_IX);

    if (res != 0) {
        perror("timer");
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
