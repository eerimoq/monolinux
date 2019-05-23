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

#ifndef ML_H
#define ML_H

#include <stdbool.h>
#include <pthread.h>

/**
 * Create a unique identifier.
 */
#define ML_UID(name)                            \
    struct ml_uid_t name = {                    \
        .name_p = #name                         \
    }

#define ML_LOG_EMERGENCY   0
#define ML_LOG_ALERT       1
#define ML_LOG_CRITICAL    2
#define ML_LOG_ERROR       3
#define ML_LOG_WARNING     4
#define ML_LOG_NOTICE      5
#define ML_LOG_INFO        6
#define ML_LOG_DEBUG       7

#define ML_LOG_MASK(level) (1 << (ML_LOG_ ## level))
#define ML_LOG_UPTO(level) ((1 << (ML_LOG_ ## level + 1)) - 1)
#define ML_LOG_ALL         ML_LOG_UPTO(DEBUG)
#define ML_LOG_NONE        0x00

#define ML_EMERGENCY(log_object_p, fmt_p, ...)                          \
    ml_log_object_print(log_object_p, ML_LOG_EMERGENCY, fmt_p, ##__VA_ARGS__)
#define ML_ALERT(log_object_p, fmt_p, ...)                              \
    ml_log_object_print(log_object_p, ML_LOG_ALERT, fmt_p, ##__VA_ARGS__)
#define ML_CRITICAL(log_object_p, fmt_p, ...)                           \
    ml_log_object_print(log_object_p, ML_LOG_CRITICAL, fmt_p, ##__VA_ARGS__)
#define ML_ERROR(log_object_p, fmt_p, ...)                              \
    ml_log_object_print(log_object_p, ML_LOG_ERROR, fmt_p, ##__VA_ARGS__)
#define ML_WARNING(log_object_p, fmt_p, ...)                            \
    ml_log_object_print(log_object_p, ML_LOG_WARNING, fmt_p, ##__VA_ARGS__)
#define ML_NOTICE(log_object_p, fmt_p, ...)                             \
    ml_log_object_print(log_object_p, ML_LOG_NOTICE, fmt_p, ##__VA_ARGS__)
#define ML_INFO(log_object_p, fmt_p, ...)                               \
    ml_log_object_print(log_object_p, ML_LOG_INFO, fmt_p, ##__VA_ARGS__)
#define ML_DEBUG(log_object_p, fmt_p, ...)                              \
    ml_log_object_print(log_object_p, ML_LOG_DEBUG, fmt_p, ##__VA_ARGS__)

typedef int (*ml_shell_command_callback_t)(int argc, const char *argv[]);

struct ml_uid_t {
    const char *name_p;
};

struct ml_message_header_t {
    struct ml_uid_t *uid_p;
    int count;
};

struct ml_queue_t {
    int rdpos;
    int wrpos;
    int length;
    struct ml_message_header_t **messages_p;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct ml_bus_elem_t {
    struct ml_uid_t *uid_p;
    int number_of_queues;
    struct ml_queue_t **queues_pp;
};

struct ml_bus_t {
    int number_of_elems;
    struct ml_bus_elem_t *elems_p;
};

struct ml_log_object_t {
    const char *name_p;
    int mask;
};

/**
 * Initialize the Monolinux module. This must be called before any
 * other function in this module.
 */
void ml_init(void);

/**
 * Get the name of given unique id.
 */
const char *ml_uid_str(struct ml_uid_t *uid_p);

/**
 * Subscribe to given message on the default bus.
 */
void ml_subscribe(struct ml_queue_t *queue_p, struct ml_uid_t *uid_p);

/**
 * Broadcast given message on the default bus.
 */
void ml_broadcast(void *message_p);

/**
 * Allocate a message with given id and size.
 */
void *ml_message_alloc(struct ml_uid_t *uid_p, size_t size);

/**
 * Free given message.
 */
void ml_message_free(void *message_p);

/**
 * Initialize given message queue. Only one thread may get messages
 * from a queue. Multiple threads may put messages on a queue.
 */
void ml_queue_init(struct ml_queue_t *self_p, int length);

/**
 * Get the oldest message from given message queue. It is forbidden to
 * modify the message as it may be shared between multiple
 * threads. Free the message once used.
 */
struct ml_uid_t *ml_queue_get(struct ml_queue_t *self_p, void **message_pp);

/**
 * Put given message into given queue.
 */
void ml_queue_put(struct ml_queue_t *self_p, void *message_p);

/**
 * Initialize given bus.
 */
void ml_bus_init(struct ml_bus_t *self_p);

/**
 * Subscribe to given message. This function must only be called
 * before any message is broadcasted on given bus.
 */
void ml_bus_subscribe(struct ml_bus_t *self_p,
                      struct ml_queue_t *queue_p,
                      struct ml_uid_t *uid_p);

/**
 * Broadcast given message on given bus. All subscribers will receive
 * the message.
 */
void ml_bus_broadcast(struct ml_bus_t *self_p, void *message_p);

/**
 * Initialize given log object with given name and mask.
 */
void ml_log_object_init(struct ml_log_object_t *self_p,
                        const char *name_p,
                        int mask);

/**
 * Set given log mask for given log object.
 */
void ml_log_object_set_mask(struct ml_log_object_t *self_p,
                            int mask);

/**
 * Check if given log level is enabled in given log object.
 */
bool ml_log_object_is_enabled_for(struct ml_log_object_t *self_p,
                                  int level);

/**
 * Check if given log level is set in the log object mask. If so,
 * format a log entry and print it.
 */
void ml_log_object_print(struct ml_log_object_t *self_p,
                         int level,
                         const char *fmt_p,
                         ...);

void ml_shell_init(void);

void ml_shell_start(void);

/**
 * Must be called before ml_shell_start().
 */
void ml_shell_register_command(const char *name_p,
                               const char *description_p,
                               ml_shell_command_callback_t callback);

void ml_network_init(void);

void ml_network_interface_configure(const char *name_p,
                                    const char *ipv4_address_p,
                                    const char *ipv4_netmask_p);

void ml_network_interface_up(const char *name_p);

void ml_network_interface_down(const char *name_p);

char *strip(char *str_p, const char *strip_p);

void *xmalloc(size_t size);

void *xrealloc(void *buf_p, size_t size);

int xmount(const char *source_p,
           const char *target_p,
           const char *type_p);

#define membersof(array) (sizeof(array) / sizeof((array)[0]))

#endif
