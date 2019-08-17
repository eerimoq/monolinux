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

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ml/ml.h"

#define WAIT_FOREVER                              -1

static void destroy(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

static void *reader_main(void *arg_p)
{
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    ML_INFO("Reader task starting...");
    ML_INFO("Reader task stopped.");

    return (NULL);
}

static void *keep_alive_main(void *arg_p)
{
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    ML_INFO("Keep alive task starting...");

    while (true) {
        sleep(self_p->ping_period_s);
        ML_DEBUG("Pinging the broker.");
    }

    ML_INFO("Keep alive task stopped.");

    return (NULL);
}

static void write_publish(struct ml_mqtt_client_t *self_p,
                          struct ml_mqtt_client_message_t *message_p)
{
    struct iovec iovec[4];
    uint8_t fixed_header[8];
    uint8_t properties[16];

    iovec[0].iov_base = &header[0];
    iovec[0].iov_len = pack_fixed_header(&header[0],
                                         CONTROL_PACKET_TYPE_PUBLISH,
                                         0,
                                         size);
    iovec[1].iov_base = message_p->topic_p;
    iovec[1].iov_len = strlen(message_p->topic_p);
    iovec[2].iov_base = &properties[0];
    iovec[2].iov_len = pack_properties(&properties[0]);
    iovec[3].iov_base = message_p->message.buf_p;
    iovec[3].iov_len = message_p->message.size;

    writev(self_p->broker_fd, &iovec[0], membersof(iovec));
}

static void handle_publish(struct ml_mqtt_client_t *self_p)
{
    struct ml_mqtt_client_message_t *message_p;

    pthread_mutex_lock(&self_p->publish.mutex);
    message_p = self_p->publish.head.next_p;
    self_p->publish.head.next_p = NULL;
    self_p->publish.tail_p = &self_p->publish.head;
    pthread_mutex_unlock(&self_p->publish.mutex);

    while (message_p != NULL) {
        write_publish(self_p, message_p);
        message_p = message_p->next_p;
    }
}

static void handle_input(struct ml_mqtt_client_t *self_p)
{
    read_packet();

    switch (packet_type) {

    case CONTROL_PACKET_TYPE_CONNACK:
        on_connack(self_p);
        break;
        
    case CONTROL_PACKET_TYPE_PUBLISH:
        on_publish(self_p);
        break;
        
    case CONTROL_PACKET_TYPE_SUBACK:
        on_suback(self_p);
        break;
        
    case CONTROL_PACKET_TYPE_UNSUBACK:
        on_unsuback(self_p);
        break;
        
    case CONTROL_PACKET_TYPE_PINGRESP:
        on_pingresp(self_p);
        break;
        
    case CONTROL_PACKET_TYPE_DISCONNECT:
        on_disconnect(self_p);
        break;

    default:
        break;
    }
}

static void *client_main(void *arg_p)
{
    int res;
    struct ml_mqtt_client_t *self_p;

    self_p = (struct ml_mqtt_client_t *)arg_p;

    while (true) {
        res = poll(&self_p->fds[0], membersof(self_p->fds), WAIT_FOREVER);

        if (res <= 0) {
            ML_INFO("Poll returned %d.", res);
            break;
        }

        if (self_p->fds[PUBLISH_IX].revents & POLLIN) {
            handle_publish_event(self_p);
        }

        if (self_p->fds[INPUT_IX].revents & POLLIN) {
            handle_input(self_p);
        }
    }

    return (NULL);
}

void ml_mqtt_client_init(struct ml_mqtt_client_t *self_p,
                         const char *host_p,
                         int port,
                         int log_mask,
                         struct ml_mqtt_client_subscription_t *subscriptions_p,
                         size_t number_of_subscriptions)
{
    ml_queue_init(&self_p->queue, 32);
    self_p->host_p = host_p;
    self_p->port = port;
    self_p->ping_period_s = 10;
    ml_log_object_init(&self_p->log_object,
                       "mqtt_client",
                       log_mask);
    self_p->subscriptions_p = subscriptions_p;
    self_p->number_of_subscriptions = number_of_subscriptions;
    self_p->publish.eventfd = eventfd(0, 0);
    pthread_mutex_init(&self_p->publish.mutex);
    self_p->publish.head.next_p = NULL;
    self_p->publish.tail_p = &self_p->publish.head;
}

int ml_mqtt_client_start(struct ml_mqtt_client_t *self_p)
{
    int res;

    ML_INFO("Connecting to broker at '%s:%d'.", self_p->host_p, self_p->port);

    res = pthread_create(&self_p->client_pthread,
                         NULL,
                         client_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    res = pthread_create(&self_p->reader_pthread,
                         NULL,
                         reader_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    res = pthread_create(&self_p->keep_alive_pthread,
                         NULL,
                         keep_alive_main,
                         self_p);

    if (res != 0) {
        goto err;
    }

    return (0);

 err:
    destroy(self_p);

    return (res);
}

void ml_mqtt_client_stop(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

void ml_mqtt_client_join(struct ml_mqtt_client_t *self_p)
{
    (void)self_p;
}

bool ml_mqtt_client_message_is_topic(
    struct ml_mqtt_client_message_t *message_p,
    const char *topic_p)
{
    return (strcmp(message_p->topic_p, topic_p) == 0);
}

struct ml_uid_t *ml_mqtt_client_message_uid(void)
{
    return (NULL);
}

void ml_mqtt_client_publish(struct ml_mqtt_client_t *self_p,
                            struct ml_mqtt_client_message_t *message_p)
{
    message_p->next_p = NULL;
    pthread_mutex_lock(&self_p->publish.mutex);
    self_p->publish.tail_p->next_p = message_p;
    self_p->publish.tail_p = message_p;
    pthread_mutex_unlock(&self_p->publish.mutex);
    write(self_p->publish.eventfd, 0, sizeof(uint64_t));
}
