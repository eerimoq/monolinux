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

#include <unicorn/unicorn.h>
#include "ml/ml.h"
#include "utils/mocks/mock_libc.h"
#include "utils/mocks/mock_ml_shell.h"
#include "utils/mocks/mock.h"

static void mock_push_ml_network_init(void)
{
    mock_push_ml_shell_register_command("ifconfig",
                                        "Network interface management.");
    mock_push_ml_shell_register_command("udp_send",
                                        "UDP send.");
    mock_push_ml_shell_register_command("udp_recv",
                                        "UDP receive.");
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

static void mock_push_configure(const char *name_p)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;
    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], name_p);
    create_address_request(&ifreq, "192.168.0.4");
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFADDR, &ifreq);
    create_address_request(&ifreq, "255.255.255.0");
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFNETMASK, &ifreq);
    mock_push_ml_close(fd, 0);
}

static void mock_push_up(const char *name_p)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;
    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], name_p);
    mock_push_ioctl_ifreq_ok(fd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags = IFF_UP;
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFFLAGS, &ifreq);
    mock_push_ml_close(fd, 0);
}

static void mock_push_down(const char *name_p)
{
    int fd;
    struct ifreq ifreq;

    fd = 5;
    mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
    memset(&ifreq, 0, sizeof(ifreq));
    strcpy(&ifreq.ifr_name[0], name_p);
    mock_push_ioctl_ifreq_ok(fd, SIOCGIFFLAGS, &ifreq);
    mock_push_ioctl_ifreq_ok(fd, SIOCSIFFLAGS, &ifreq);
    mock_push_ml_close(fd, 0);
}

TEST(network_interface_configure)
{
    mock_push_ml_network_init();
    ml_network_init();

    mock_push_configure("eth0");

    ASSERT_EQ(ml_network_interface_configure("eth0",
                                             "192.168.0.4",
                                             "255.255.255.0"), 0);

    mock_finalize();
}

TEST(network_interface_up)
{
    mock_push_ml_network_init();
    ml_network_init();

    mock_push_up("eth0");

    ASSERT_EQ(ml_network_interface_up("eth0"), 0);

    mock_finalize();
}

TEST(network_interface_down)
{
    mock_push_ml_network_init();
    ml_network_init();

    mock_push_down("eth0");

    ASSERT_EQ(ml_network_interface_down("eth0"), 0);

    mock_finalize();
}

TEST(command_ifconfig_no_args)
{
    ml_shell_command_callback_t command_ifconfig;
    const char *argv[] = { "ifconfig" };

    mock_push_ml_network_init();
    ml_network_init();

    command_ifconfig = mock_get_callback("ifconfig");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_ifconfig(membersof(argv), argv), -1);
    }

    ASSERT_EQ(output,
              "ifconfig <interface> <ip-address> <netmask>\n"
              "ifconfig <interface> up/down\n");

    mock_finalize();
}

TEST(command_ifconfig_configure)
{
    ml_shell_command_callback_t command_ifconfig;
    const char *argv[] = { "ifconfig", "eth2", "192.168.0.4", "255.255.255.0" };

    mock_push_ml_network_init();
    ml_network_init();

    command_ifconfig = mock_get_callback("ifconfig");

    mock_push_configure("eth2");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_ifconfig(membersof(argv), argv), 0);
    }

    ASSERT_EQ(output, "");

    mock_finalize();
}

TEST(command_ifconfig_up)
{
    ml_shell_command_callback_t command_ifconfig;
    const char *argv[] = { "ifconfig", "eth2", "up" };

    mock_push_ml_network_init();
    ml_network_init();

    command_ifconfig = mock_get_callback("ifconfig");

    mock_push_up("eth2");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_ifconfig(membersof(argv), argv), 0);
    }

    ASSERT_EQ(output, "");

    mock_finalize();
}

TEST(command_ifconfig_down)
{
    ml_shell_command_callback_t command_ifconfig;
    const char *argv[] = { "ifconfig", "eth1", "down" };

    mock_push_ml_network_init();
    ml_network_init();

    command_ifconfig = mock_get_callback("ifconfig");

    mock_push_down("eth1");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_ifconfig(membersof(argv), argv), 0);
    }

    ASSERT_EQ(output, "");

    mock_finalize();
}

TEST(command_ifconfig_foobar)
{
    ml_shell_command_callback_t command_ifconfig;
    const char *argv[] = { "ifconfig", "eth1", "foobar" };

    mock_push_ml_network_init();
    ml_network_init();

    command_ifconfig = mock_get_callback("ifconfig");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_ifconfig(membersof(argv), argv), -1);
    }

    ASSERT_EQ(output,
              "ifconfig <interface> <ip-address> <netmask>\n"
              "ifconfig <interface> up/down\n");

    mock_finalize();
}

TEST(command_udp_send_no_args)
{
    ml_shell_command_callback_t command_udp_send;
    const char *argv[] = { "udp_send" };

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_send = mock_get_callback("udp_send");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_udp_send(membersof(argv), argv), -1);
    }

    ASSERT_EQ(output, "udp_send <ip-address> <port> <data>\n");

    mock_finalize();
}

TEST(command_udp_send_bad_ip_address)
{
    ml_shell_command_callback_t command_udp_send;
    const char *argv[] = { "udp_send", "b.b.c.d", "9999", "Hello" };

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_send = mock_get_callback("udp_send");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_udp_send(membersof(argv), argv), -1);
    }

    ASSERT_SUBSTRING(output, "udp_send <ip-address> <port> <data>\n");

    mock_finalize();
}

TEST(command_udp_send_open_socket_failure)
{
    ml_shell_command_callback_t command_udp_send;
    const char *argv[] = { "udp_send", "1.2.3.4", "9999", "Hello" };

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_send = mock_get_callback("udp_send");

    CAPTURE_OUTPUT(output) {
        mock_push_socket(AF_INET, SOCK_DGRAM, 0, -1);
        ASSERT_EQ(command_udp_send(membersof(argv), argv), -1);
    }

    ASSERT_SUBSTRING(output, "error: socket:");
    ASSERT_SUBSTRING(output, "udp_send <ip-address> <port> <data>\n");

    mock_finalize();
}

TEST(command_udp_send_sendto_failure)
{
    int fd;
    ml_shell_command_callback_t command_udp_send;
    const char *argv[] = { "udp_send", "1.2.3.4", "1234", "Hello!" };
    struct sockaddr_in other;

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_send = mock_get_callback("udp_send");
    fd = 9;

    CAPTURE_OUTPUT(output) {
        mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
        memset(&other, 0, sizeof(other));
        other.sin_family = AF_INET;
        other.sin_port = htons(1234);
        inet_aton("1.2.3.4", &other.sin_addr);
        mock_push_sendto(fd,
                         "Hello!",
                         6,
                         (struct sockaddr *)&other,
                         sizeof(other),
                         -1);
        mock_push_ml_close(fd, 0);
        ASSERT_EQ(command_udp_send(membersof(argv), argv), -1);
    }

    ASSERT_SUBSTRING(output, "sendto failed:");
    ASSERT_SUBSTRING(output, "udp_send <ip-address> <port> <data>\n");

    mock_finalize();
}

TEST(command_udp_send)
{
    int fd;
    ml_shell_command_callback_t command_udp_send;
    const char *argv[] = { "udp_send", "1.2.3.4", "1234", "Hello!" };
    struct sockaddr_in other;

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_send = mock_get_callback("udp_send");
    fd = 9;

    CAPTURE_OUTPUT(output) {
        mock_push_socket(AF_INET, SOCK_DGRAM, 0, fd);
        memset(&other, 0, sizeof(other));
        other.sin_family = AF_INET;
        other.sin_port = htons(1234);
        inet_aton("1.2.3.4", &other.sin_addr);
        mock_push_sendto(fd,
                         "Hello!",
                         6,
                         (struct sockaddr *)&other,
                         sizeof(other),
                         6);
        mock_push_ml_close(fd, 0);
        ASSERT_EQ(command_udp_send(membersof(argv), argv), 0);
    }

    ASSERT_EQ(output, "");

    mock_finalize();
}

TEST(command_udp_recv_no_args)
{
    ml_shell_command_callback_t command_udp_recv;
    const char *argv[] = { "udp_recv" };

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_recv = mock_get_callback("udp_recv");

    CAPTURE_OUTPUT(output) {
        ASSERT_EQ(command_udp_recv(membersof(argv), argv), -1);
    }

    ASSERT_EQ(output, "udp_recv <port> [<timeout in seconds>]\n");

    mock_finalize();
}

TEST(command_udp_recv_open_socket_failure)
{
    ml_shell_command_callback_t command_udp_recv;
    const char *argv[] = { "udp_recv", "9999" };

    mock_push_ml_network_init();
    ml_network_init();

    command_udp_recv = mock_get_callback("udp_recv");

    CAPTURE_OUTPUT(output) {
        mock_push_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, -1);
        ASSERT_EQ(command_udp_recv(membersof(argv), argv), -1);
    }

    ASSERT_SUBSTRING(output, "error: socket:");
    ASSERT_SUBSTRING(output, "udp_recv <port> [<timeout in seconds>]\n");

    mock_finalize();
}

int main()
{
    ml_init();
    ml_shell_init();

    return RUN_TESTS(
        network_interface_configure,
        network_interface_up,
        network_interface_down,
        command_ifconfig_no_args,
        command_ifconfig_configure,
        command_ifconfig_up,
        command_ifconfig_down,
        command_ifconfig_foobar,
        command_udp_send_no_args,
        command_udp_send_bad_ip_address,
        command_udp_send_open_socket_failure,
        command_udp_send_sendto_failure,
        command_udp_send,
        command_udp_recv_no_args,
        command_udp_recv_open_socket_failure
    );
}
