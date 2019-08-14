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

#include <narwhal/narwhal.h>
#include "utils/utils.h"
#include "ml/ml.h"

TEST(format, basic_fixture)
{
    struct ml_log_object_t log_object;

    ml_log_object_init(&log_object, "foo", ML_LOG_UPTO(DEBUG));

    /* Emergency. */
    CAPTURE_OUTPUT(output1) {
        ml_log_object_print(&log_object, ML_LOG_EMERGENCY, "bar %d", 1);
    }

    ASSERT_SUBSTRING(output1, " EMERGENCY foo bar 1\n");

    /* Alert. */
    CAPTURE_OUTPUT(output2) {
        ml_log_object_print(&log_object, ML_LOG_ALERT, "bar %d", 2);
    }

    ASSERT_SUBSTRING(output2, " ALERT foo bar 2\n");

    /* Critical. */
    CAPTURE_OUTPUT(output3) {
        ml_log_object_print(&log_object, ML_LOG_CRITICAL, "bar %d", 3);
    }

    ASSERT_SUBSTRING(output3, " CRITICAL foo bar 3\n");

    /* Error. */
    CAPTURE_OUTPUT(output4) {
        ml_log_object_print(&log_object, ML_LOG_ERROR, "bar %d", 4);
    }

    ASSERT_SUBSTRING(output4, " ERROR foo bar 4\n");

    /* Warning. */
    CAPTURE_OUTPUT(output5) {
        ml_log_object_print(&log_object, ML_LOG_WARNING, "bar %d", 5);
    }

    ASSERT_SUBSTRING(output5, " WARNING foo bar 5\n");

    /* Notice. */
    CAPTURE_OUTPUT(output6) {
        ml_log_object_print(&log_object, ML_LOG_NOTICE, "bar %d", 6);
    }

    ASSERT_SUBSTRING(output6, " NOTICE foo bar 6\n");

    /* Info. */
    CAPTURE_OUTPUT(output7) {
        ml_log_object_print(&log_object, ML_LOG_INFO, "bar %d", 7);
    }

    ASSERT_SUBSTRING(output7, " INFO foo bar 7\n");

    /* Debug. */
    CAPTURE_OUTPUT(output8) {
        ml_log_object_print(&log_object, ML_LOG_DEBUG, "bar %d", 8);
    }

    ASSERT_SUBSTRING(output8, " DEBUG foo bar 8\n");
}

TEST(enable_disable, basic_fixture)
{
    struct ml_log_object_t log_object;

    /* Only debug, no info. */
    ml_log_object_init(&log_object, "foo", ML_LOG_MASK(DEBUG));
    ASSERT(ml_log_object_is_enabled_for(&log_object, ML_LOG_DEBUG));
    ASSERT(!ml_log_object_is_enabled_for(&log_object, ML_LOG_INFO));

    CAPTURE_OUTPUT(output1) {
        ml_log_object_print(&log_object, ML_LOG_DEBUG, "bar");
        ml_log_object_print(&log_object, ML_LOG_INFO, "bar");
    }

    ASSERT_SUBSTRING(output1, " DEBUG foo bar\n");
    ASSERT_NOT_SUBSTRING(output1, " INFO foo bar\n");

    /* Only info, no debug. */
    ml_log_object_set_mask(&log_object, ML_LOG_MASK(INFO));
    ASSERT(!ml_log_object_is_enabled_for(&log_object, ML_LOG_DEBUG));
    ASSERT(ml_log_object_is_enabled_for(&log_object, ML_LOG_INFO));

    CAPTURE_OUTPUT(output2) {
        ml_log_object_print(&log_object, ML_LOG_DEBUG, "bar");
        ml_log_object_print(&log_object, ML_LOG_INFO, "bar");
    }

    ASSERT_SUBSTRING(output2, " INFO foo bar\n");
    ASSERT_NOT_SUBSTRING(output2, " DEBUG foo bar\n");
}

int main()
{
    return RUN_TESTS(
        format,
        enable_disable
    );
}
