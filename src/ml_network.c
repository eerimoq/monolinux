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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "ml.h"

static void xioctl(int fd, unsigned long request, void *data_p)
{
    int res;

    res = ioctl(fd, request, data_p);

    if (res != 0) {
        perror("error: ioctl");
        exit(1);
    }
}

static int net_open(const char *name_p,
                    struct ifreq *ifreq_p)
{
    int netfd;

    netfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (netfd == -1) {
        perror("error: open network socket");
        exit(1);
    }

    strncpy(&ifreq_p->ifr_name[0], name_p, sizeof(ifreq_p->ifr_name) - 1);
    ifreq_p->ifr_name[sizeof(ifreq_p->ifr_name) - 1] = '\0';

    return (netfd);
}

static void net_close(int netfd)
{
    close(netfd);
}

static void create_address_request(struct ifreq *ifreq_p,
                                   const char *address_p)
{
    struct sockaddr_in sai;

    memset(&sai, 0, sizeof(sai));
    sai.sin_family = AF_INET;
    sai.sin_port = 0;
    sai.sin_addr.s_addr = inet_addr(address_p);
    memcpy(&ifreq_p->ifr_addr, &sai, sizeof(ifreq_p->ifr_addr));
}

static void up(int netfd, struct ifreq *ifreq_p)
{
    xioctl(netfd, SIOCGIFFLAGS, ifreq_p);
    ifreq_p->ifr_flags |= IFF_UP;
    xioctl(netfd, SIOCSIFFLAGS, ifreq_p);
}

static void down(int netfd, struct ifreq *ifreq_p)
{
    xioctl(netfd, SIOCGIFFLAGS, ifreq_p);
    ifreq_p->ifr_flags &= ~IFF_UP;
    xioctl(netfd, SIOCSIFFLAGS, ifreq_p);
}

static void set_ip_address(int netfd,
                           struct ifreq *ifreq_p,
                           const char *address_p)
{
    create_address_request(ifreq_p, address_p);
    xioctl(netfd, SIOCSIFADDR, ifreq_p);
}

static void set_netmask(int netfd,
                        struct ifreq *ifreq_p,
                        const char *netmask_p)
{
    create_address_request(ifreq_p, netmask_p);
    xioctl(netfd, SIOCSIFNETMASK, ifreq_p);
}

static int command_ifconfig(int argc, const char *argv[])
{
    int res;

    res = 0;

    if (argc == 3) {
        if (strcmp(argv[2], "up") == 0) {
            ml_network_interface_up(argv[1]);
        } else if (strcmp(argv[2], "down") == 0) {
            ml_network_interface_down(argv[1]);
        } else {
            res = -1;
        }
    } else if (argc == 4) {
        ml_network_interface_configure(argv[1], argv[2], argv[3]);
    } else {
        res = -1;
    }

    if (res != 0) {
        printf("ifconfig <interface> <ip-address> <netmask>\n"
               "ifconfig <interface> up/down\n");
    }

    return (res);
}

static int command_udp_send(const char *ip_address_p,
                            const char *port_p,
                            const char *data_p)
{
    int res;
    ssize_t size;
    int sockfd;
    struct sockaddr_in other;

    res = -1;
    memset(&other, 0, sizeof(other));
    other.sin_family = AF_INET;
    other.sin_port = htons(atoi(port_p));

    if (inet_aton(ip_address_p, &other.sin_addr) != 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        if (sockfd != -1) {
            size = sendto(sockfd,
                          data_p,
                          strlen(data_p),
                          0,
                          (struct sockaddr *)&other,
                          sizeof(other));

            if (size != -1) {
                res = 0;
            } else {
                perror("sendto failed");
            }

            close(sockfd);
        } else {
            perror("socket creation failed");
        }
    }

    return (res);
}

static int command_udp_recv(const char *port_p)
{
    int res;
    ssize_t size;
    int sockfd;
    socklen_t len;
    struct sockaddr_in me;
    struct sockaddr_in other;
    char buf[256];

    res = -1;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd != -1) {
        memset(&me, 0, sizeof(me));
        me.sin_family = AF_INET;
        me.sin_port = htons(atoi(port_p));
        me.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockfd, (struct sockaddr *)&me, sizeof(me)) != -1) {
            len = sizeof(other);
            size = recvfrom(sockfd,
                            &buf[0],
                            sizeof(buf) - 1,
                            0,
                            (struct sockaddr *)&other,
                            &len);

            if (size != -1) {
                buf[size] = '\0';
                printf("Received packet from %s:%d\n"
                       "Data: %s\n",
                       inet_ntoa(other.sin_addr),
                       ntohs(other.sin_port),
                       &buf[0]);
                res = 0;
            } else {
                perror("recvfrom failed");
            }
        } else {
            perror("bind failed");
        }

        close(sockfd);
    } else {
        perror("socket creation failed");
    }

    return (res);
}

static int command_udp(int argc, const char *argv[])
{
    int res;

    res = -1;

    if (argc == 5) {
        if (strcmp(argv[1], "send") == 0) {
            res = command_udp_send(argv[2], argv[3], argv[4]);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "recv") == 0) {
            res = command_udp_recv(argv[2]);
        }
    }

    if (res != 0) {
        printf("udp send <ip-address> <port> <data>\n"
               "udp recv <port>\n");
    }

    return (res);
}

void ml_network_init(void)
{
    ml_shell_register_command("ifconfig",
                              "Network interface management.",
                              command_ifconfig);
    ml_shell_register_command("udp",
                              "UDP testing.",
                              command_udp);
}

void ml_network_interface_configure(const char *name_p,
                                    const char *ipv4_address_p,
                                    const char *ipv4_netmask_p)
{
    struct ifreq ifreq;
    int netfd;

    netfd = net_open(name_p, &ifreq);
    set_ip_address(netfd, &ifreq, ipv4_address_p);
    set_netmask(netfd, &ifreq, ipv4_netmask_p);
    net_close(netfd);
}

void ml_network_interface_up(const char *name_p)
{
    struct ifreq ifreq;
    int netfd;

    netfd = net_open(name_p, &ifreq);
    up(netfd, &ifreq);
    net_close(netfd);
}

void ml_network_interface_down(const char *name_p)
{
    struct ifreq ifreq;
    int netfd;

    netfd = net_open(name_p, &ifreq);
    down(netfd, &ifreq);
    net_close(netfd);
}
