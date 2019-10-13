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

#include <stdbool.h>
#include <stdio.h>
#include "ml/ml.h"

static ML_UID(timeout_1);
static ML_UID(timeout_2);
static ML_UID(timeout_3);

int main()
{
    struct ml_timer_t timer_1;
    struct ml_timer_t timer_2;
    struct ml_timer_t timer_3;
    struct ml_queue_t queue;
    struct ml_uid_t *uid_p;
    struct ml_timer_timeout_message_t *message_p;

    ml_init();
    ml_queue_init(&queue, 16);
    ml_timer_init(&timer_1, 1000, &timeout_1, &queue, ML_TIMER_PERIODIC);
    ml_timer_init(&timer_2, 3000, &timeout_2, &queue, ML_TIMER_PERIODIC);
    ml_timer_init(&timer_3, 5000, &timeout_3, &queue, 0);

    ml_log_print(ML_LOG_INFO, "Starting timer 1.");
    ml_timer_start(&timer_1);

    ml_log_print(ML_LOG_INFO, "Starting timer 2.");
    ml_timer_start(&timer_2);

    ml_log_print(ML_LOG_INFO, "Starting timer 3.");
    ml_timer_start(&timer_3);

    while (true) {
        uid_p = ml_queue_get(&queue, (void **)&message_p);

        if (uid_p == &timeout_1) {
            ml_log_print(ML_LOG_INFO, "Timer 1 expired.");
        } else if (uid_p == &timeout_2) {
            ml_log_print(ML_LOG_INFO, "Timer 2 expired.");
        } else if (uid_p == &timeout_3) {
            ml_log_print(ML_LOG_INFO, "Timer 3 expired.");
        }

        ml_message_free(message_p);
    }

    return (0);
}
