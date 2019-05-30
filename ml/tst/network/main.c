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

#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unicorn/unicorn.h>
#include "ml/ml.h"
#include "mock_libc.h"

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

TEST(network_interface_configure)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;

    ml_network_init();

    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], "eth0");
    create_address_request(&ifreq, "192.168.0.4");
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFADDR, &ifreq);
    create_address_request(&ifreq, "255.255.255.0");
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFNETMASK, &ifreq);

    ml_network_interface_configure("eth0", "192.168.0.4", "255.255.255.0");
}

TEST(network_interface_up)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;

    ml_network_init();

    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], "eth0");
    mock_push_ioctl_ifreq_ok(fd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags = IFF_UP;
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFFLAGS, &ifreq);

    ml_network_interface_up("eth0");
}

TEST(network_interface_down)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;

    ml_network_init();

    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], "eth0");
    mock_push_ioctl_ifreq_ok(fd, SIOCGIFFLAGS, &ifreq);
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFFLAGS, &ifreq);

    ml_network_interface_down("eth0");
}

int main()
{
    ml_init();
    ml_shell_init();

    return RUN_TESTS(
        network_interface_configure,
        network_interface_up,
        network_interface_down
    );
}
