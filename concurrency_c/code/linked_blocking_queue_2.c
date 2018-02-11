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

# include "linked_blocking_queue_2.h"
# ifdef __STDC_NO_THREADS__
# include "threads_wrapper.h"
# endif
# include "time_utility.h"

static
void
signal_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue
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
    struct linked_blocking_queue_2 * linked_blocking_queue
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
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node
)
{
    intmax_t count = atomic_load(& (linked_blocking_queue->count));
    if (count != 1)
    {
        linked_blocking_queue->tail->next = linked_blocking_queue_node;
        linked_blocking_queue->tail = linked_blocking_queue_node;
        linked_blocking_queue_node->next = 0;
    }
    else
    {
        mtx_lock(& (linked_blocking_queue->mutex));
        linked_blocking_queue->tail->next = linked_blocking_queue_node;
        linked_blocking_queue->tail = linked_blocking_queue_node;
        linked_blocking_queue_node->next = 0;
        mtx_unlock(& (linked_blocking_queue->mutex));
    }
}

static
struct linked_blocking_queue_2_node *
do_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    struct linked_blocking_queue_2_node * node = 0;
    intmax_t count = atomic_load(& (linked_blocking_queue->count));
    if (count != 1)
    {
        node = linked_blocking_queue->head->next;
        linked_blocking_queue->head->next = node->next;
        node->next = 0;
    }
    else
    {
        mtx_lock(& (linked_blocking_queue->mutex));
        node = linked_blocking_queue->head->next;
        linked_blocking_queue->head->next = 0;
        linked_blocking_queue->head = & (linked_blocking_queue->node);
        linked_blocking_queue->tail = & (linked_blocking_queue->node);
        node->next = 0;
        mtx_unlock(& (linked_blocking_queue->mutex));
    }
    return node;
}

static
intmax_t
fully_lock
(
    struct linked_blocking_queue_2 * linked_blocking_queue
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
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    mtx_unlock(& (linked_blocking_queue->mutex_pop));
    mtx_unlock(& (linked_blocking_queue->mutex_push));
    return 0;
}

intmax_t
linked_blocking_queue_2_init
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    intmax_t capacity
)
{
    if (linked_blocking_queue == 0)
    {
        return -1;
    }
    linked_blocking_queue->capacity = capacity;
    intmax_t value = 0;
    atomic_init(& (linked_blocking_queue->count), 0);
    linked_blocking_queue->node.next = 0;
    linked_blocking_queue->head = & (linked_blocking_queue->node);
    linked_blocking_queue->tail = & (linked_blocking_queue->node);
    mtx_init(& (linked_blocking_queue->mutex_push), (mtx_timed | mtx_recursive));
    cnd_init(& (linked_blocking_queue->condition_push));
    mtx_init(& (linked_blocking_queue->mutex_pop), (mtx_timed | mtx_recursive));
    cnd_init(& (linked_blocking_queue->condition_pop));
    mtx_init(& (linked_blocking_queue->mutex), (mtx_timed | mtx_recursive));
    return 0;
}

intmax_t
linked_blocking_queue_2_destroy
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    mtx_destroy(& (linked_blocking_queue->mutex));
    cnd_destroy(& (linked_blocking_queue->condition_pop));
    mtx_destroy(& (linked_blocking_queue->mutex_pop));
    cnd_destroy(& (linked_blocking_queue->condition_push));
    mtx_destroy(& (linked_blocking_queue->mutex_push));
    return 0;
}

intmax_t
linked_blocking_queue_2_capacity
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    return linked_blocking_queue->capacity;
}

intmax_t
linked_blocking_queue_2_size
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    return atomic_load(& (linked_blocking_queue->count));
}

intmax_t
linked_blocking_queue_2_remaining_capacity
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    return linked_blocking_queue->capacity - atomic_load(& (linked_blocking_queue->count));
}

intmax_t
linked_blocking_queue_2_empty
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    return atomic_load(& (linked_blocking_queue->count)) == 0;
}

intmax_t
linked_blocking_queue_2_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node
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
    do_push(linked_blocking_queue, linked_blocking_queue_node);
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
linked_blocking_queue_2_try_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node
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
        do_push(linked_blocking_queue, linked_blocking_queue_node);
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
linked_blocking_queue_2_timed_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node,
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
    do_push(linked_blocking_queue, linked_blocking_queue_node);
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

struct linked_blocking_queue_2_node *
linked_blocking_queue_2_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    struct linked_blocking_queue_2_node * return_value = 0;
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
    return_value = do_pop(linked_blocking_queue);
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

struct linked_blocking_queue_2_node *
linked_blocking_queue_2_try_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    struct linked_blocking_queue_2_node * return_value = 0;
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
        return_value = do_pop(linked_blocking_queue);
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

struct linked_blocking_queue_2_node *
linked_blocking_queue_2_timed_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct timespec * timespec
)
{
    struct linked_blocking_queue_2_node * return_value = 0;
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
    return_value = do_pop(linked_blocking_queue);
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
linked_blocking_queue_2_for_each
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    void (* function)(struct linked_blocking_queue_2_node *)
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
    struct linked_blocking_queue_2_node * node = linked_blocking_queue->head;
    struct linked_blocking_queue_2_node * next = node->next;
    while (next != 0)
    {
        node = next;
        next = next->next;
        function(node);
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
linked_blocking_queue_2_clear
(
    struct linked_blocking_queue_2 * linked_blocking_queue
)
{
    intmax_t return_value = -1;
    intmax_t value = fully_lock(linked_blocking_queue);
    if (value != 0)
    {
        goto mutex_lock_failure;
    }
    int count = 0;
    struct linked_blocking_queue_2_node * node = linked_blocking_queue->head;
    struct linked_blocking_queue_2_node * next = node->next;
    while (next != 0)
    {
        node = next;
        next = next->next;
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
