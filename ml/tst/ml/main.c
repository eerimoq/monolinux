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

int main()
{
    ml_init();

    return RUN_TESTS(
        strip,
        hexdump_short,
        hexdump_long
    );
}
