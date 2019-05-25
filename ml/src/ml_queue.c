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

#include <stdbool.h>
#include "ml/ml.h"
#include "internal.h"

static bool is_empty(struct ml_queue_t *self_p)
{
    return (self_p->rdpos == self_p->wrpos);
}

static bool is_full(struct ml_queue_t *self_p)
{
    return (((self_p->wrpos + 1) % self_p->length) == self_p->rdpos);
}

static void push_message(struct ml_queue_t *self_p,
                         struct ml_message_header_t *header_p)
{
    self_p->messages_p[self_p->wrpos] = header_p;
    self_p->wrpos++;
    self_p->wrpos %= self_p->length;
}

static struct ml_message_header_t *pop_message(struct ml_queue_t *self_p)
{
    struct ml_message_header_t *header_p;

    header_p = self_p->messages_p[self_p->rdpos];
    self_p->rdpos++;
    self_p->rdpos %= self_p->length;

    return (header_p);
}

void ml_queue_init(struct ml_queue_t *self_p, int length)
{
    self_p->rdpos = 0;
    self_p->wrpos = 0;
    self_p->length = (length + 1);
    self_p->messages_p = xmalloc(sizeof(void *) * (length + 1));
    pthread_mutex_init(&self_p->mutex, NULL);
    pthread_cond_init(&self_p->cond, NULL);
}

struct ml_uid_t *ml_queue_get(struct ml_queue_t *self_p, void **message_pp)
{
    struct ml_message_header_t *header_p;

    pthread_mutex_lock(&self_p->mutex);

    if (is_empty(self_p)) {
        pthread_cond_wait(&self_p->cond, &self_p->mutex);
    }

    header_p = pop_message(self_p);
    pthread_cond_signal(&self_p->cond);
    pthread_mutex_unlock(&self_p->mutex);

    *message_pp = message_from_header(header_p);

    return (header_p->uid_p);
}

void ml_queue_put(struct ml_queue_t *self_p, void *message_p)
{
    pthread_mutex_lock(&self_p->mutex);

    if (is_full(self_p)) {
        pthread_cond_wait(&self_p->cond, &self_p->mutex);
    }

    push_message(self_p, message_to_header(message_p));
    pthread_cond_signal(&self_p->cond);
    pthread_mutex_unlock(&self_p->mutex);
}
