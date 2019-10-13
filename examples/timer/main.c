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

int main()
{
    struct ml_timer_t timer_1;
    struct ml_timer_t timer_2;
    struct ml_queue_t queue;
    struct ml_uid_t *uid_p;
    struct ml_timer_timeout_message_t *message_p;

    ml_init();
    ml_queue_init(&queue, 16);
    ml_timer_init(&timer_1, 1000, &timeout_1, &queue, ML_TIMER_PERIODIC);
    ml_timer_init(&timer_2, 3000, &timeout_2, &queue, ML_TIMER_PERIODIC);

    printf("Starting timer 1.\n");
    ml_timer_start(&timer_1);

    printf("Starting timer 2.\n");
    ml_timer_start(&timer_2);

    while (true) {
        uid_p = ml_queue_get(&queue, (void **)&message_p);

        if (uid_p == &timeout_1) {
            printf("Timer 1 expired.\n");
        } else if (uid_p == &timeout_2) {
            printf("Timer 2 expired.\n");
        }

        ml_message_free(message_p);
    }

    return (0);
}
