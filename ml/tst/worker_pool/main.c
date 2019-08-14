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
#include <narwhal/narwhal.h>
#include "ml/ml.h"
#include "utils/mocks/mock.h"
#include "utils/utils.h"

static struct ml_queue_t queue;
static ML_UID(mid);

static void test_full_entry(void *arg_p)
{
    usleep(500);
    ml_queue_put(&queue, arg_p);
}

TEST(full, basic_fixture)
{
    void *message_p;
    struct ml_worker_pool_t worker_pool;
    int i;

    ml_queue_init(&queue, 100);
    ml_worker_pool_init(&worker_pool, 4, 10);

    for (i = 0; i < 100; i++) {
        message_p = ml_message_alloc(&mid, 0);
        ml_worker_pool_spawn(&worker_pool, test_full_entry, message_p);
    }

    for (i = 0; i < 100; i++) {
        ASSERT_EQ(ml_queue_get(&queue, &message_p), &mid);
        ml_message_free(message_p);
    }
}

int main()
{
    return RUN_TESTS(
        full
    );
}
