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
 * This file is part of the monolinux project.
 */

#include <stdlib.h>
#include "ml.h"
#include "internal.h"

struct module_t {
    pthread_mutex_t mutex;
};

static struct module_t module;

void ml_message_init(void)
{
    pthread_mutex_init(&module.mutex, NULL);
}

void *ml_message_alloc(struct ml_uid_t *uid_p, size_t size)
{
    struct ml_message_header_t *header_p;

    header_p = xmalloc(sizeof(*header_p) + size);
    header_p->count = 1;
    header_p->uid_p = uid_p;

    return (message_from_header(header_p));
}

void ml_message_free(void *message_p)
{
    struct ml_message_header_t *header_p;
    int count;

    header_p = message_to_header(message_p);

    pthread_mutex_lock(&module.mutex);
    header_p->count--;
    count = header_p->count;
    pthread_mutex_unlock(&module.mutex);

    if (count == 0) {
        free(header_p);
    }
}

void ml_message_share(void *message_p, int count)
{
    pthread_mutex_lock(&module.mutex);
    message_to_header(message_p)->count += count;
    pthread_mutex_unlock(&module.mutex);
}
