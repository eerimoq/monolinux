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

#ifndef ML_H
#define ML_H

#include <pthread.h>

/**
 * Create a unique identifier.
 */
#define ML_UID(name)                            \
    struct ml_uid_t name = {                    \
        .name_p = #name                         \
    }

struct ml_uid_t {
    const char *name_p;
};

struct ml_message_header_t {
    struct ml_uid_t *uid_p;
    int count;
};

struct ml_queue_t {
    int rdpos;
    int wrpos;
    int length;
    struct ml_message_header_t **messages_p;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct ml_bus_elem_t {
    struct ml_uid_t *uid_p;
    int number_of_queues;
    struct ml_queue_t **queues_pp;
};

struct ml_bus_t {
    int number_of_elems;
    struct ml_bus_elem_t *elems_p;
};

/**
 * Initialize the monolinux module. This must be called before any
 * other function in this module.
 */
void ml_init(void);

/**
 * Get the name of given unique id.
 */
const char *ml_uid_str(struct ml_uid_t *uid_p);

/**
 * Subscribe to given message on the default bus.
 */
void ml_subscribe(struct ml_queue_t *queue_p, struct ml_uid_t *uid_p);

/**
 * Broadcast given message on the default bus.
 */
void ml_broadcast(void *message_p);

/**
 * Initialize the message submodule. Normally only called by
 * ml_init().
 */
void ml_message_init(void);

/**
 * Allocate a message with given id and size.
 */
void *ml_message_alloc(struct ml_uid_t *uid_p, size_t size);

/**
 * Free given message.
 */
void ml_message_free(void *message_p);

/**
 * Share given message count times. Count must not be negative.
 */
void ml_message_share(void *message_p, int count);

/**
 * Initialize given message queue. Only one thread may get messages
 * from a queue. Multiple threads may put messages on a queue.
 */
void ml_queue_init(struct ml_queue_t *self_p, int length);

/**
 * Get the oldest message from given message queue. It is forbidden to
 * modify the message as it may be shared between multiple
 * threads. Free the message once used.
 */
struct ml_uid_t *ml_queue_get(struct ml_queue_t *self_p, void **message_pp);

/**
 * Put given message into given queue.
 */
void ml_queue_put(struct ml_queue_t *self_p, void *message_p);

/**
 * Initialize given bus.
 */
void ml_bus_init(struct ml_bus_t *self_p);

/**
 * Subscribe to given message. This function must only be called
 * before any message is broadcasted on given bus.
 */
void ml_bus_subscribe(struct ml_bus_t *self_p,
                      struct ml_queue_t *queue_p,
                      struct ml_uid_t *uid_p);

/**
 * Broadcast given message on given bus. All subscribers will receive
 * the message.
 */
void ml_bus_broadcast(struct ml_bus_t *self_p, void *message_p);

#endif
