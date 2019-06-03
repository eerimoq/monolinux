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

/* Needed by ftw. */
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unicorn/unicorn.h>
#include "mock.h"
#include "mock_libc.h"

/* socket() and close() may be used by unicorn. Should not use wrap,
   but instead only replace symbols in tested files. Possibly examine
   the backtrace. Both variants are dirty. */

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

void mock_push_sendto(int fd,
                      const void *buf_p,
                      size_t len,
                      const struct sockaddr_in *dest_addr_p,
                      ssize_t res)
{
    mock_push("sendto(fd)", &fd, sizeof(fd));
    mock_push("sendto(buf_p)", buf_p, len);
    mock_push("sendto(len)", &len, sizeof(len));
    mock_push("sendto(dest_addr_p)", dest_addr_p, sizeof(*dest_addr_p));
    mock_push("sendto(): return (res)", &res, sizeof(res));
}

ssize_t __wrap_sendto(int fd,
                      const void *buf_p,
                      size_t len,
                      int flags,
                      const struct sockaddr *dest_addr_p,
                      socklen_t addrlen)
{
    ssize_t res;

    mock_pop_assert("sendto(fd)", &fd);
    mock_pop_assert("sendto(buf_p)", buf_p);
    mock_pop_assert("sendto(len)", &len);
    ASSERT_EQ(flags, 0);
    ASSERT_EQ(addrlen, sizeof(*dest_addr_p));
    mock_pop_assert("sendto(dest_addr_p)", dest_addr_p);
    mock_pop("sendto(): return (res)", &res);

    return (res);
}

void mock_push_ml_open(const char *path_p, int flags, int res)
{
    mock_push("ml_open(path_p)", path_p, strlen(path_p) + 1);
    mock_push("ml_open(flags)", &flags, sizeof(flags));
    mock_push("ml_open(): return (res)", &res, sizeof(res));
}

int __wrap_ml_open(const char *path_p, int flags)
{
    int res;

    mock_pop_assert("ml_open(path_p)", path_p);
    mock_pop_assert("ml_open(flags)", &flags);
    mock_pop("ml_open(): return (res)", &res);

    return (res);
}

void mock_push_ml_close(int fd, int res)
{
    mock_push("ml_close(fd)", &fd, sizeof(fd));
    mock_push("ml_close(): return (res)", &res, sizeof(res));
}

int __wrap_ml_close(int fd)
{
    int res;

    mock_pop_assert("ml_close(fd)", &fd);
    mock_pop("ml_close(): return (res)", &res);

    return (res);
}

void mock_push_ml_read(int fd, void *buf_p, size_t count, ssize_t res)
{
    mock_push("ml_read(fd)", &fd, sizeof(fd));
    mock_push("ml_read(buf_p)", buf_p, count);
    mock_push("ml_read(count)", &count, sizeof(count));
    mock_push("ml_read(): return (res)", &res, sizeof(res));
}

ssize_t __wrap_ml_read(int fd, void *buf_p, size_t count)
{
    ssize_t res;

    mock_pop_assert("ml_read(fd)", &fd);
    mock_pop("ml_read(buf_p)", buf_p);
    mock_pop_assert("ml_read(count)", &count);
    mock_pop("ml_read(): return (res)", &res);

    return (res);
}

void mock_push_ml_write(int fd, const void *buf_p, size_t count, ssize_t res)
{
    mock_push("ml_write(fd)", &fd, sizeof(fd));
    mock_push("ml_write(buf_p)", buf_p, count);
    mock_push("ml_write(count)", &count, sizeof(count));
    mock_push("ml_write(): return (res)", &res, sizeof(res));
}

ssize_t __wrap_ml_write(int fd, const void *buf_p, size_t count)
{
    ssize_t res;

    mock_pop_assert("ml_write(fd)", &fd);
    mock_pop_assert("ml_write(buf_p)", buf_p);
    mock_pop_assert("ml_write(count)", &count);
    mock_pop("ml_write(): return (res)", &res);

    return (res);
}

void mock_push_ml_finit_module(int fd,
                               const char *params_p,
                               int flags,
                               int res)
{
    mock_push("ml_finit_module(fd)", &fd, sizeof(fd));
    mock_push("ml_finit_module(params_p)", params_p, strlen(params_p) + 1);
    mock_push("ml_finit_module(flags)", &flags, sizeof(flags));
    mock_push("ml_finit_module(): return (res)", &res, sizeof(res));
}

int __wrap_ml_finit_module(int fd, const char *params_p, int flags)
{
    int res;

    mock_pop_assert("ml_finit_module(fd)", &fd);
    mock_pop_assert("ml_finit_module(params_p)", params_p);
    mock_pop_assert("ml_finit_module(flags)", &flags);
    mock_pop("ml_finit_module(): return (res)", &res);

    return (res);
}

void mock_push_setmntent(const char *filename_p,
                         const char *type_p,
                         FILE *res_p)
{
    mock_push("setmntent(filename_p)", filename_p, strlen(filename_p) + 1);
    mock_push("setmntent(type_p)", type_p, strlen(type_p) + 1);
    mock_push("setmntent(): return (res)", &res_p, sizeof(res_p));
}

FILE *__wrap_setmntent(const char *filename_p, const char *type_p)
{
    FILE *res_p;

    mock_pop_assert("setmntent(filename_p)", filename_p);
    mock_pop_assert("setmntent(type_p)", type_p);
    mock_pop("setmntent(): return (res)", &res_p);

    return (res_p);
}

void mock_push_getmntent(FILE *stream_p, struct mntent *res_p)
{
    mock_push("getmntent(stream_p)", &stream_p, sizeof(stream_p));
    mock_push("getmntent(): return (res)", &res_p, sizeof(res_p));
}

struct mntent *__wrap_getmntent(FILE *stream_p)
{
    struct mntent *res_p;

    mock_pop_assert("getmntent(stream_p)", &stream_p);
    mock_pop("getmntent(): return (res)", &res_p);

    return (res_p);
}

void mock_push_endmntent(FILE *stream_p, int res)
{
    mock_push("endmntent(stream_p)", &stream_p, sizeof(stream_p));
    mock_push("endmntent(): return (res)", &res, sizeof(res));
}

int __wrap_endmntent(FILE *stream_p)
{
    int res;

    mock_pop_assert("endmntent(stream_p)", &stream_p);
    mock_pop("endmntent(): return (res)", &res);

    return (res);
}

void mock_push_statvfs(const char *path_p, struct statvfs *buf_p, int res)
{
    mock_push("statvfs(path_p)", path_p, strlen(path_p) + 1);
    mock_push("statvfs(buf_p)", buf_p, sizeof(*buf_p));
    mock_push("statvfs(): return (res)", &res, sizeof(res));
}

int __wrap_statvfs(const char *path_p, struct statvfs *buf_p)
{
    int res;

    mock_pop_assert("statvfs(path_p)", path_p);
    mock_pop("statvfs(buf_p)", buf_p);
    mock_pop("statvfs(): return (res)", &res);

    return (res);
}

void mock_push_nftw(const char *dirpath_p,
                    int nopenfd,
                    int flags,
                    const char *paths[],
                    mode_t modes[],
                    int length,
                    int res)
{
    int i;

    mock_push("nftw(dirpath_p)", dirpath_p, strlen(dirpath_p) + 1);
    mock_push("nftw(nopenfd)", &nopenfd, sizeof(nopenfd));
    mock_push("nftw(flags)", &flags, sizeof(flags));
    mock_push("nftw(): return (length)", &length, sizeof(length));

    for (i = 0; i < length; i++) {
        mock_push("nftw(): return (path)", &paths[i], sizeof(paths[0]));
        mock_push("nftw(): return (mode)", &modes[i], sizeof(modes[0]));
    }

    mock_push("nftw(): return (res)", &res, sizeof(res));
}

int __wrap_nftw(const char *dirpath_p,
                int (*fn) (const char *fpath_p,
                           const struct stat *sb_p,
                           int typeflag,
                           struct FTW *ftwbuf_p),
                int nopenfd,
                int flags)
{
    int res;
    int i;
    int length;
    const char *path_p;
    struct stat stat;

    mock_pop_assert("nftw(dirpath_p)", dirpath_p);
    mock_pop_assert("nftw(nopenfd)", &nopenfd);
    mock_pop_assert("nftw(flags)", &flags);
    mock_pop("nftw(): return (length)", &length);

    for (i = 0; i < length; i++) {
        mock_pop("nftw(): return (path)", &path_p);
        mock_pop("nftw(): return (mode)", &stat.st_mode);

        ASSERT_EQ(fn(path_p, &stat, 0, NULL), 0);
    }

    mock_pop("nftw(): return (res)", &res);

    return (res);
}

void mock_push_ml_mknod(const char *path_p, mode_t mode, dev_t dev, int res)
{
    mock_push("ml_mknod(path_p)", path_p, strlen(path_p) + 1);
    mock_push("ml_mknod(mode)", &mode, sizeof(mode));
    mock_push("ml_mknod(dev)", &dev, sizeof(dev));
    mock_push("ml_mknod(): return (res)", &res, sizeof(res));
}

int __wrap_ml_mknod(const char *path_p, mode_t mode, dev_t dev)
{
    int res;

    mock_pop_assert("ml_mknod(path_p)", path_p);
    mock_pop_assert("ml_mknod(mode)", &mode);
    mock_pop_assert("ml_mknod(dev)", &dev);
    mock_pop("ml_mknod(): return (res)", &res);

    return (res);
}

void mock_push_timerfd_create(int clockid, int flags, int res)
{
    mock_push("timerfd_create(clockid)", &clockid, sizeof(clockid));
    mock_push("timerfd_create(flags)", &flags, sizeof(flags));
    mock_push("timerfd_create(): return (res)", &res, sizeof(res));
}

int __wrap_timerfd_create(int clockid, int flags)
{
    int res;

    mock_pop_assert("timerfd_create(clockid)", &clockid);
    mock_pop_assert("timerfd_create(flags)", &flags);
    mock_pop("timerfd_create(): return (res)", &res);

    return (res);
}

void mock_push_timerfd_settime(int fd,
                               int flags,
                               const struct itimerspec *new_value_p,
                               int res)
{
    mock_push("timerfd_settime(fd)", &fd, sizeof(fd));
    mock_push("timerfd_settime(flags)", &flags, sizeof(flags));
    mock_push("timerfd_settime(new_value_p)", new_value_p, sizeof(*new_value_p));
    mock_push("timerfd_settime(): return (res)", &res, sizeof(res));
}

int __wrap_timerfd_settime(int fd,
                           int flags,
                           const struct itimerspec *new_value_p,
                           struct itimerspec *old_value_p)
{
    (void)old_value_p;

    int res;

    mock_pop_assert("timerfd_settime(fd)", &fd);
    mock_pop_assert("timerfd_settime(flags)", &flags);
    mock_pop_assert("timerfd_settime(new_value_p)", new_value_p);
    mock_pop("timerfd_settime(): return (res)", &res);

    return (res);
}

void mock_push_poll(struct pollfd *fds_p, nfds_t nfds, int timeout, int res)
{
    nfds_t i;

    for (i = 0; i < nfds; i++) {
        mock_push("poll(fds_p->fd)", &fds_p[i].fd, sizeof(fds_p[i].fd));
        mock_push("poll(fds_p->events)", &fds_p[i].events, sizeof(fds_p[i].events));
        mock_push("poll(): return (fds_p->revents)",
                  &fds_p[i].revents,
                  sizeof(fds_p[i].revents));
    }

    mock_push("poll(nfds)", &nfds, sizeof(nfds));
    mock_push("poll(timeout)", &timeout, sizeof(timeout));
    mock_push("poll(): return (res)", &res, sizeof(res));
}

int __wrap_poll(struct pollfd *fds_p, nfds_t nfds, int timeout)
{
    int res;
    nfds_t i;

    for (i = 0; i < nfds; i++) {
        mock_pop_assert("poll(fds_p->fd)", &fds_p[i].fd);
        mock_pop_assert("poll(fds_p->events)", &fds_p[i].events);
        mock_pop("poll(): return (fds_p->revents)", &fds_p[i].revents);
    }

    mock_pop_assert("poll(nfds)", &nfds);
    mock_pop_assert("poll(timeout)", &timeout);
    mock_pop("poll(): return (res)", &res);

    return (res);
}
