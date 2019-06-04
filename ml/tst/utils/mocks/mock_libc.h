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

#ifndef MOCK_LIBC_H
#define MOCK_LIBC_H

#include <mntent.h>
#include <sys/statvfs.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "ml/ml.h"

void mock_push_mount(const char *source_p,
                     const char *target_p,
                     const char *type_p,
                     unsigned long flags,
                     const void *data_p,
                     size_t data_size,
                     int res);

void mock_push_socket(int domain, int type, int protocol, int res);

void mock_push_bind(int fd,
                    const struct sockaddr *addr_p,
                    socklen_t addrlen,
                    int res);

void mock_push_setsockopt(int sockfd,
                          int level,
                          int optname,
                          const void *optval_p,
                          socklen_t optlen,
                          int res);

void mock_push_ioctl(int fd,
                     unsigned long request,
                     void *data_p,
                     size_t data_size,
                     int res);

void mock_push_ioctl_ifreq_ok(int fd,
                              unsigned long request,
                              struct ifreq *ifreq_p);

void mock_push_sendto(int fd,
                      const void *buf_p,
                      size_t len,
                      const struct sockaddr_in *dest_addr_p,
                      ssize_t res);

void mock_push_recvfrom(int fd,
                        void *buf_p,
                        size_t len,
                        struct sockaddr_in *dest_addr_p,
                        ssize_t res);

void mock_push_ml_open(const char *path_p, int flags, int res);

void mock_push_ml_close(int fd, int res);

void mock_push_ml_read(int fd, void *buf_p, size_t count, ssize_t res);

void mock_push_ml_write(int fd, const void *buf_p, size_t count, ssize_t res);

void mock_push_ml_finit_module(int fd,
                               const char *params_p,
                               int flags,
                               int res);

void mock_push_setmntent(const char *filename_p,
                         const char *type_p,
                         FILE *res_p);

void mock_push_getmntent(FILE *stream_p, struct mntent *res_p);

void mock_push_endmntent(FILE *stream_p, int res);

void mock_push_statvfs(const char *path_p, struct statvfs *buf_p, int res);

void mock_push_nftw(const char *dirpath_p,
                    int nopenfd,
                    int flags,
                    const char *paths[],
                    mode_t modes[],
                    int length,
                    int res);

void mock_push_ml_mknod(const char *path_p, mode_t mode, dev_t dev, int res);

void mock_push_timerfd_create(int clockid, int flags, int res);

void mock_push_timerfd_settime(int fd,
                               int flags,
                               const struct itimerspec *new_value_p,
                               int res);

void mock_push_poll(struct pollfd *fds_p, nfds_t nfds, int timeout, int res);

#endif
