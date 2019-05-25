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
#include "ml/ml.h"
#include "internal.h"

static int compare_bsearch(const void *key_p, const void *elem_p)
{
    uintptr_t uid;
    uintptr_t elem_uid;

    uid = (uintptr_t)(struct ml_uid_t *)key_p;
    elem_uid = (uintptr_t)((struct ml_bus_elem_t *)elem_p)->uid_p;

    if (uid < elem_uid) {
        return (-1);
    } else if (uid == elem_uid) {
        return (0);
    } else {
        return (1);
    }
}

static int compare_qsort(const void *lelem_p, const void *relem_p)
{
    uintptr_t luid;
    uintptr_t ruid;

    luid = (uintptr_t)((struct ml_bus_elem_t *)lelem_p)->uid_p;
    ruid = (uintptr_t)((struct ml_bus_elem_t *)relem_p)->uid_p;

    if (luid < ruid) {
        return (-1);
    } else if (luid == ruid) {
        return (0);
    } else {
        return (1);
    }
}

static void insert_element(struct ml_bus_t *self_p, struct ml_uid_t *uid_p)
{
    struct ml_bus_elem_t *elem_p;

    self_p->number_of_elems++;
    self_p->elems_p = xrealloc(
        self_p->elems_p,
        sizeof(*self_p->elems_p) * self_p->number_of_elems);
    elem_p = &self_p->elems_p[self_p->number_of_elems - 1];
    elem_p->uid_p = uid_p;
    elem_p->number_of_queues = 0;
    elem_p->queues_pp = xmalloc(1);
    qsort(self_p->elems_p,
          self_p->number_of_elems,
          sizeof(*self_p->elems_p),
          compare_qsort);
}

static struct ml_bus_elem_t *find_element(struct ml_bus_t *self_p,
                                          struct ml_uid_t *uid_p)
{
    return (bsearch(uid_p,
                    self_p->elems_p,
                    self_p->number_of_elems,
                    sizeof(*self_p->elems_p),
                    compare_bsearch));
}

static void append_queue_to_element(struct ml_bus_elem_t *elem_p,
                                    struct ml_queue_t *queue_p)
{
    elem_p->number_of_queues++;
    elem_p->queues_pp = xrealloc(
        elem_p->queues_pp,
        sizeof(*elem_p->queues_pp) * elem_p->number_of_queues);
    elem_p->queues_pp[elem_p->number_of_queues - 1] = queue_p;
}

void ml_bus_init(struct ml_bus_t *self_p)
{
    self_p->number_of_elems = 0;
    self_p->elems_p = xmalloc(1);
}

void ml_bus_broadcast(struct ml_bus_t *self_p, void *message_p)
{
    struct ml_bus_elem_t *elem_p;
    int i;

    elem_p = find_element(self_p, message_to_header(message_p)->uid_p);

    if (elem_p != NULL) {
        ml_message_share(message_p, elem_p->number_of_queues - 1);

        for (i = 0; i < elem_p->number_of_queues; i++) {
            ml_queue_put(elem_p->queues_pp[i], message_p);
        }
    } else {
        ml_message_free(message_p);
    }
}

void ml_bus_subscribe(struct ml_bus_t *self_p,
                      struct ml_queue_t *queue_p,
                      struct ml_uid_t *uid_p)
{
    struct ml_bus_elem_t *elem_p;

    elem_p = find_element(self_p, uid_p);

    if (elem_p == NULL) {
        insert_element(self_p, uid_p);
        elem_p = find_element(self_p, uid_p);
    }

    append_queue_to_element(elem_p, queue_p);
}
