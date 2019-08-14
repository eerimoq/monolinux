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
#include "ml/ml.h"

static struct ml_mqtt_client_t client;
static struct ml_queue_t messages;

static void init(void)
{
    static struct ml_mqtt_client_subscription_t subscriptions[] = {
        {
            .topic_p = "test/echo"
        }
    };

    ml_init();
    ml_queue_init(&messages, 64);
    ml_subscribe(&messages, ml_mqtt_client_message_uid());
    ml_mqtt_client_init(&client,
                        "localhost",
                        1883,
                        ML_LOG_ALL,
                        &subscriptions[0],
                        membersof(subscriptions));
}

static void start(void)
{
    ml_mqtt_client_start(&client);
}

static void handle_mqtt_message(struct ml_mqtt_client_message_t *message_p)
{
    printf("Topic:   %s\n", message_p->topic_p);
    printf("Message:\n");
    ml_hexdump(message_p->message.buf_p, message_p->message.size);

    if (ml_mqtt_client_message_is_topic(message_p, "test/echo")) {
        ml_mqtt_client_publish(&client, message_p);
    }
}

static void loop(void)
{
    struct ml_uid_t *uid_p;
    void *message_p;

    while (true) {
        uid_p = ml_queue_get(&messages, &message_p);

        if (uid_p == ml_mqtt_client_message_uid()) {
            handle_mqtt_message(message_p);
        } else {
            ML_WARNING("");
        }
    }
}

int main()
{
    init();
    start();
    loop();

    return (1);
}
