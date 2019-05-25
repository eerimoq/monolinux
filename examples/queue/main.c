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
 * +--------+           +--------+
 * | client |           | server |
 * +--------+           +--------+
 *      |                    |
 *      |------- ping ------>|
 *      |<------ pong -------|
 *      |<------ bad --------|
 *      |                    |
 *      |------- ping ------>|
 *      |<------ pong -------|
 *      |<------ bad --------|
 *      |                    |
 *      .                    .
 *      .                    .
 *      .                    .
 */

#include <stdbool.h>
#include <stdio.h>
#include "ml/ml.h"

struct server_t {
    struct ml_queue_t queue;
    pthread_t pthread;
};

struct client_t {
    struct ml_queue_t queue;
    pthread_t pthread;
};

/* Message ids. */
static ML_UID(ping);
static ML_UID(pong);
static ML_UID(bad);

static struct server_t server;
static struct client_t client;

static void *server_main(struct server_t *self_p)
{
    struct ml_uid_t *uid_p;
    int *ping_p;
    int *pong_p;
    void *bad_p;

    while (true) {
        /* Wait for a ping message. */
        uid_p = ml_queue_get(&self_p->queue, (void **)&ping_p);

        if (uid_p == &ping) {
            printf("ping: count: %d\n", *ping_p);

            /* Put a pong message on the queue. */
            pong_p = ml_message_alloc(&pong, sizeof(*pong_p));
            *pong_p = (*ping_p + 1);
            ml_queue_put(&client.queue, pong_p);

            /* Put a bad empty message on the queue. */
            bad_p = ml_message_alloc(&bad, 0);
            ml_queue_put(&client.queue, bad_p);
        } else {
            printf("Server got unexpected message '%s'\n", ml_uid_str(uid_p));
        }

        ml_message_free(ping_p);
    }

    return (NULL);
}

static void client_send_ping(int count)
{
    int *ping_p;

    ping_p = ml_message_alloc(&ping, sizeof(int));
    *ping_p = count;
    ml_queue_put(&server.queue, ping_p);
}

static void *client_main(struct client_t *self_p)
{
    struct ml_uid_t *uid_p;
    void *message_p;
    int *pong_p;

    /* Send a ping with count 1 to start the infinate ping-pong
       message passing between the client and server. */
    client_send_ping(1);

    while (true) {
        /* Wait for a message. */
        uid_p = ml_queue_get(&self_p->queue, &message_p);

        if (uid_p == &pong) {
            pong_p = message_p;
            printf("pong: count: %d\n", *pong_p);
            client_send_ping(*pong_p + 10);
        } else {
            printf("Client got unexpected message '%s'\n", ml_uid_str(uid_p));
        }

        ml_message_free(message_p);
    }

    return (NULL);
}

int main()
{
    ml_init();

    ml_queue_init(&client.queue, 32);
    ml_queue_init(&server.queue, 32);

    pthread_create(&client.pthread,
                   NULL,
                   (void *(*)(void *))client_main,
                   &client);
    pthread_create(&server.pthread,
                   NULL,
                   (void *(*)(void *))server_main,
                   &server);

    pthread_join(client.pthread, NULL);
    pthread_join(server.pthread, NULL);

    return (0);
}
