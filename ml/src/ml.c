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

#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mount.h>
#include "ml/ml.h"
#include "internal.h"

struct module_t {
    struct ml_bus_t bus;
};

static struct module_t module;

static inline bool char_in_string(char c, const char *str_p)
{
    while (*str_p != '\0') {
        if (c == *str_p) {
            return (true);
        }

        str_p++;
    }

    return (false);
}

static void print_ascii(const uint8_t *buf_p, size_t size)
{
    size_t i;

    for (i = 0; i < 16 - size; i++) {
        printf("   ");
    }

    printf("'");

    for (i = 0; i < size; i++) {
        printf("%c", isprint((int)buf_p[i]) ? buf_p[i] : '.');
    }

    printf("'");
}

static void hexdump(const uint8_t *buf_p, size_t size, int offset)
{
    int pos;

    pos = 0;

    while (size > 0) {
        if ((pos % 16) == 0) {
            printf("%08x: ", offset + pos);
        }

        printf("%02x ", buf_p[pos] & 0xff);

        if ((pos % 16) == 15) {
            print_ascii(&buf_p[pos - 15], 16);
            printf("\n");
        }

        pos++;
        size--;
    }

    if ((pos % 16) != 0) {
        print_ascii(&buf_p[pos - (pos % 16)], pos % 16);
        printf("\n");
    }
}

void ml_init(void)
{
    ml_message_init();
    ml_bus_init(&module.bus);
}

const char *ml_uid_str(struct ml_uid_t *uid_p)
{
    return (uid_p->name_p);
}

void ml_broadcast(void *message_p)
{
    ml_bus_broadcast(&module.bus, message_p);
}

void ml_subscribe(struct ml_queue_t *queue_p, struct ml_uid_t *uid_p)
{
    ml_bus_subscribe(&module.bus, queue_p, uid_p);
}

char *ml_strip(char *str_p, const char *strip_p)
{
    ml_rstrip(str_p, strip_p);

    return (ml_lstrip(str_p, strip_p));
}

char *ml_lstrip(char *str_p, const char *strip_p)
{
    /* Strip whitespace characters by default. */
    if (strip_p == NULL) {
        strip_p = "\t\n\x0b\x0c\r ";
    }

    /* String leading characters. */
    while ((*str_p != '\0') && char_in_string(*str_p, strip_p)) {
        str_p++;
    }

    return (str_p);
}

void ml_rstrip(char *str_p, const char *strip_p)
{
    char *begin_p;
    size_t length;

    /* Strip whitespace characters by default. */
    if (strip_p == NULL) {
        strip_p = "\t\n\x0b\x0c\r ";
    }

    begin_p = str_p;

    /* Strip training characters. */
    length = strlen(str_p);
    str_p += (length - 1);

    while ((str_p >= begin_p) && char_in_string(*str_p, strip_p)) {
        *str_p = '\0';
        str_p--;
    }
}

void ml_hexdump(const void *buf_p, size_t size)
{
    hexdump(buf_p, size, 0);
}

int ml_hexdump_file(FILE *fin_p, size_t offset, ssize_t size)
{
    uint8_t buf[256];
    size_t chunk_size;

    if (fseek(fin_p, offset, SEEK_SET) != 0) {
        return (-1);
    }

    while (true) {
        if (size == -1) {
            chunk_size = sizeof(buf);
        } else {
            if ((size_t)size < sizeof(buf)) {
                chunk_size = size;
            } else {
                chunk_size = sizeof(buf);
            }
        }

        chunk_size = fread(&buf[0], 1, chunk_size, fin_p);
        hexdump(&buf[0], chunk_size, offset);

        if (chunk_size < sizeof(buf)) {
            break;
        }

        offset += chunk_size;
        size -= chunk_size;
    }

    return (0);
}

void ml_print_file(const char *name_p)
{
    FILE *fin_p;
    uint8_t buf[256];
    size_t size;

    fin_p = fopen(name_p, "rb");

    if (fin_p != NULL) {
        while ((size = fread(&buf[0], 1, membersof(buf), fin_p)) > 0) {
            if (fwrite(&buf[0], 1, size, stdout) != size) {
                break;
            }
        }

        fclose(fin_p);
    }
}

void ml_print_uptime(void)
{
    printf("Uptime: ");
    ml_print_file("/proc/uptime");
    printf("\n");
}

int ml_insert_module(const char *path_p, const char *params_p)
{
    int res;
    int fd;

    res = -1;
    fd = ml_open(path_p, O_RDONLY);

    if (fd != -1) {
        res = ml_finit_module(fd, params_p, 0);
        ml_close(fd);
    }

    return (res);
}

void *xmalloc(size_t size)
{
    void *buf_p;

    buf_p = malloc(size);

    if (buf_p == NULL) {
        exit(1);
    }

    return (buf_p);
}

void *xrealloc(void *buf_p, size_t size)
{
    buf_p = realloc(buf_p, size);

    if (buf_p == NULL) {
        exit(1);
    }

    return (buf_p);
}

void xmount(const char *source_p,
            const char *target_p,
            const char *type_p)
{
    int res;

    res = mount(source_p, target_p, type_p, 0, "");

    if (res != 0) {
        perror("error: mount: ");
        exit(1);
    }
}
