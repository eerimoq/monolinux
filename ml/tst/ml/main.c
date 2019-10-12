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

#include <fcntl.h>
#include <narwhal/narwhal.h>
#include "ml/ml.h"
#include "utils/mocks/mock_libc.h"
#include "utils/mocks/mock.h"
#include "utils/utils.h"

TEST(strip, basic_fixture)
{
    char string1[] = "1  ";
    char string2[] = " 1 ";
    char string3[] = "  1";
    char string4[] = "   ";
    char *begin_p;

    begin_p = ml_strip(string1, NULL);
    ASSERT_EQ(begin_p, "1");
    ASSERT_EQ(begin_p, &string1[0]);

    begin_p = ml_strip(string1, "1");
    ASSERT_EQ(begin_p, "");
    ASSERT_EQ(begin_p, &string1[0]);

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

TEST(lstrip, basic_fixture)
{
    char string1[] = "1 ";
    char string2[] = " 1";
    char string3[] = "  ";
    char *begin_p;

    begin_p = ml_lstrip(string1, NULL);
    ASSERT_EQ(begin_p, "1 ");
    ASSERT_EQ(begin_p, &string1[0]);

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

TEST(rstrip, basic_fixture)
{
    char string1[] = "1 ";
    char string2[] = " 1";
    char string3[] = "  ";

    ml_rstrip(string1, NULL);
    ASSERT_EQ(&string1[0], "1");

    ml_rstrip(string2, NULL);
    ASSERT_EQ(&string2[0], " 1");

    ml_rstrip(string2, "1");
    ASSERT_EQ(&string2[0], " ");

    ml_rstrip(string3, NULL);
    ASSERT_EQ(&string3[0], "");
}

TEST(hexdump_empty, basic_fixture)
{
    CAPTURE_OUTPUT(output) {
        ml_hexdump("", 0);
    }

    ASSERT_EQ(output, "");
}

TEST(hexdump_short, basic_fixture)
{
    CAPTURE_OUTPUT(output) {
        ml_hexdump("1", 1);
    }

    ASSERT_EQ(
        output,
        "00000000: 31                                              '1'\n");
}

TEST(hexdump_long, basic_fixture)
{
    CAPTURE_OUTPUT(output) {
        ml_hexdump(
            "110238\x00\x21h0112039jiajsFEWAFWE@#%!45eeeeeeeeeeeeeeeeeeeeeee"
            "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\x01\x0ageeeeerG012345678901234"
            "567890123456789012345678901234567890123456789012345678901234567"
            "890123456789012345678901234567890123456789012345678901234567890"
            "12345678901234567",
            257);
    }

    ASSERT_EQ(
        output,
        "00000000: 31 31 30 32 33 38 00 21 68 30 31 31 32 30 33 39 '110238.!h0112039'\n"
        "00000010: 6a 69 61 6a 73 46 45 57 41 46 57 45 40 23 25 21 'jiajsFEWAFWE@#%!'\n"
        "00000020: 34 35 65 65 65 65 65 65 65 65 65 65 65 65 65 65 '45eeeeeeeeeeeeee'\n"
        "00000030: 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 'eeeeeeeeeeeeeeee'\n"
        "00000040: 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 65 'eeeeeeeeeeeeeeee'\n"
        "00000050: 65 65 65 65 65 65 65 65 65 01 0a 67 65 65 65 65 'eeeeeeeee..geeee'\n"
        "00000060: 65 72 47 30 31 32 33 34 35 36 37 38 39 30 31 32 'erG0123456789012'\n"
        "00000070: 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 '3456789012345678'\n"
        "00000080: 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 '9012345678901234'\n"
        "00000090: 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 '5678901234567890'\n"
        "000000a0: 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 '1234567890123456'\n"
        "000000b0: 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 '7890123456789012'\n"
        "000000c0: 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 '3456789012345678'\n"
        "000000d0: 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 '9012345678901234'\n"
        "000000e0: 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 '5678901234567890'\n"
        "000000f0: 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 '1234567890123456'\n"
        "00000100: 37                                              '7'\n");
}

TEST(hexdump_file_0_0, basic_fixture)
{
    FILE *fin_p;

    fin_p = fopen("hexdump.in", "rb");
    ASSERT(fin_p != NULL);

    CAPTURE_OUTPUT(output) {
        ml_hexdump_file(fin_p, 0, 0);
    }

    fclose(fin_p);

    ASSERT_EQ(output, "");
}

TEST(hexdump_file_0_16, basic_fixture)
{
    FILE *fin_p;

    fin_p = fopen("hexdump.in", "rb");
    ASSERT(fin_p != NULL);

    CAPTURE_OUTPUT(output) {
        ml_hexdump_file(fin_p, 0, 16);
    }

    fclose(fin_p);

    ASSERT_EQ(
        output,
        "00000000: 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 '0123456789012345'\n");
}

TEST(hexdump_file_1_16, basic_fixture)
{
    FILE *fin_p;

    fin_p = fopen("hexdump.in", "rb");
    ASSERT(fin_p != NULL);

    CAPTURE_OUTPUT(output) {
        ml_hexdump_file(fin_p, 1, 16);
    }

    fclose(fin_p);

    ASSERT_EQ(
        output,
        "00000001: 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 '1234567890123456'\n");
}

TEST(hexdump_file_0_m1, basic_fixture)
{
    FILE *fin_p;

    fin_p = fopen("hexdump.in", "rb");
    ASSERT(fin_p != NULL);

    CAPTURE_OUTPUT(output) {
        ml_hexdump_file(fin_p, 0, -1);
    }

    fclose(fin_p);

    ASSERT_EQ(
        output,
        "00000000: 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 '0123456789012345'\n"
        "00000010: 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 '6789012345678901'\n"
        "00000020: 32 33 34 35 36 37 38 39                         '23456789'\n");
}

TEST(hexdump_file_1_m1, basic_fixture)
{
    FILE *fin_p;

    fin_p = fopen("hexdump.in", "rb");
    ASSERT(fin_p != NULL);

    CAPTURE_OUTPUT(output) {
        ml_hexdump_file(fin_p, 1, -1);
    }

    fclose(fin_p);

    ASSERT_EQ(
        output,
        "00000001: 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 '1234567890123456'\n"
        "00000011: 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 '7890123456789012'\n"
        "00000021: 33 34 35 36 37 38 39                            '3456789'\n");
}

TEST(print_file, basic_fixture)
{
    CAPTURE_OUTPUT(output) {
        ml_print_file("hexdump.in");
    }

    ASSERT_EQ(output, "0123456789012345678901234567890123456789");
}

TEST(print_uptime, basic_fixture)
{
    CAPTURE_OUTPUT(output) {
        ml_print_uptime();
    }

    ASSERT_SUBSTRING(output, "Uptime:");
    ASSERT_SUBSTRING(output, "\n");
}

static ML_UID(m1);

static struct ml_queue_t queue;

TEST(bus, basic_fixture)
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

TEST(ml_mount_ok, basic_fixture)
{
    mock_push_mount("a", "b", "c", 0, "", 1, 0);

    ASSERT_EQ(ml_mount("a", "b", "c"), 0);
}

TEST(insmod, basic_fixture)
{
    int fd;

    fd = 99;
    mock_push_ml_open("foo.ko", O_RDONLY, fd);
    mock_push_ml_finit_module(fd, "", 0, 0);
    mock_push_ml_close(fd, 0);

    ASSERT_EQ(ml_insert_module("foo.ko", ""), 0);
}

static struct ml_queue_t test_spawn_queue;
static ML_UID(test_spawn_message_id);

static void test_spawn_entry(void *arg_p)
{
    PRINT_FILE_LINE();
    ml_queue_put(&test_spawn_queue, arg_p);
}

TEST(spawn, basic_fixture)
{
    void *message_p;

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
        strip,
        lstrip,
        rstrip,
        hexdump_empty,
        hexdump_short,
        hexdump_long,
        hexdump_file_0_0,
        hexdump_file_0_16,
        hexdump_file_1_16,
        hexdump_file_0_m1,
        hexdump_file_1_m1,
        print_file,
        print_uptime,
        bus,
        ml_mount_ok,
        insmod,
        spawn
    );
}
