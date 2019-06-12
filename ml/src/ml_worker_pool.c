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

#include "ml/ml.h"

struct worker_pool_job_message_t {
    ml_worker_pool_job_entry_t entry;
    void *arg_p;
};

static ML_UID(worker_pool_job_mid);

static void *worker_pool_main(void *arg_p)
{
    struct ml_worker_pool_t *self_p;
    struct worker_pool_job_message_t *message_p;

    self_p = (struct ml_worker_pool_t *)arg_p;

    while (true) {
        (void)ml_queue_get(&self_p->jobs, (void **)&message_p);
        message_p->entry(message_p->arg_p);
        ml_message_free(message_p);
    }

    return (NULL);
}

void ml_worker_pool_init(struct ml_worker_pool_t *self_p,
                         int number_of_workers,
                         int job_queue_length)
{
    pthread_t pthread;
    int i;

    ml_queue_init(&self_p->jobs, job_queue_length);

    for (i = 0; i < number_of_workers; i++) {
        pthread_create(&pthread, NULL, worker_pool_main, self_p);
    }
}

void ml_worker_pool_spawn(struct ml_worker_pool_t *self_p,
                          ml_worker_pool_job_entry_t entry,
                          void *arg_p)
{
    struct worker_pool_job_message_t *message_p;

    message_p = ml_message_alloc(&worker_pool_job_mid, sizeof(*message_p));
    message_p->entry = entry;
    message_p->arg_p = arg_p;
    ml_queue_put(&self_p->jobs, message_p);
}
