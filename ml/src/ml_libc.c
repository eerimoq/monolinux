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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "ml/ml.h"

int ml_open(const char *path_p, int flags)
{
    return (open(path_p, flags));
}

int ml_close(int fd)
{
    return (close(fd));
}

ssize_t ml_read(int fd, void *buf_p, size_t count)
{
    return (read(fd, buf_p, count));
}

ssize_t ml_write(int fd, const void *buf_p, size_t count)
{
    return (write(fd, buf_p, count));
}

int ml_finit_module(int fd, const char *params_p, int flags)
{
    return (syscall(SYS_finit_module, fd, params_p, flags));
}

int ml_mknod(const char *path_p, mode_t mode, dev_t dev)
{
    return (mknod(path_p, mode, dev));
}

#if defined(__GNU_LIBRARY__) && (__GLIBC__ <= 2) && (__GLIBC_MINOR__ <= 26)

int memfd_create(const char *name, unsigned flags)
{
    return syscall(SYS_memfd_create, name, flags);
}

#endif
