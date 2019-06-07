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

#ifndef ML_ML_H
#define ML_ML_H

#include <netinet/in.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <poll.h>

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

/* Requires "self_p->log_object". */
#define ML_EMERGENCY(fmt_p, ...)                                        \
    ml_log_object_print(&self_p->log_object, ML_LOG_EMERGENCY, fmt_p, ##__VA_ARGS__)
#define ML_ALERT(fmt_p, ...)                              \
    ml_log_object_print(&self_p->log_object, ML_LOG_ALERT, fmt_p, ##__VA_ARGS__)
#define ML_CRITICAL(fmt_p, ...)                           \
    ml_log_object_print(&self_p->log_object, ML_LOG_CRITICAL, fmt_p, ##__VA_ARGS__)
#define ML_ERROR(fmt_p, ...)                              \
    ml_log_object_print(&self_p->log_object, ML_LOG_ERROR, fmt_p, ##__VA_ARGS__)
#define ML_WARNING(fmt_p, ...)                            \
    ml_log_object_print(&self_p->log_object, ML_LOG_WARNING, fmt_p, ##__VA_ARGS__)
#define ML_NOTICE(fmt_p, ...)                             \
    ml_log_object_print(&self_p->log_object, ML_LOG_NOTICE, fmt_p, ##__VA_ARGS__)
#define ML_INFO(fmt_p, ...)                               \
    ml_log_object_print(&self_p->log_object, ML_LOG_INFO, fmt_p, ##__VA_ARGS__)
#define ML_DEBUG(fmt_p, ...)                              \
    ml_log_object_print(&self_p->log_object, ML_LOG_DEBUG, fmt_p, ##__VA_ARGS__)

#define PRINT_FILE_LINE() printf("%s:%d\n", __FILE__, __LINE__);

#define membersof(array) (sizeof(array) / sizeof((array)[0]))

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

enum ml_dhcp_client_state_t {
    ml_dhcp_client_state_init_t = 0,
    ml_dhcp_client_state_selecting_t,
    ml_dhcp_client_state_requesting_t,
    ml_dhcp_client_state_bound_t,
    ml_dhcp_client_state_renewing_t
};

enum ml_dhcp_client_packet_type_t {
    ml_dhcp_client_packet_type_offer_t = 0,
    ml_dhcp_client_packet_type_ack_t,
    ml_dhcp_client_packet_type_nak_t,
    ml_dhcp_client_packet_type_none_t
};

struct ml_dhcp_client_t {
    enum ml_dhcp_client_state_t state;
    enum ml_dhcp_client_packet_type_t packet_type;
    struct in_addr ip_address;
    struct in_addr subnet_mask;
    struct in_addr gateway;
    struct in_addr dns;
    int lease_time;
    int renewal_interval;
    int rebinding_time;
    bool renewal_timer_expired;
    bool rebinding_timer_expired;
    bool response_timer_expired;
    bool init_timer_expired;
    bool is_packet_socket;
    struct pollfd fds[5];
    struct {
        struct in_addr ip_address;
    } server;
    struct {
        const char *name_p;
        int index;
        uint8_t mac_address[6];
    } interface;
    pthread_t pthread;
    struct ml_log_object_t log_object;
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

/**
 * Initialize the shell. Commands may be registered after this
 * function has been called.
 */
void ml_shell_init(void);

/**
 * Start the shell. No commands may be registered after this function
 * has been called.
 */
void ml_shell_start(void);

/**
 * Wait for the shell thread to terminate.
 */
void ml_shell_join(void);

/**
 * Must be called before ml_shell_start().
 */
void ml_shell_register_command(const char *name_p,
                               const char *description_p,
                               ml_shell_command_callback_t callback);

/**
 * Initialize the motwork module.
 */
void ml_network_init(void);

/**
 * Configure given network interface.
 */
int ml_network_interface_configure(const char *name_p,
                                   const char *ipv4_address_p,
                                   const char *ipv4_netmask_p);

/**
 * Bring up given network interface.
 */
int ml_network_interface_up(const char *name_p);

/**
 * Take down given network interface.
 */
int ml_network_interface_down(const char *name_p);

/**
 * Network interface index.
 */
int ml_network_interface_index(const char *name_p, int *index_p);

/**
 * Network interface MAC address.
 */
int ml_network_interface_mac_address(const char *name_p,
                                     uint8_t *mac_address_p);

/**
 * Network interface IP address.
 */
int ml_network_interface_ip_address(const char *name_p,
                                    struct in_addr *in_addr_p);

/**
 * Strip leading and trailing characters from given string and return
 * a pointer to the beginning of the new string.
 */
char *ml_strip(char *str_p, const char *strip_p);

/**
 * Strip leading characters from given string and return a pointer to
 * the beginning of the new string.
 */
char *ml_lstrip(char *str_p, const char *strip_p);

/**
 * Strip trailing characters from given string.
 */
void ml_rstrip(char *str_p, const char *strip_p);

/**
 * Print a hexdump of given buffer.
 */
void ml_hexdump(const void *buf_p, size_t size);

/**
 * Print a hexdump of given file to given file.
 */
int ml_hexdump_file(FILE *fin_p, size_t offset, ssize_t size);

/**
 * Print given file.
 */
void ml_print_file(const char *name_p);

/**
 * Print system uptime.
 */
void ml_print_uptime(void);

/**
 * Insert given kernel module.
 */
int ml_insert_module(const char *path_p, const char *params_p);

/**
 * Get file system space usage.
 */
int ml_file_system_space_usage(const char *path_p,
                               unsigned long *total_p,
                               unsigned long *used_p,
                               unsigned long *free_p);

/**
 * Print file systems space usage.
 */
int ml_print_file_systems_space_usage(void);

int ml_mount(const char *source_p,
             const char *target_p,
             const char *type_p);

int ml_socket(int domain, int type, int protocol);

int ml_ioctl(int fd, unsigned long request, void *data_p);

int finit_module(int fd, const char *params_p, int flags);

/**
 * Initialize a DHCP client for given ethernet interface.
 */
void ml_dhcp_client_init(struct ml_dhcp_client_t *self_p,
                         const char *interface_name_p,
                         int log_mask);

/**
 * Start given client.
 */
int ml_dhcp_client_start(struct ml_dhcp_client_t *self_p);

/**
 * Stop given client.
 */
void ml_dhcp_client_stop(struct ml_dhcp_client_t *self_p);

/**
 * Join given client.
 */
int ml_dhcp_client_join(struct ml_dhcp_client_t *self_p);

/**
 * @return "true" or "false" strings.
 */
const char *ml_bool_str(bool value);

/**
 * Internet checksum.
 */
uint32_t ml_inet_checksum_begin(void);

uint32_t ml_inet_checksum_acc(uint32_t acc,
                              const uint16_t *buf_p,
                              size_t size);

uint16_t ml_inet_checksum_end(uint32_t acc);

uint16_t ml_inet_checksum(const void *buf_p, size_t size);

/* Exits on failure. Use with care. */

void *xmalloc(size_t size);

void *xrealloc(void *buf_p, size_t size);

/* For mocking. */

int ml_open(const char *path_p, int flags);

int ml_close(int fd);

ssize_t ml_read(int fd, void *buf_p, size_t count);

ssize_t ml_write(int fd, const void *buf_p, size_t count);

int ml_finit_module(int fd, const char *params_p, int flags);

int ml_mknod(const char *path_p, mode_t mode, dev_t dev);

#if defined(__GNU_LIBRARY__) && (__GLIBC__ <= 2) && (__GLIBC_MINOR__ <= 26)
int memfd_create(const char *name, unsigned flags);
#endif

#endif
