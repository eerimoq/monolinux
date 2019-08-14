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
#include "utils/utils.h"
#include "ml/ml.h"

ML_UID(m1);

static int on_free_count;

static void on_free_cb(void *message_p)
{
    int *value_p;

    value_p = (int *)message_p;

    if (*value_p == 5) {
        on_free_count++;
    }
}

TEST(on_free, basic_fixture)
{
    void *message_p;
    int *value_p;

    on_free_count = 0;
    message_p = ml_message_alloc(&m1, sizeof(*message_p));
    ASSERT(message_p != NULL);
    value_p = (int *)message_p;
    *value_p = 5;
    ml_message_set_on_free(message_p, on_free_cb);
    ml_message_free(message_p);
    ASSERT(on_free_count == 1);
}

int main()
{
    return RUN_TESTS(
        on_free
    );
}
