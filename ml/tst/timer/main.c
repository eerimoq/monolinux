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
#include "utils/utils.h"

ML_UID(timeout);

TEST(single_shot, basic_fixture)
{
    struct ml_timer_t timer;
    struct ml_queue_t queue;
    /* struct ml_uid_t *uid_p; */
    /* void *message_p; */
    /* struct ml_timer_timeout_message_t *timeout_p; */

    ml_queue_init(&queue, 1);
    ml_timer_init(&timer,
                  0,
                  &timeout,
                  &queue,
                  0);
    ml_timer_start(&timer);
    /* uid_p = ml_queue_get(&queue, &message_p); */
    /* ASSERT_EQ(uid_p, &timeout_1); */
    /* timeout_p = (struct ml_timer_timeout_message_t *)message_p; */
    /* ASSERT_EQ(timeout_p->stopped, false); */
}

int main()
{
    return RUN_TESTS(
        single_shot
    );
}
