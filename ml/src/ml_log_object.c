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
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include "ml/ml.h"
#include "internal.h"

static const char *level_to_string(int level)
{
    const char *name_p;

    switch (level) {

    case ML_LOG_EMERGENCY:
        name_p = "EMERGENCY";
        break;

    case ML_LOG_ALERT:
        name_p = "ALERT";
        break;

    case ML_LOG_CRITICAL:
        name_p = "CRITICAL";
        break;

    case ML_LOG_ERROR:
        name_p = "ERROR";
        break;

    case ML_LOG_WARNING:
        name_p = "WARNING";
        break;

    case ML_LOG_NOTICE:
        name_p = "NOTICE";
        break;

    case ML_LOG_INFO:
        name_p = "INFO";
        break;

    case ML_LOG_DEBUG:
        name_p = "DEBUG";
        break;

    default:
        name_p = "INVALID";
        break;
    }

    return (name_p);
}

void ml_log_object_init(struct ml_log_object_t *self_p,
                     const char *name_p,
                     int mask)
{
    self_p->name_p = name_p;
    self_p->mask = mask;
}

void ml_log_object_set_mask(struct ml_log_object_t *self_p,
                         int mask)
{
    self_p->mask = mask;
}

bool ml_log_object_is_enabled_for(struct ml_log_object_t *self_p,
                               int level)
{
    return ((self_p->mask & (1 << level)) != 0);
}

void ml_log_object_print(struct ml_log_object_t *self_p,
                      int level,
                      const char *fmt_p,
                      ...)
{
    char buf[16];
    time_t now;
    struct tm tm;
    va_list ap;

    if ((self_p->mask & (1 << level)) == 0) {
        return;
    }

    now = time(NULL);
    gmtime_r(&now, &tm);
    strftime(&buf[0], sizeof(buf), "%b %e %T", &tm);

    printf("%s %s %s ", &buf[0], level_to_string(level), self_p->name_p);
    va_start(ap, fmt_p);
    vprintf(fmt_p, ap);
    va_end(ap);
    putchar('\n');
}
