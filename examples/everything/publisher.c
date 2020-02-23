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
 * This file is part of the Async project.
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "async.h"
#include "hf.h"
#include "publisher.h"

static void on_subscribe_complete(struct publisher_t *self_p,
                                  uint16_t transaction_id)
{
    (void)self_p;

    printf("%s: Subscribe with transaction id %d completed.\n",
           &self_p->client.client_id[0],
           transaction_id);
}

static void start_publish_timer(struct publisher_t *self_p)
{
    printf("%s: Starting the publish timer with timeout %u ms.\n",
           &self_p->client.client_id[0],
           async_timer_get_repeat(&self_p->publish_timer));
    async_timer_start(&self_p->publish_timer);
}

static void stop_publish_timer(struct publisher_t *self_p)
{
    printf("%s: Stopping the publish timer.\n", &self_p->client.client_id[0]);
    async_timer_stop(&self_p->publish_timer);
}

static void on_connected(struct publisher_t *self_p)
{
    printf("%s: Connected.\n", &self_p->client.client_id[0]);
    async_mqtt_client_subscribe(&self_p->client, "async/start");
    async_mqtt_client_subscribe(&self_p->client, "async/stop");
    async_mqtt_client_subscribe(&self_p->client, "async/reconnect");
    start_publish_timer(self_p);
}

static void on_disconnected(struct publisher_t *self_p)
{
    printf("%s: Disconnected.\n", &self_p->client.client_id[0]);
    stop_publish_timer(self_p);
}

static int parse_timeout(const uint8_t *buf_p,
                         size_t size,
                         int minimum,
                         int default_value)
{
    char buf[16];

    hf_buffer_to_string(&buf[0], sizeof(buf), buf_p, size);

    return (hf_string_to_long(&buf[0], minimum, INT_MAX, default_value, 10));
}

static void on_publish_start(struct publisher_t *self_p,
                             const uint8_t *buf_p,
                             size_t size)
{
    unsigned int timeout;

    timeout = parse_timeout(buf_p, size, 100, 1000);
    async_timer_set_initial(&self_p->publish_timer, timeout);
    async_timer_set_repeat(&self_p->publish_timer, timeout);
    start_publish_timer(self_p);
}

static void on_publish_stop(struct publisher_t *self_p)
{
    stop_publish_timer(self_p);
}

static void on_reconnect_timeout(struct publisher_t *self_p)
{
    printf("%s: Starting the MQTT client.\n", &self_p->client.client_id[0]);
    async_mqtt_client_start(&self_p->client);
}

static void on_publish_reconnect(struct publisher_t *self_p,
                                 const uint8_t *buf_p,
                                 size_t size)
{
    int timeout_ms;

    timeout_ms = parse_timeout(buf_p, size, 3000, 3000);
    printf("%s: Stopping the MQTT client for %d ms.\n",
           &self_p->client.client_id[0],
           timeout_ms);
    async_mqtt_client_stop(&self_p->client);
    stop_publish_timer(self_p);
    async_timer_set_initial(&self_p->reconnect_timer, timeout_ms);
    async_timer_start(&self_p->reconnect_timer);
}

static void on_publish(struct publisher_t *self_p,
                       const char *topic_p,
                       const uint8_t *buf_p,
                       size_t size)
{
    if (strcmp(topic_p, "async/start") == 0) {
        on_publish_start(self_p, buf_p, size);
    } else if (strcmp(topic_p, "async/stop") == 0) {
        on_publish_stop(self_p);
    } else if (strcmp(topic_p, "async/reconnect") == 0) {
        on_publish_reconnect(self_p, buf_p, size);
    } else {
        printf("%s: Unexpected topic '%s'.\n",
               &self_p->client.client_id[0],
               topic_p);
    }
}

static void on_publish_timeout(struct publisher_t *self_p)
{
    char buf[32];
    size_t size;
    static int counter = 0;

    size = sprintf(&buf[0], "count: %d", counter++);
    printf("%s: Publishing '%s' on 'async/hello'.\n",
           &self_p->client.client_id[0],
           &buf[0]);
    async_mqtt_client_publish(&self_p->client, "async/hello", &buf[0], size);
}

void publisher_init(struct publisher_t *self_p,
                    const char *client_id_p,
                    int port,
                    struct async_ssl_context_t *ssl_context_p,
                    struct async_t *async_p)
{
    static uint8_t will_message[] = { ':', '(' };

    async_mqtt_client_init(&self_p->client,
                           "10.0.2.2",
                           port,
                           ssl_context_p,
                           (async_func_t)on_connected,
                           (async_func_t)on_disconnected,
                           (async_mqtt_client_on_publish_t)on_publish,
                           self_p,
                           async_p);
    async_mqtt_client_set_client_id(&self_p->client, client_id_p);
    async_mqtt_client_set_will(&self_p->client,
                               "async/will",
                               &will_message[0],
                               sizeof(will_message));
    async_mqtt_client_set_on_subscribe_complete(
        &self_p->client,
        (async_mqtt_client_on_subscribe_complete_t)on_subscribe_complete);
    async_timer_init(&self_p->publish_timer,
                     (async_timer_timeout_t)on_publish_timeout,
                     self_p,
                     1000,
                     1000,
                     async_p);
    async_timer_init(&self_p->reconnect_timer,
                     (async_timer_timeout_t)on_reconnect_timeout,
                     self_p,
                     1000,
                     0,
                     async_p);
    printf("%s: Starting the MQTT client, connecting to '%s:%d'.\n",
           client_id_p,
           "10.0.2.2",
           port);
    async_mqtt_client_start(&self_p->client);
}
