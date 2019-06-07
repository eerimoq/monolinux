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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mock.h"
#include "mock_libc.h"

#include "mock_ml_network.h"

void mock_push_ml_network_init(void)
{
}

void __wrap_ml_network_init(void)
{
}

void mock_push_ml_network_interface_configure(const char *name_p,
                                              const char *ipv4_address_p,
                                              const char *ipv4_netmask_p,
                                              int res)
{
    mock_push("ml_network_interface_configure(name_p)",
              name_p,
              strlen(name_p) + 1);
    mock_push("ml_network_interface_configure(ipv4_address_p)",
              ipv4_address_p,
              strlen(ipv4_address_p) + 1);
    mock_push("ml_network_interface_configure(ipv4_netmask_p)",
              ipv4_netmask_p,
              strlen(ipv4_netmask_p) + 1);
    mock_push("ml_network_interface_configure(): return (res)",
              &res,
              sizeof(res));
}

int __wrap_ml_network_interface_configure(const char *name_p,
                                          const char *ipv4_address_p,
                                          const char *ipv4_netmask_p)
{
    int res;

    mock_pop_assert("ml_network_interface_configure(name_p)", name_p);
    mock_pop_assert("ml_network_interface_configure(ipv4_address_p)",
                    ipv4_address_p);
    mock_pop_assert("ml_network_interface_configure(ipv4_netmask_p)",
                    ipv4_netmask_p);
    mock_pop("ml_network_interface_configure(): return (res)", &res);

    return (res);
}

void mock_push_ml_network_interface_up(const char *name_p,
                                       int res)
{
    mock_push("ml_network_interface_up(name_p)", name_p, strlen(name_p) + 1);
    mock_push("ml_network_interface_up(): return (res)", &res, sizeof(res));
}

int __wrap_ml_network_interface_up(const char *name_p)
{
    int res;

    mock_pop_assert("ml_network_interface_up(name_p)", name_p);
    mock_pop("ml_network_interface_up(): return (res)", &res);

    return (res);
}

void mock_push_ml_network_interface_down(const char *name_p,
                                         int res)
{
    mock_push("ml_network_interface_down(name_p)", name_p, strlen(name_p) + 1);
    mock_push("ml_network_interface_down(): return (res)", &res, sizeof(res));
}

int __wrap_ml_network_interface_down(const char *name_p)
{
    int res;

    mock_pop_assert("ml_network_interface_down(name_p)", name_p);
    mock_pop("ml_network_interface_down(): return (res)", &res);

    return (res);
}

void mock_push_ml_network_interface_index(const char *name_p,
                                          int index,
                                          int res)
{
    mock_push("ml_network_interface_index(name_p)", name_p, strlen(name_p) + 1);
    mock_push("ml_network_interface_index(index_p)", &index, sizeof(index));
    mock_push("ml_network_interface_index(): return (res)", &res, sizeof(res));
}

int __wrap_ml_network_interface_index(const char *name_p, int *index_p)
{
    int res;

    mock_pop_assert("ml_network_interface_index(name_p)", name_p);
    mock_pop("ml_network_interface_index(index_p)", index_p);
    mock_pop("ml_network_interface_index(): return (res)", &res);

    return (res);
}

void mock_push_ml_network_interface_mac_address(const char *name_p,
                                                uint8_t *mac_address_p,
                                                int res)
{
    mock_push("ml_network_interface_mac_address(name_p)",
              name_p,
              strlen(name_p) + 1);
    mock_push("ml_network_interface_mac_address(mac_address_p)",
              mac_address_p,
              6);
    mock_push("ml_network_interface_mac_address(): return (res)",
              &res,
              sizeof(res));
}

int __wrap_ml_network_interface_mac_address(const char *name_p,
                                            uint8_t *mac_address_p)
{
    int res;

    mock_pop_assert("ml_network_interface_mac_address(name_p)", name_p);
    mock_pop("ml_network_interface_mac_address(mac_address_p)", mac_address_p);
    mock_pop("ml_network_interface_mac_address(): return (res)", &res);

    return (res);
}

void mock_push_ml_network_interface_add_route(const char *name_p,
                                              const char *ip_address_p,
                                              int res)
{
    mock_push("ml_network_interface_add_route(name_p)",
              name_p,
              strlen(name_p) + 1);
    mock_push("ml_network_interface_add_route(ip_address_p)",
              ip_address_p,
              strlen(ip_address_p) + 1);
    mock_push("ml_network_interface_add_route(): return (res)",
              &res,
              sizeof(res));
}

int __wrap_ml_network_interface_add_route(const char *name_p,
                                          const char *ip_address_p)
{
    int res;

    mock_pop_assert("ml_network_interface_add_route(name_p)", name_p);
    mock_pop_assert("ml_network_interface_add_route(ip_address_p)",
                    ip_address_p);
    mock_pop("ml_network_interface_add_route(): return (res)", &res);

    return (res);
}
