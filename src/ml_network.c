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

void ml_network_interface_configure(const char *name_p,
                                    const char *ipv4_address_p,
                                    const char *ipv4_netmask_p)
{
    struct ifreq ifreq;
    int netfd;

    netfd = net_open(name_p, &ifreq);
    down(netfd, &ifreq);
    set_ip_address(netfd, &ifreq, ipv4_address_p);
    set_netmask(netfd, &ifreq, ipv4_netmask_p);
    up(netfd, &ifreq);
    net_close(netfd);
}
