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
#include <sys/mount.h>
#include "ml.h"
#include "internal.h"

struct module_t {
    struct ml_bus_t bus;
};

static struct module_t module;

static inline bool char_in_string(char c, const char *str_p)
{
    while (*str_p != '\0') {
        if (c == *str_p) {
            return (true);
        }

        str_p++;
    }

    return (false);
}

void ml_init(void)
{
    ml_message_init();
    ml_bus_init(&module.bus);
}

const char *ml_uid_str(struct ml_uid_t *uid_p)
{
    return (uid_p->name_p);
}

void ml_broadcast(void *message_p)
{
    ml_bus_broadcast(&module.bus, message_p);
}

void ml_subscribe(struct ml_queue_t *queue_p, struct ml_uid_t *uid_p)
{
    ml_bus_subscribe(&module.bus, queue_p, uid_p);
}

char *strip(char *str_p, const char *strip_p)
{
    char *begin_p;
    size_t length;

    /* Strip whitespace characters by default. */
    if (strip_p == NULL) {
        strip_p = "\t\n\x0b\x0c\r ";
    }

    /* String leading characters. */
    while ((*str_p != '\0') && char_in_string(*str_p, strip_p)) {
        str_p++;
    }

    begin_p = str_p;

    /* Strip training characters. */
    length = strlen(str_p);
    str_p += (length - 1);

    while ((str_p >= begin_p) && char_in_string(*str_p, strip_p)) {
        *str_p = '\0';
        str_p--;
    }

    return (begin_p);
}

void *xmalloc(size_t size)
{
    void *buf_p;

    buf_p = malloc(size);

    if (buf_p == NULL) {
        exit(1);
    }

    return (buf_p);
}

void *xrealloc(void *buf_p, size_t size)
{
    buf_p = realloc(buf_p, size);

    if (buf_p == NULL) {
        exit(1);
    }

    return (buf_p);
}

int xmount(const char *source_p,
           const char *target_p,
           const char *type_p)
{
    int res;

    res = mount(source_p, target_p, type_p, 0, "");

    if (res != 0) {
        perror("error: mount: ");
        exit(1);
    }

    return (res);
}
