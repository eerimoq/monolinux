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

#include <unistd.h>
#include <unicorn/unicorn.h>
#include "ml/ml.h"

ML_UID(m1);

static struct ml_bus_t bus;
static struct ml_queue_t queue_1;
static struct ml_queue_t queue_2;

TEST(broadcast_to_two_subscribers)
{
    struct ml_uid_t *uid_p;
    void *bmessage_p;
    void *message_p;

    ml_queue_init(&queue_1, 1);
    ml_queue_init(&queue_2, 1);
    ml_bus_init(&bus);
    ml_bus_subscribe(&bus, &queue_1, &m1);
    ml_bus_subscribe(&bus, &queue_2, &m1);

    /* Broadcast. */
    bmessage_p = ml_message_alloc(&m1, 0);
    ASSERT(bmessage_p != NULL);
    ml_bus_broadcast(&bus, bmessage_p);

    /* Get message for first subscriber. */
    uid_p = ml_queue_get(&queue_1, &message_p);
    ASSERT_EQ(uid_p, &m1);
    ASSERT_EQ(message_p, bmessage_p);
    ml_message_free(message_p);

    /* Get message for second subscriber. */
    uid_p = ml_queue_get(&queue_2, &message_p);
    ASSERT_EQ(uid_p, &m1);
    ASSERT_EQ(message_p, bmessage_p);
    ml_message_free(message_p);
}

int main()
{
    ml_init();

    return RUN_TESTS(
        broadcast_to_two_subscribers
    );
}
