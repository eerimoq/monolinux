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
#include "mock.h"
#include "mock_libc.h"

void mock_push_mount(const char *source_p,
                     const char *target_p,
                     const char *type_p,
                     unsigned long flags,
                     const void *data_p,
                     size_t data_size,
                     int res)
{
    mock_push("mount(source_p)", source_p, strlen(source_p) + 1);
    mock_push("mount(target_p)", target_p, strlen(target_p) + 1);
    mock_push("mount(type_p)", type_p, strlen(type_p) + 1);
    mock_push("mount(flags)", &flags, sizeof(flags));
    mock_push("mount(data_p)", data_p, data_size);
    mock_push("mount(): return (res)", &res, sizeof(res));
}

int __wrap_mount(const char *source_p,
                 const char *target_p,
                 const char *type_p,
                 unsigned long flags,
                 const void *data_p)
{
    int res;

    mock_pop_assert("mount(source_p)", source_p);
    mock_pop_assert("mount(target_p)", target_p);
    mock_pop_assert("mount(type_p)", type_p);
    mock_pop_assert("mount(flags)", &flags);
    mock_pop_assert("mount(data_p)", data_p);
    mock_pop("mount(): return (res)", &res);

    return (res);
}

void mock_push_socket(int domain, int type, int protocol, int res)
{
    mock_push("socket(domain)", &domain, sizeof(domain));
    mock_push("socket(type)", &type, sizeof(type));
    mock_push("socket(protocol)", &protocol, sizeof(protocol));
    mock_push("socket(): return (res)", &res, sizeof(res));
}

int __wrap_socket(int domain, int type, int protocol)
{
    int res;

    mock_pop_assert("socket(domain)", &domain);
    mock_pop_assert("socket(type)", &type);
    mock_pop_assert("socket(protocol)", &protocol);
    mock_pop("socket(): return (res)", &res);

    return (res);
}

void mock_push_ioctl(int fd,
                     unsigned long request,
                     void *data_p,
                     size_t data_size,
                     int res)
{
    mock_push("ioctl(fd)", &fd, sizeof(fd));
    mock_push("ioctl(request)", &request, sizeof(request));
    mock_push("ioctl(data_p)", data_p, data_size);
    mock_push("ioctl(): return (res)", &res, sizeof(res));
}

void mock_push_ioctl_ifreq_ok(int fd,
                              unsigned long request,
                              struct ifreq *ifreq_p)
{
    mock_push_ioctl(fd, request, ifreq_p, sizeof(*ifreq_p), 0);
}

int __wrap_ioctl(int fd,
                 unsigned long request,
                 void *data_p)
{
    int res;

    mock_pop_assert("ioctl(fd)", &fd);
    mock_pop_assert("ioctl(request)", &request);
    mock_pop_assert("ioctl(data_p)", data_p);
    mock_pop("ioctl(): return (res)", &res);

    return (res);
}
