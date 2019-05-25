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

/*
 * +---+         +-----+   +-----+
 * | a |         | b_1 |   | b_2 |
 * +---+         +-----+   +-----+
 *   |              |         |
 *   |-- ping --+-->|         |
 *   |          +------------>|
 *   |<---- pong ---|         |
 *   |<--------- pong --------|
 *   |              |         |
 */

#include <stdio.h>
#include "ml/ml.h"

struct a_t {
    struct ml_queue_t queue;
    pthread_t pthread;
};

struct b_t {
    struct ml_queue_t queue;
    pthread_t pthread;
};

/* Message ids. */
static ML_UID(ping);
static ML_UID(pong);

static struct a_t a;
static struct b_t b_1;
static struct b_t b_2;

static void *a_main(struct a_t *self_p)
{
    struct ml_uid_t *uid_p;
    void *message_p;
    int i;

    /* Broadcast ping (to b_1 and b_2). */
    printf("Broadcasting ping!\n");
    message_p = ml_message_alloc(&ping, 0);
    ml_broadcast(message_p);

    /* Wait for pong from b_1 and b_2. */
    for (i = 0; i < 2; i++) {
        uid_p = ml_queue_get(&self_p->queue, &message_p);

        if (uid_p == &pong) {
            printf("Got pong!\n");
        }

        ml_message_free(message_p);
    }

    return (NULL);
}

static void *b_main(struct b_t *self_p)
{
    struct ml_uid_t *uid_p;
    void *message_p;

    /* Wait for ping (from a). */
    uid_p = ml_queue_get(&self_p->queue, &message_p);
    ml_message_free(message_p);

    if (uid_p == &ping) {
        printf("Got ping!\n");

        /* Broadcast pong (to a). */
        printf("Broadcasting pong!\n");
        message_p = ml_message_alloc(&pong, 0);
        ml_broadcast(message_p);
    }

    return (NULL);
}

int main()
{
    ml_init();

    ml_queue_init(&a.queue, 32);
    ml_subscribe(&a.queue, &pong);
    ml_queue_init(&b_1.queue, 32);
    ml_subscribe(&b_1.queue, &ping);
    ml_queue_init(&b_2.queue, 32);
    ml_subscribe(&b_2.queue, &ping);

    pthread_create(&a.pthread,
                   NULL,
                   (void *(*)(void *))a_main,
                   &a);
    pthread_create(&b_1.pthread,
                   NULL,
                   (void *(*)(void *))b_main,
                   &b_1);
    pthread_create(&b_2.pthread,
                   NULL,
                   (void *(*)(void *))b_main,
                   &b_2);

    pthread_join(a.pthread, NULL);
    pthread_join(b_1.pthread, NULL);
    pthread_join(b_2.pthread, NULL);

    return (0);
}
