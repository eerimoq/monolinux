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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ftw.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <syslog.h>

static int pmount(const char *source_p,
                  const char *target_p,
                  const char *type_p)
{
    int res;

    res = mount(source_p, target_p, type_p, 0, "");

    if (res != 0) {
        perror("error: mount: ");
    }

    return (res);
}

static void make_stdin_unbuffered(void)
{
    struct termios ctrl;

    tcgetattr(STDIN_FILENO, &ctrl);
    ctrl.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}

static int init(void)
{
    int res;

    make_stdin_unbuffered();

    res = pmount("none", "/proc", "proc");

    if (res != 0) {
        return (res);
    }

    res = pmount("none", "/sys", "sysfs");

    if (res != 0) {
        return (res);
    }

    return (pmount("none", "/sys/kernel/debug", "debugfs"));
}

static void print_banner(void)
{
    int fd;
    char buf[128];
    ssize_t size;

    fd = open("/proc/uptime", O_RDONLY);

    if (fd < 0) {
        perror("error: ");

        return;
    }

    size = read(fd, &buf[0], sizeof(buf) - 1);

    if (size >= 0) {
        buf[size] = '\0';
        strtok(&buf[0], " ");
    } else {
        strcpy(&buf[0], "-");
    }

    close(fd);

    printf("Welcome to Monolinux!\n"
           "\n"
           "Uptime: %s\n"
           "\n",
           &buf[0]);
}

int print_info(const char *fpath,
               const struct stat *sb,
               const int tflag,
               struct FTW *ftwbuf)
{
    printf("%-3s %2d ",
           (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
           (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
           (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
           (tflag == FTW_SLN) ? "sln" : "???",
           ftwbuf->level);

    if (tflag == FTW_NS) {
        printf("-------");
    } else {
        printf("%7jd", (intmax_t) sb->st_size);
    }

    printf("   %-40s %d %s\n", fpath, ftwbuf->base, fpath + ftwbuf->base);

    return (0);
}

static void print_filesystem(void)
{
    nftw("/", print_info, 20, FTW_PHYS);
}

static void slog(void)
{
    printf("writing log\n");
    openlog("apa", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

    syslog(LOG_MAKEPRI(LOG_USER, LOG_NOTICE),
           "Program started by user %d",
           getuid());
    syslog(LOG_INFO, "A tree falls in a forest");

    closelog();
}

int main()
{
    int res;
    char ch;

    res = init();

    if (res != 0) {
        printf("error: Init failed. Aborting.\n");

        return (res);
    }

    print_banner();
    print_filesystem();
    slog();

    while (1) {
        res = read(STDIN_FILENO, &ch, sizeof(ch));

        if (res == sizeof(ch)) {
            printf("echo: %c\n", ch);
        } else {
            printf("error: Failed to read from stdin.");
        }
    }

    return (0);
}
