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

static int stdin_pipe(void)
{
    int fds[2];

    ASSERT_EQ(pipe(fds), 0);
    dup2(fds[0], STDIN_FILENO);

    return (fds[1]);
}

static void input(int fd, const char *string_p)
{
    size_t length;

    length = strlen(string_p);

    ASSERT_EQ(write(fd, string_p, length), length);
}

TEST(various_commands)
{
    int fd;

    ml_shell_init();
    ml_shell_register_command("hello", "My command.", command_hello);

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "root\n");
        input(fd, "\n");
        input(fd, "help\n");
        input(fd, "history\n");
        input(fd, "hello\n");
        input(fd, "hello Foo\n");
        input(fd, "logout\n");
        /* ToDo: Should wait until output is available, but how? */
        usleep(50000);
    }

    ASSERT_EQ(output,
              "username: root\n"
              "password: \n"
              "$ help\n"
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
              "        hello   My command.\n"
              "         help   Print this help.\n"
              "      history   List comand history.\n"
              "       logout   Shell logout.\n"
              "           ls   List directory contents.\n"
              "       reboot   Reboot.\n"
              "$ history\n"
              "1: help\n"
              "2: history\n"
              "$ hello\n"
              "Hello stranger!\n"
              "OK\n"
              "$ hello Foo\n"
              "Hello Foo!\n"
              "OK\n"
              "$ logout\n"
              "username: ");
}

TEST(ls)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "root\n");
        input(fd, "\n");
        input(fd, "ls\n");
        /* ToDo: Should wait until output is available, but how?. */
        usleep(50000);
    }

    ASSERT_SUBSTRING(output, "OK\n$ ");
}

TEST(cat)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "root\n");
        input(fd, "\n");
        input(fd, "cat\n");
        input(fd, "cat Makefile\n");
        input(fd, "cat foobar\n");
        /* ToDo: Should wait until output is available, but how?. */
        usleep(50000);
    }

    ASSERT_EQ(
        output,
        "username: root\n"
        "password: \n"
        "$ cat\n"
        "No file given\n"
        "ERROR(-1)\n"
        "$ cat Makefile\n"
        "include $(ML_ROOT)/make/suite.mk\n"
        "OK\n"
        "$ cat foobar\n"
        "ERROR(-1)\n"
        "$ ");
}

TEST(command_editing)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "root\n");
        input(fd, "\n");

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

        /* ToDo: Should wait until output is available, but how?. */
        usleep(50000);
    }

    ASSERT_EQ(
        output,
        "username: root\n"
        "password: \n"

        /* 1. Ctrl-A + Ctrl-D + Enter. */
        "$ 12"ESC"[2D"ESC"[K2"ESC"[1D\n"
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

        "$ ");
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
        input(fd, "root\n");
        input(fd, "\n");
        input(fd, "quotes \"ba\\\" \\r\" \"\"\n");
        /* ToDo: Should wait until output is available, but how?. */
        usleep(50000);
    }

    ASSERT_EQ(
        output,
        "username: root\n"
        "password: \n"
        "$ quotes \"ba\\\" \\r\" \"\"\n"
        "OK\n"
        "$ ");
}

TEST(history)
{
    int fd;

    ml_shell_init();

    CAPTURE_OUTPUT(output) {
        fd = stdin_pipe();
        ml_shell_start();
        input(fd, "root\n");
        input(fd, "\n");
        input(fd, "foo\n");
        input(fd, "bar\n");
        input(fd, "fie\n");
        input(fd, "history\n");
        input(fd, "\x12""fo\n");
        /* ToDo: Should wait until output is available, but how?. */
        usleep(50000);
    }

    ASSERT_EQ(
        output,
        "username: root\n"
        "password: \n"
        "$ foo\n"
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
        "$ "ESC"[K(history-search)`': "ESC"[3D"ESC"[Kf': fie"ESC"[6D"
        ESC"[Ko': foo"ESC"[6D"ESC"[19D"ESC"[Kfoo\n"
        "foo: command not found\n"
        "ERROR(-1)\n"
        "$ ");
}

int main()
{
    ml_init();

    return RUN_TESTS(
        various_commands,
        ls,
        cat,
        command_editing,
        quotes,
        history
    );
}
