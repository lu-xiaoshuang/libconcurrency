/**
 * Copyright (C) 2018 Xiaoshuang LU
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

# include <stdatomic.h>
# ifndef __STDC_NO_THREADS__
# include <threads.h>
# endif
# include <time.h>

# include "linked_blocking_queue_1.h"
# include "memory_management.h"
# ifdef __STDC_NO_THREADS__
# include "threads_wrapper.h"
# endif
# include "time_utility.h"

struct linked_blocking_queue_1_node
{
    struct linked_blocking_queue_1_node * next;

    void * element;
};

static
void
signal_push
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    intmax_t value = 0;
    value = mtx_lock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    value = cnd_signal(& (linked_blocking_queue->condition_push));
    if (value != thrd_success)
    {
        goto condition_signal_failure;
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    ;
}

static
void
signal_pop
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    intmax_t value = 0;
    value = mtx_lock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    value = cnd_signal(& (linked_blocking_queue->condition_pop));
    if (value != thrd_success)
    {
        goto condition_signal_failure;
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    ;
}

static
void
do_push
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    struct linked_blocking_queue_1_node * linked_blocking_queue_node
)
{
    linked_blocking_queue->tail->next = linked_blocking_queue_node;
    linked_blocking_queue->tail = linked_blocking_queue_node;
    linked_blocking_queue_node->next = 0;
}

static
struct linked_blocking_queue_1_node *
do_pop
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    // If there is only one node in the queue, `next' of the first node
    // (i.e. `next' of head->next) maybe modified by a push thread concurrently.
    // Do not touch it.
    struct linked_blocking_queue_1_node * node = linked_blocking_queue->head;
    linked_blocking_queue->head = linked_blocking_queue->head->next;
    node->next = 0;
    node->element = linked_blocking_queue->head->element;
    linked_blocking_queue->head->element = 0;
    return node;
}

static
intmax_t
fully_lock
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    mtx_lock(& (linked_blocking_queue->mutex_push));
    mtx_lock(& (linked_blocking_queue->mutex_pop));
    return 0;
}

static
intmax_t
fully_unlock
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    mtx_unlock(& (linked_blocking_queue->mutex_pop));
    mtx_unlock(& (linked_blocking_queue->mutex_push));
    return 0;
}

intmax_t
linked_blocking_queue_1_init
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    intmax_t capacity,
    struct memory_management * memory_management
)
{
    if (linked_blocking_queue == 0)
    {
        return -1;
    }
    linked_blocking_queue->capacity = capacity;
    intmax_t value = 0;
    atomic_init(& (linked_blocking_queue->count), 0);
    mtx_init(& (linked_blocking_queue->mutex_push), (mtx_timed | mtx_recursive));
    cnd_init(& (linked_blocking_queue->condition_push));
    mtx_init(& (linked_blocking_queue->mutex_pop), (mtx_timed | mtx_recursive));
    cnd_init(& (linked_blocking_queue->condition_pop));
    linked_blocking_queue->memory_management.allocate = memory_management->allocate;
    linked_blocking_queue->memory_management.deallocate = memory_management->deallocate;
    linked_blocking_queue->memory_management.data = memory_management->data;
    struct linked_blocking_queue_1_node * node =
        linked_blocking_queue->memory_management.allocate
        (
            & (linked_blocking_queue->memory_management),
            sizeof(struct linked_blocking_queue_1_node)
        );
    node->next = 0;
    node->element = 0;
    linked_blocking_queue->head = node;
    linked_blocking_queue->tail = node;
    return 0;
}

intmax_t
linked_blocking_queue_1_destroy
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    // TODO
    // fully_lock(linked_blocking_queue);
    // deallocate
    // fully_unlock(linked_blocking_queue);
    linked_blocking_queue->memory_management.deallocate
    (
        & (linked_blocking_queue->memory_management),
        linked_blocking_queue->head
    );
    cnd_destroy(& (linked_blocking_queue->condition_pop));
    mtx_destroy(& (linked_blocking_queue->mutex_pop));
    cnd_destroy(& (linked_blocking_queue->condition_push));
    mtx_destroy(& (linked_blocking_queue->mutex_push));
    return 0;
}

intmax_t
linked_blocking_queue_1_capacity
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    return linked_blocking_queue->capacity;
}

intmax_t
linked_blocking_queue_1_size
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    return atomic_load(& (linked_blocking_queue->count));
}

intmax_t
linked_blocking_queue_1_remaining_capacity
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    return linked_blocking_queue->capacity - atomic_load(& (linked_blocking_queue->count));
}

intmax_t
linked_blocking_queue_1_empty
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    return atomic_load(& (linked_blocking_queue->count)) == 0;
}

intmax_t
linked_blocking_queue_1_push
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    void * element
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    intmax_t count = -1;
    while (atomic_load(& (linked_blocking_queue->count)) >= linked_blocking_queue->capacity)
    {
        value = cnd_wait(& (linked_blocking_queue->condition_push), & (linked_blocking_queue->mutex_push));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
    struct linked_blocking_queue_1_node * node =
        linked_blocking_queue->memory_management.allocate
        (
            & (linked_blocking_queue->memory_management),
            sizeof(struct linked_blocking_queue_1_node)
        );
    if (node == 0)
    {
        goto allocate_failure;
    }
    node->next = 0;
    node->element = element;
    do_push(linked_blocking_queue, node);
    return_value = 0;
    count = atomic_fetch_add(& (linked_blocking_queue->count), 1);
    if (count + 1 < linked_blocking_queue->capacity)
    {
        value = cnd_signal(& (linked_blocking_queue->condition_push));
        if (value != thrd_success)
        {
            goto condition_signal_failure;
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
allocate_success:
    ;
allocate_failure:
    ;
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == 0)
    {
        signal_pop(linked_blocking_queue);
    }
    return return_value;
}

intmax_t
linked_blocking_queue_1_try_push
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    void * element
)
{
    intmax_t return_value = -1;
    intmax_t count = -1;
    if (atomic_load(& (linked_blocking_queue->count)) >= linked_blocking_queue->capacity)
    {
        return return_value;
    }
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    if (atomic_load(& (linked_blocking_queue->count)) < linked_blocking_queue->capacity)
    {
        struct linked_blocking_queue_1_node * node =
            linked_blocking_queue->memory_management.allocate
            (
                & (linked_blocking_queue->memory_management),
                sizeof(struct linked_blocking_queue_1_node)
            );
        if (node == 0)
        {
            goto allocate_failure;
        }
        node->next = 0;
        node->element = element;
        do_push(linked_blocking_queue, node);
        return_value = 0;
        count = atomic_fetch_add(& (linked_blocking_queue->count), 1);
        if (count + 1 < linked_blocking_queue->capacity)
        {
            value = cnd_signal(& (linked_blocking_queue->condition_push));
            if (value != thrd_success)
            {
                goto condition_signal_failure;
            }
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
allocate_success:
    ;
allocate_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == 0)
    {
        signal_pop(linked_blocking_queue);
    }
    return return_value;
}

intmax_t
linked_blocking_queue_1_timed_push
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    void * element,
    struct timespec * timespec
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    intmax_t count = -1;
    while (atomic_load(& (linked_blocking_queue->count)) >= linked_blocking_queue->capacity)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_queue->condition_push), & (linked_blocking_queue->mutex_push), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
    struct linked_blocking_queue_1_node * node =
        linked_blocking_queue->memory_management.allocate
        (
            & (linked_blocking_queue->memory_management),
            sizeof(struct linked_blocking_queue_1_node)
        );
    if (node == 0)
    {
        goto allocate_failure;
    }
    node->next = 0;
    node->element = element;
    do_push(linked_blocking_queue, node);
    return_value = 0;
    count = atomic_fetch_add(& (linked_blocking_queue->count), 1);
    if (count + 1 < linked_blocking_queue->capacity)
    {
        value = cnd_signal(& (linked_blocking_queue->condition_push));
        if (value != thrd_success)
        {
            goto condition_signal_failure;
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
allocate_success:
    ;
allocate_failure:
    ;
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_push));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == 0)
    {
        signal_pop(linked_blocking_queue);
    }
    return return_value;
}

void *
linked_blocking_queue_1_pop
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    void * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    intmax_t count = -1;
    while (atomic_load(& (linked_blocking_queue->count)) <= 0)
    {
        value = cnd_wait(& (linked_blocking_queue->condition_pop), & (linked_blocking_queue->mutex_pop));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
    struct linked_blocking_queue_1_node * node = do_pop(linked_blocking_queue);
    return_value = node->element;
    node->element = 0;
    linked_blocking_queue->memory_management.deallocate(& (linked_blocking_queue->memory_management), node);
    node = 0;
    count = atomic_fetch_sub(& (linked_blocking_queue->count), 1);
    if (count > 1)
    {
        value = cnd_signal(& (linked_blocking_queue->condition_pop));
        if (value != thrd_success)
        {
            goto condition_signal_failure;
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == linked_blocking_queue->capacity)
    {
        signal_push(linked_blocking_queue);
    }
    return return_value;
}

void *
linked_blocking_queue_1_try_pop
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    void * return_value = 0;
    intmax_t count = -1;
    if (atomic_load(& (linked_blocking_queue->count)) <= 0)
    {
        return return_value;
    }
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    if (atomic_load(& (linked_blocking_queue->count)) > 0)
    {
        struct linked_blocking_queue_1_node * node = do_pop(linked_blocking_queue);
        return_value = node->element;
        node->element = 0;
        linked_blocking_queue->memory_management.deallocate(& (linked_blocking_queue->memory_management), node);
        node = 0;
        count = atomic_fetch_sub(& (linked_blocking_queue->count), 1);
        if (count > 1)
        {
            value = cnd_signal(& (linked_blocking_queue->condition_pop));
            if (value != thrd_success)
            {
                goto condition_signal_failure;
            }
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == linked_blocking_queue->capacity)
    {
        signal_push(linked_blocking_queue);
    }
    return return_value;
}

void *
linked_blocking_queue_1_timed_pop
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    struct timespec * timespec
)
{
    void * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    intmax_t count = -1;
    while (atomic_load(& (linked_blocking_queue->count)) <= 0)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_queue->condition_pop), & (linked_blocking_queue->mutex_pop), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
    struct linked_blocking_queue_1_node * node = do_pop(linked_blocking_queue);
    return_value = node->element;
    node->element = 0;
    linked_blocking_queue->memory_management.deallocate(& (linked_blocking_queue->memory_management), node);
    node = 0;
    count = atomic_fetch_sub(& (linked_blocking_queue->count), 1);
    if (count > 1)
    {
        value = cnd_signal(& (linked_blocking_queue->condition_pop));
        if (value != thrd_success)
        {
            goto condition_signal_failure;
        }
    }
condition_signal_success:
    ;
condition_signal_failure:
    ;
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_queue->mutex_pop));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    if (count == linked_blocking_queue->capacity)
    {
        signal_push(linked_blocking_queue);
    }
    return return_value;
}

intmax_t
linked_blocking_queue_1_for_each
(
    struct linked_blocking_queue_1 * linked_blocking_queue,
    void (* function)(void *)
)
{
    intmax_t return_value = -1;
    if (function == 0)
    {
        return return_value;
    }
    intmax_t value = fully_lock(linked_blocking_queue);
    if (value != 0)
    {
        goto mutex_lock_failure;
    }
    struct linked_blocking_queue_1_node * node = linked_blocking_queue->head;
    struct linked_blocking_queue_1_node * next = node->next;
    while (next != 0)
    {
        node = next;
        next = next->next;
        function(node->element);
    }
    return_value = 0;
mutex_lock_success:
    value = fully_unlock(linked_blocking_queue);
    if (value != 0)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_queue_1_clear
(
    struct linked_blocking_queue_1 * linked_blocking_queue
)
{
    intmax_t return_value = -1;
    intmax_t value = fully_lock(linked_blocking_queue);
    if (value != 0)
    {
        goto mutex_lock_failure;
    }
    int count = 0;
    struct linked_blocking_queue_1_node * node = linked_blocking_queue->head;
    struct linked_blocking_queue_1_node * next = node->next;
    while (next != 0)
    {
        node = next;
        next = next->next;
        linked_blocking_queue->memory_management.deallocate(& (linked_blocking_queue->memory_management), node);
        ++ count;
    }
    linked_blocking_queue->head->next = 0;
    linked_blocking_queue->tail = linked_blocking_queue->head;
    return_value = 0;
    if (atomic_fetch_sub(& (linked_blocking_queue->count), count) == linked_blocking_queue->capacity)
    {
        signal_push(linked_blocking_queue);
    }
mutex_lock_success:
    value = fully_unlock(linked_blocking_queue);
    if (value != 0)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}
