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
    ml_log_object_init(&self_p->log_object,
                       "mqtt_client",
                       log_mask);
    self_p->subscriptions_p = subscriptions_p;
    self_p->number_of_subscriptions = number_of_subscriptions;
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
