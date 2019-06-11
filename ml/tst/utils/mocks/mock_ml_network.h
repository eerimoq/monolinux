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

#ifndef MOCK_ML_NETWORK_H
#define MOCK_ML_NETWORK_H

void mock_push_ml_network_init(void);

void mock_push_ml_network_interface_configure(const char *name_p,
                                              const char *ipv4_address_p,
                                              const char *ipv4_netmask_p,
                                              int res);

void mock_push_ml_network_interface_up(const char *name_p,
                                       int res);

void mock_push_ml_network_interface_down(const char *name_p,
                                         int res);

void mock_push_ml_network_interface_ip_address(const char *name_p,
                                               struct in_addr *addr_p,
                                               int res);

void mock_push_ml_network_interface_index(const char *name_p,
                                          int index,
                                          int res);

void mock_push_ml_network_interface_mac_address(const char *name_p,
                                                uint8_t *mac_address_p,
                                                int res);

void mock_push_ml_network_interface_add_route(const char *name_p,
                                              const char *ip_address_p,
                                              int res);

#endif
