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
#include "narwhal.h"
#include "utils/utils.h"
#include "ml/ml.h"

ML_UID(m1);
ML_UID(m2);

static struct ml_queue_t queue;

TEST(multiple_put_get, basic_fixture)
{
    void *message_1_p;
    void *message_2_p;
    struct ml_uid_t *uid_p;
    void *message_p;

    /* Maximum two messages. */
    ml_queue_init(&queue, 2);

    /* Put first message. */
    message_1_p = ml_message_alloc(&m1, 0);
    ASSERT(message_1_p != NULL);
    ml_queue_put(&queue, message_1_p);

    /* Put second message. */
    message_2_p = ml_message_alloc(&m2, 0);
    ASSERT(message_2_p != NULL);
    ml_queue_put(&queue, message_2_p);

    /* Get first message. */
    uid_p = ml_queue_get(&queue, &message_p);
    ASSERT_EQ(uid_p, &m1);
    ASSERT_EQ(message_p, message_1_p);
    ml_message_free(message_p);

    /* Get second message. */
    uid_p = ml_queue_get(&queue, &message_p);
    ASSERT_EQ(uid_p, &m2);
    ASSERT_EQ(message_p, message_2_p);
    ml_message_free(message_p);

    /* Put a third message. */
    message_1_p = ml_message_alloc(&m1, 0);
    ASSERT(message_1_p != NULL);
    ml_queue_put(&queue, message_1_p);

    /* Get the third message. */
    uid_p = ml_queue_get(&queue, &message_p);
    ASSERT_EQ(uid_p, &m1);
    ASSERT_EQ(message_p, message_1_p);
    ml_message_free(message_p);
}

static struct ml_queue_t queue_1;
static struct ml_queue_t queue_2;

static void *queue_empty_and_full_main(void *arg_p)
{
    (void)arg_p;

    void *message_p;
    struct ml_uid_t *uid_p;
    int i;

    /* Wait 50 ms for the test to fill the queue. */
    usleep(50000);

    for (i = 0; i < 5; i++) {
        uid_p = ml_queue_get(&queue_1, &message_p);
        ASSERT_EQ(uid_p, &m1);
        ml_message_free(message_p);
    }

    /* Wait 50 ms for the test to get a message from an empty
       queue. */
    usleep(50000);

    for (i = 0; i < 5; i++) {
        message_p = ml_message_alloc(&m2, 0);
        ASSERT(message_p != NULL);
        ml_queue_put(&queue_2, message_p);
    }

    return (NULL);
}

TEST(queue_empty_and_full, basic_fixture)
{
    struct ml_uid_t *uid_p;
    void *message_p;
    pthread_t pthread;
    int i;

    /* Maximum one message. */
    ml_queue_init(&queue_1, 1);
    ml_queue_init(&queue_2, 1);

    pthread_create(&pthread,
                   NULL,
                   queue_empty_and_full_main,
                   NULL);

    for (i = 0; i < 5; i++) {
        message_p = ml_message_alloc(&m1, 0);
        ASSERT(message_p != NULL);
        ml_queue_put(&queue_1, message_p);
    }

    for (i = 0; i < 5; i++) {
        uid_p = ml_queue_get(&queue_2, &message_p);
        ASSERT_EQ(uid_p, &m2);
        ml_message_free(message_p);
    }

    pthread_join(pthread, NULL);
}

int main()
{
    return RUN_TESTS(
        multiple_put_get,
        queue_empty_and_full
    );
}
