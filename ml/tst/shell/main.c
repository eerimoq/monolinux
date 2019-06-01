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
#include <unicorn/unicorn.h>
#include "utils/mocks/mock_libc.h"
#include "utils/mocks/mock.h"
#include "utils/utils.h"
#include "ml/ml.h"

#define ESC "\x1b"

static int command_hello(int argc, const char *argv[])
{
    const char *name_p;

    if (argc == 2) {
        name_p = argv[1];
    } else {
        name_p = "stranger";
    }

    printf("Hello %s!\n", name_p);

    return (0);
}

TEST(various_commands)
{
    int fd;

    ml_shell_init();
    ml_shell_register_command("hello", "My command.", command_hello);

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "help\n");
        input(fd, "history\n");
        input(fd, "hello\n");
        input(fd, "hello Foo\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "help\n"
              "Cursor movement\n"
              "\n"
              "         LEFT   Go left one character.\n"
              "        RIGHT   Go right one character.\n"
              "  HOME/Ctrl+A   Go to the beginning of the line.\n"
              "   END/Ctrl+E   Go to the end of the line.\n"
              "\n"
              "Edit\n"
              "\n"
              "        Alt+D   Delete the word at the cursor.\n"
              "       Ctrl+D   Delete the chracter at the cursor.\n"
              "       Ctrl+K   Cut the line from cursor to end.\n"
              "       Ctrl+T   Swap the last two characters before the "
              "cursor (typo).\n"
              "          TAB   Tab completion for file/directory names.\n"
              "    BACKSPACE   Delete the character before the cursor.\n"
              "\n"
              "History\n"
              "\n"
              "           UP   Previous command.\n"
              "         DOWN   Next command.\n"
              "       Ctrl+R   Recall the last command including the specified "
              "character(s)\n"
              "                searches the command history as you type.\n"
              "       Ctrl+G   Escape from history searching mode.\n"
              "\n"
              "Commands\n"
              "\n"
              "          cat   Print a file.\n"
              "           df   Disk space usage.\n"
              "         exit   Shell exit.\n"
              "         find   Find files and folders.\n"
              "        hello   My command.\n"
              "         help   Print this help.\n"
              "      hexdump   Hexdump a file.\n"
              "      history   List command history.\n"
              "       insmod   Insert a kernel module.\n"
              "           ls   List directory contents.\n"
              "        mknod   Create a node.\n"
              "        mount   Mount a filesystem.\n"
              "       reboot   Reboot.\n"
              "      suicide   Process suicide.\n"
              "OK\n"
              "$ history\n"
              "1: help\n"
              "2: history\n"
              "OK\n"
              "$ hello\n"
              "Hello stranger!\n"
              "OK\n"
              "$ hello Foo\n"
              "Hello Foo!\n"
              "OK\n"
              "$ exit\n");
}

TEST(command_ls)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "ls\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_SUBSTRING(output, "OK\n$ ");
}

TEST(command_cat)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "cat\n");
        input(fd, "cat hexdump.in\n");
        input(fd, "cat foobar\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(
        output,
        "cat\n"
        "No file given\n"
        "ERROR(-1)\n"
        "$ cat hexdump.in\n"
        "0123456789012345678901234567890123456789"
        "OK\n"
        "$ cat foobar\n"
        "ERROR(-1)\n"
        "$ exit\n");
}

TEST(command_hexdump)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "hexdump\n");
        input(fd, "hexdump hexdump.in\n");
        input(fd, "hexdump 0 hexdump.in\n");
        input(fd, "hexdump 1 hexdump.in\n");
        input(fd, "hexdump 0 1 hexdump.in\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(
        output,
        "hexdump\n"
        "hexdump [[<offset>] <size>] <file>\n"
        "ERROR(-1)\n"
        "$ hexdump hexdump.in\n"
        "00000000: 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 '0123456789012345'\n"
        "00000010: 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 '6789012345678901'\n"
        "00000020: 32 33 34 35 36 37 38 39                         '23456789'\n"
        "OK\n"
        "$ hexdump 0 hexdump.in\n"
        "OK\n"
        "$ hexdump 1 hexdump.in\n"
        "00000000: 30                                              '0'\n"
        "OK\n"
        "$ hexdump 0 1 hexdump.in\n"
        "00000000: 30                                              '0'\n"
        "OK\n"
        "$ exit\n");
}

TEST(command_editing)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();

        /* 1. Ctrl-A + Ctrl-D + Enter. */
        input(fd, "12");
        input(fd, "\x01\x04\n");

        /* 2. Left arrow + Ctrl-T + Enter. */
        input(fd, "12");
        input(fd, ESC"[D\x14\n");

        /* 3. Left arrow + Space + Enter. */
        input(fd, "12");
        input(fd, ESC"[D \n");

        /* 4. Left arrow + Right arrow+ Enter. */
        input(fd, "12");
        input(fd, ESC"[D"ESC"[C""\n");

        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(
        output,
        /* 1. Ctrl-A + Ctrl-D + Enter. */
        "12"ESC"[2D"ESC"[K2"ESC"[1D\n"
        "2: command not found\n"
        "ERROR(-1)\n"

        /* 2. Left arrow + Ctrl-T + Enter. */
        "$ 12"ESC"[1D"ESC"[1D"ESC"[K21\n"
        "21: command not found\n"
        "ERROR(-1)\n"

        /* 3. Left arrow + Space + Enter. */
        "$ 12"ESC"[1D"ESC"[1D"ESC"[K1 2"ESC"[1D\n"
        "1: command not found\n"
        "ERROR(-1)\n"

        /* 4. Left arrow + Right arrow + Enter. */
        "$ 12"ESC"[1D"ESC"[1C\n"
        "12: command not found\n"
        "ERROR(-1)\n"

        "$ exit\n");
}

static int command_quotes(int argc, const char *argv[])
{
    ASSERT_EQ(argc, 3);
    ASSERT_EQ(strcmp(argv[0], "quotes"), 0);
    ASSERT_EQ(strcmp(argv[1], "ba\" \\r"), 0);
    ASSERT_EQ(strcmp(argv[2], ""), 0);

    return (0);
}

TEST(quotes)
{
    int fd;

    ml_shell_init();
    ml_shell_register_command("quotes", ".", command_quotes);

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "quotes \"ba\\\" \\r\" \"\"\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(
        output,
        "quotes \"ba\\\" \\r\" \"\"\n"
        "OK\n"
        "$ exit\n");
}

TEST(history)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "foo\n");
        input(fd, "bar\n");
        input(fd, "fie\n");
        input(fd, "history\n");
        input(fd, "\x12""fo\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(
        output,
        "foo\n"
        "foo: command not found\n"
        "ERROR(-1)\n"
        "$ bar\n"
        "bar: command not found\n"
        "ERROR(-1)\n"
        "$ fie\n"
        "fie: command not found\n"
        "ERROR(-1)\n"
        "$ history\n"
        "1: foo\n"
        "2: bar\n"
        "3: fie\n"
        "4: history\n"
        "OK\n"
        "$ "ESC"[K(history-search)`': "ESC"[3D"ESC"[Kf': fie"ESC"[6D"
        ESC"[Ko': foo"ESC"[6D"ESC"[19D"ESC"[Kfoo\n"
        "foo: command not found\n"
        "ERROR(-1)\n"
        "$ exit\n");
}

TEST(command_insmod)
{
    int fd;

    fd = 99;
    mock_push_ml_open("foo.ko", O_RDONLY, fd);
    mock_push_ml_finit_module(fd, "", 0, 0);
    mock_push_ml_close(fd, 0);

    fd = 98;
    mock_push_ml_open("bar.ko", O_RDONLY, fd);
    mock_push_ml_finit_module(fd, "fie=fum", 0, 0);
    mock_push_ml_close(fd, 0);

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "insmod\n");
        input(fd, "insmod foo.ko\n");
        input(fd, "insmod bar.ko fie=fum\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "insmod\n"
              "insmod <file> [<params>]\n"
              "ERROR(-1)\n"
              "$ insmod foo.ko\n"
              "OK\n"
              "$ insmod bar.ko fie=fum\n"
              "OK\n"
              "$ exit\n");

    mock_finalize();
}

TEST(command_df_setmntent_failure)
{
    int fd;

    mock_push_setmntent("/proc/mounts", "r", NULL);

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "df\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "df\n"
              "ERROR(-1)\n"
              "$ exit\n");

    mock_finalize();
}

TEST(command_df)
{
    int fd;
    FILE *f_p;
    struct mntent *mntent_root_p;
    struct mntent *mntent_proc_p;
    struct statvfs stat;

    f_p = (FILE *)1;
    mock_push_setmntent("/proc/mounts", "r", f_p);

    /* /. */
    mntent_root_p = xmalloc(sizeof(*mntent_root_p));
    mntent_root_p->mnt_dir = "/";
    mock_push_getmntent(f_p, mntent_root_p);
    stat.f_bsize = 512;
    stat.f_blocks = 20000;
    stat.f_bfree = 15000;
    mock_push_statvfs("/", &stat, 0);

    /* /proc. */
    mntent_proc_p = xmalloc(sizeof(*mntent_proc_p));
    mntent_proc_p->mnt_dir = "/proc";
    mock_push_getmntent(f_p, mntent_proc_p);
    stat.f_bsize = 512;
    stat.f_blocks = 40000;
    stat.f_bfree = 10000;
    mock_push_statvfs("/proc", &stat, 0);

    /* No more mounted file systems. */
    mock_push_getmntent(f_p, NULL);
    mock_push_endmntent(f_p, 0);

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "df\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "df\n"
              "MOUNTED ON               TOTAL      USED      FREE\n"
              "/                         9 MB      2 MB      7 MB\n"
              "/proc                    19 MB     14 MB      5 MB\n"
              "OK\n"
              "$ exit\n");

    free(mntent_root_p);
    free(mntent_proc_p);

    mock_finalize();
}

TEST(command_suicide_no_args)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "suicide\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "suicide\n"
              "suicide {exit,segfault}\n"
              "ERROR(-1)\n"
              "$ exit\n");

    mock_finalize();
}

TEST(command_find_too_many_args)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "find a b\n");
        input(fd, "exit\n");
        ml_shell_join();
    }

    ASSERT_EQ(output,
              "find a b\n"
              "find [<path>]\n"
              "ERROR(-1)\n"
              "$ exit\n");

    mock_finalize();
}

int main()
{
    ml_init();

    return RUN_TESTS(
        various_commands,
        command_ls,
        command_cat,
        command_hexdump,
        command_editing,
        quotes,
        history,
        command_insmod,
        command_df_setmntent_failure,
        command_df,
        command_suicide_no_args,
        command_find_too_many_args
    );
}
