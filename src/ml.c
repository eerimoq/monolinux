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
 * This file is part of the monolinux project.
 */

#include <stdlib.h>
#include "ml.h"

struct module_t {
    struct ml_bus_t bus;
};

static struct module_t module;

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
