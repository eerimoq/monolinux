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

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ml/ml.h"

#define WAIT_FOREVER                              -1

static void destroy(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

static void update_events(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

static void process_events(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

static void *reader_main(void *arg_p)
{
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    ML_INFO("Reader task starting...");
    ML_INFO("Reader task stopped.");

    return (NULL);
}

static void *keep_alive_main(void *arg_p)
{
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    ML_INFO("Keep alive task starting...");

    while (true) {
        sleep(self_p->ping_period_s);
        ML_DEBUG("Pinging the broker.");
    }

    ML_INFO("Keep alive task stopped.");

    return (NULL);
}

static void *client_main(void *arg_p)
{
    int res;
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    while (true) {
        res = poll(&self_p->fds[0], membersof(self_p->fds), WAIT_FOREVER);

        if (res <= 0) {
            ML_INFO("Poll returned %d.", res);
            break;
        }

        update_events(self_p);
        process_events(self_p);
    }

    return (NULL);
}

void ml_mqtt_client_init(struct ml_mqtt_client_t *self_p,
                         const char *host_p,
                         int port,
                         int log_mask,
                         struct ml_mqtt_client_subscription_t *subscriptions_p,
                         size_t number_of_subscriptions)
{
    ml_queue_init(&self_p->queue, 32);
    self_p->host_p = host_p;
    self_p->port = port;
    self_p->ping_period_s = 10;
    ml_log_object_init(&self_p->log_object,
                       "mqtt_client",
                       log_mask);
    self_p->subscriptions_p = subscriptions_p;
    self_p->number_of_subscriptions = number_of_subscriptions;
}

int ml_mqtt_client_start(struct ml_mqtt_client_t *self_p)
{
    int res;

    ML_INFO("Connecting to broker at '%s:%d'.", self_p->host_p, self_p->port);

    res = pthread_create(&self_p->client_pthread,
                         NULL,
                         client_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    res = pthread_create(&self_p->reader_pthread,
                         NULL,
                         reader_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    res = pthread_create(&self_p->keep_alive_pthread,
                         NULL,
                         keep_alive_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    return (0);

 err:
    destroy(self_p);

    return (res);
}

void ml_mqtt_client_stop(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

void ml_mqtt_client_join(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

bool ml_mqtt_client_message_is_topic(
    struct ml_mqtt_client_message_t *message_p,
    const char *topic_p)
{
    return (strcmp(message_p->topic_p, topic_p) == 0);
}

struct ml_uid_t *ml_mqtt_client_message_uid(void)
{
    return (NULL);
}

void ml_mqtt_client_publish(struct ml_mqtt_client_t *self_p,
                            struct ml_mqtt_client_message_t *message_p)
{
    (void)self_p;
    (void)message_p;
}
