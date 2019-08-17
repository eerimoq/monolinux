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

#include <stdint.h>
#include "ml/ml.h"
#include "internal.h"

struct module_t {
    pthread_t ticker_pthread;
};

static struct module_t module;

static void expired_list_insert(struct ml_timer_t *self_p,
                                struct ml_timer_timeout_message_t *message_p)
{
    pthread_mutex_lock(&self_p->expired.mutex);
    message_p->next_p = self_p->expired.list.next_p;
    message_p->prev_p = &self_p->expired.list;
    self_p->expired.list.next_p = message_p;
    pthread_mutex_unlock(&self_p->expired.mutex);
}

static void expired_list_remove(struct ml_timer_t *self_p,
                                struct ml_timer_timeout_message_t *message_p)
{
    pthread_mutex_lock(&self_p->expired.mutex);
    message_p->prev_p->next_p = message_p->next_p;
    pthread_mutex_unlock(&self_p->expired.mutex);
}

static void on_free(struct ml_timer_timeout_message_t *message_p)
{
    expired_list_remove(message_p->timer_p, message_p);
}

static void tick(struct ml_timer_handler_t *self_p)
{
    struct ml_timer_t *timer_p;
    struct ml_timer_timeout_message_t *message_p;
    
    message_p = ml_message_alloc(timer_p->timeout_p, sizeof(*message_p));
    ml_message_set_on_free(message_p, (ml_message_on_free_t)on_free);
    message_p->stopped = false;
    expired_list_insert(timer_p, message_p);
    ml_queue_put(timer_p->queue_p, message_p);
}

static void *ticker_main(void *arg_p)
{
    int fd;
    struct itimerspec timeout;

    fd = timerfd_create(CLOCK_REALTIME, 0);

    if (fd == -1) {
        return (NULL);
    }

    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 10000000;
    timeout.it_interval.tv_sec= 0;
    timeout.it_interval.tv_nsec = 10000000;
    timerfd_settime(fd, 0, &timeout, NULL);

    while (true) {
        read(fd);
        tick(self_p);
    }

    return (NULL);
}

void ml_timer_init(struct ml_timer_t *self_p,
                   int timeout_ms,
                   struct ml_uid_t *timeout_p,
                   struct ml_queue_t *queue_p,
                   int flags)
{
    self_p->timeout_ms = timeout_ms;
    self_p->timeout_p = timeout_p;
    self_p->queue_p = queue_p;
    self_p->flags = flags;
}

void ml_timer_start(struct ml_timer_t *self_p)
{
    timer_insert(self_p->handler_p, self_p);
}

void ml_timer_stop(struct ml_timer_t *self_p)
{
    struct ml_timer_timeout_message_t *message_p;

    message_p = &self_p->expired.list;
    pthread_mutex_lock(&self_p->expired.mutex);

    while (message_p != NULL) {
        message_p->stopped = true;
        message_p = message_p->next_p;
    }

    pthread_mutex_unlock(&self_p->expired.mutex);

    timer_remove(self_p->handler_p, self_p);
}

void ml_timer_handler_timer_init(struct ml_timer_t *self_p,
                                 struct ml_timer_handler_timer_t *handler_p,
                                 int timeout_ms,
                                 struct ml_uid_t *timeout_p,
                                 struct ml_queue_t *queue_p,
                                 int flags)
{
    self_p->handler_p = handler_p;
    self_p->timeout_ms = timeout_ms;
    self_p->timeout_p = timeout_p;
    self_p->queue_p = queue_p;
    self_p->flags = flags;
}
