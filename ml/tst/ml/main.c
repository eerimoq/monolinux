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

TEST(strip)
{
    char string1[] = "1  ";
    char string2[] = " 1 ";
    char string3[] = "  1";
    char string4[] = "   ";
    char *begin_p;

    begin_p = ml_strip(string1, NULL);
    ASSERT_EQ(begin_p, "1");
    ASSERT_EQ(begin_p, string1);

    begin_p = ml_strip(string1, "1");
    ASSERT_EQ(begin_p, "");
    ASSERT_EQ(begin_p, string1);

    begin_p = ml_strip(string2, NULL);
    ASSERT_EQ(begin_p, "1");
    ASSERT_EQ(begin_p, &string2[1]);

    begin_p = ml_strip(string3, NULL);
    ASSERT_EQ(begin_p, "1");
    ASSERT_EQ(begin_p, &string3[2]);

    begin_p = ml_strip(string4, NULL);
    ASSERT_EQ(begin_p, "");
    ASSERT_EQ(begin_p, &string4[3]);
}

TEST(lstrip)
{
    char string1[] = "1 ";
    char string2[] = " 1";
    char string3[] = "  ";
    char *begin_p;

    begin_p = ml_lstrip(string1, NULL);
    ASSERT_EQ(begin_p, "1 ");
    ASSERT_EQ(begin_p, string1);

    begin_p = ml_lstrip(string1, "1");
    ASSERT_EQ(begin_p, " ");
    ASSERT_EQ(begin_p, &string1[1]);

    begin_p = ml_lstrip(string2, NULL);
    ASSERT_EQ(begin_p, "1");
    ASSERT_EQ(begin_p, &string2[1]);

    begin_p = ml_lstrip(string3, NULL);
    ASSERT_EQ(begin_p, "");
    ASSERT_EQ(begin_p, &string3[2]);
}

TEST(rstrip)
{
    char string1[] = "1 ";
    char string2[] = " 1";
    char string3[] = "  ";

    ml_rstrip(string1, NULL);
    ASSERT_EQ(string1, "1");

    ml_rstrip(string2, NULL);
    ASSERT_EQ(string2, " 1");

    ml_rstrip(string2, "1");
    ASSERT_EQ(string2, " ");

    ml_rstrip(string3, NULL);
    ASSERT_EQ(string3, "");
}

TEST(hexdump_short)
{
    CAPTURE_OUTPUT(output) {
        ml_hexdump("1", 1);
    }

    ASSERT_EQ(
        output,
        "00000000: 31                                              '1'\n");
}

TEST(hexdump_long)
{
    CAPTURE_OUTPUT(output) {
        ml_hexdump(
            "110238\x00\x21h0112039jiajsFEWAFWE@#%!45eeeeeeeeeeeeeeeeeeeeeee"
            "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\x01\x0ageeeeerG",
            99);
    }

    ASSERT_EQ(
        output,
        "00000000: 31 31 30 32 33 38 00 21 68 30 31 31 32 30 33 39 '110238.!h0112039'\n"
        "00000010: 6a 69 61 6a 73 46 45 57 41 46 57 45 40 23 25 21 'jiajsFEWAFWE@#%!'\n"
        "00000020: 34 35 65 65 65 65 65 65 65 65 65 65 65 65 65 65 '45eeeeeeeeeeeeee'\n"
        "00000030: 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 'eeeeeeeeeeeeeeee'\n"
        "00000040: 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 'eeeeeeeeeeeeeeee'\n"
        "00000050: 65 65 65 65 65 65 65 65 65 01 0a 67 65 65 65 65 'eeeeeeeee..geeee'\n"
        "00000060: 65 72 47                                        'erG'\n");
}

static ML_UID(m1);

static struct ml_queue_t queue;

TEST(bus)
{
    struct ml_uid_t *uid_p;
    int *message_p;
    int *rmessage_p;

    ml_queue_init(&queue, 1);
    ml_subscribe(&queue, &m1);

    message_p = ml_message_alloc(&m1, sizeof(int));
    *message_p = 9;
    ml_broadcast(message_p);

    uid_p = ml_queue_get(&queue, (void **)&rmessage_p);
    ASSERT_EQ(ml_uid_str(uid_p), ml_uid_str(&m1));
    ASSERT_EQ(*rmessage_p, 9);
    ml_message_free(rmessage_p);
}

int __wrap_mount(const char *source_p,
                 const char *target_p,
                 const char *type_p,
                 unsigned long flags,
                 const void *data_p)
{
    ASSERT_EQ(source_p, "a");
    ASSERT_EQ(target_p, "b");
    ASSERT_EQ(type_p, "c");
    ASSERT_EQ(flags, 0);
    ASSERT_EQ(memcmp(data_p, "", 1), 0);

    return (0);
}

TEST(xmount_ok)
{
    xmount("a", "b", "c");
}

int main()
{
    ml_init();

    return RUN_TESTS(
        strip,
        lstrip,
        rstrip,
        hexdump_short,
        hexdump_long,
        bus,
        xmount_ok
    );
}
