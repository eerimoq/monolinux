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

#include <unicorn/unicorn.h>
#include "ml/ml.h"
#include "utils/mocks/mock.h"

static struct ml_queue_t test_spawn_queue;
static ML_UID(test_spawn_message_id);

static void test_spawn_entry(void *arg_p)
{
    ml_queue_put(&test_spawn_queue, arg_p);
}

TEST(spawn)
{
    void *message_p;

    ml_init();

    ml_queue_init(&test_spawn_queue, 10);
    message_p = ml_message_alloc(&test_spawn_message_id, 0);
    ml_spawn(test_spawn_entry, message_p);
    ASSERT_EQ(ml_queue_get(&test_spawn_queue, &message_p),
              &test_spawn_message_id);
    ml_message_free(message_p);
}

int main()
{
    return RUN_TESTS(
        spawn
    );
}
