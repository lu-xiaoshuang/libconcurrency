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

# include <stdint.h>
# ifndef __STDC_NO_THREADS__
# include <threads.h>
# endif
# include <time.h>

# include "linked_blocking_deque_2.h"
# ifdef __STDC_NO_THREADS__
# include "threads_wrapper.h"
# endif
# include "time_utility.h"

/**
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
static
intmax_t
do_push_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    if (linked_blocking_deque->count >= linked_blocking_deque->capacity)
    {
        return -1;
    }
    struct linked_blocking_deque_2_node * first = linked_blocking_deque->head;
    linked_blocking_deque_node->next = first;
    linked_blocking_deque->head = linked_blocking_deque_node;
    if (linked_blocking_deque->tail == 0)
    {
        linked_blocking_deque->tail = linked_blocking_deque_node;
    }
    else
    {
        first->prev = linked_blocking_deque_node;
    }
    ++ (linked_blocking_deque->count);
    cnd_signal(& (linked_blocking_deque->condition_pop));
    return 0;
}

/**
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
static
intmax_t
do_push_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    if (linked_blocking_deque->count >= linked_blocking_deque->capacity)
    {
        return -1;
    }
    struct linked_blocking_deque_2_node * last = linked_blocking_deque->tail;
    linked_blocking_deque_node->prev = last;
    linked_blocking_deque->tail = linked_blocking_deque_node;
    if (linked_blocking_deque->head == 0)
    {
        linked_blocking_deque->head = linked_blocking_deque_node;
    }
    else
    {
        last->next = linked_blocking_deque_node;
    }
    ++ (linked_blocking_deque->count);
    cnd_signal(& (linked_blocking_deque->condition_pop));
    return 0;
}

/**
 * Upon successful completion, a value of non-null is returned.
 * Otherwise, a null value is returned.
 */
static
struct linked_blocking_deque_2_node *
do_pop_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * first = linked_blocking_deque->head;
    if (first == 0)
    {
        return 0;
    }
    struct linked_blocking_deque_2_node * node = first->next;
    first->prev = 0;
    first->next = 0;
    linked_blocking_deque->head = node;
    if (node == 0)
    {
        linked_blocking_deque->tail = 0;
    }
    else
    {
        node->prev = 0;
    }
    -- (linked_blocking_deque->count);
    cnd_signal(& (linked_blocking_deque->condition_push));
    return first;
}

/**
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero value is returned.
 */
static
struct linked_blocking_deque_2_node *
do_pop_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * last = linked_blocking_deque->tail;
    if (last == 0)
    {
        return 0;
    }
    struct linked_blocking_deque_2_node * node = last->prev;
    last->prev = 0;
    last->next = 0;
    linked_blocking_deque->tail = node;
    if (node == 0)
    {
        linked_blocking_deque->head = 0;
    }
    else
    {
        node->next = 0;
    }
    -- (linked_blocking_deque->count);
    cnd_signal(& (linked_blocking_deque->condition_push));
    return last;
}

intmax_t
linked_blocking_deque_2_init
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    intmax_t capacity
)
{
    if (linked_blocking_deque == 0)
    {
        return -1;
    }
    linked_blocking_deque->capacity = capacity;
    linked_blocking_deque->count = 0;
    linked_blocking_deque->head = 0;
    linked_blocking_deque->tail = 0;
    intmax_t value = 0;
    mtx_init(& (linked_blocking_deque->mutex), (mtx_timed | mtx_recursive));
    cnd_init(& (linked_blocking_deque->condition_push));
    cnd_init(& (linked_blocking_deque->condition_pop));
    return 0;
}

intmax_t
linked_blocking_deque_2_destroy
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    cnd_destroy(& (linked_blocking_deque->condition_pop));
    cnd_destroy(& (linked_blocking_deque->condition_push));
    mtx_destroy(& (linked_blocking_deque->mutex));
    return 0;
}

intmax_t
linked_blocking_deque_2_capacity
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    return linked_blocking_deque->capacity;
}

intmax_t
linked_blocking_deque_2_size
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = linked_blocking_deque->count;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_remaining_capacity
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = linked_blocking_deque->capacity - linked_blocking_deque->count;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_empty
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    intmax_t size = linked_blocking_deque_2_size(linked_blocking_deque);
    if (size < 0)
    {
        return -1;
    }
    else if (size == 0)
    {
        return 1;
    } else
    {
        return 0;
    }
}

intmax_t
linked_blocking_deque_2_push_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    while (do_push_front(linked_blocking_deque, linked_blocking_deque_node) != 0)
    {
        value = cnd_wait(& (linked_blocking_deque->condition_push), & (linked_blocking_deque->mutex));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
    return_value = 0;
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_try_push_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = do_push_front(linked_blocking_deque, linked_blocking_deque_node);
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_timed_push_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node,
    struct timespec * timespec
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    while (do_push_front(linked_blocking_deque, linked_blocking_deque_node) != 0)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_deque->condition_push), & (linked_blocking_deque->mutex), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
    return_value = 0;
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_push_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    while (do_push_back(linked_blocking_deque, linked_blocking_deque_node) != 0)
    {
        value = cnd_wait(& (linked_blocking_deque->condition_push), & (linked_blocking_deque->mutex));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
    return_value = 0;
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_try_push_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = do_push_back(linked_blocking_deque, linked_blocking_deque_node);
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

intmax_t
linked_blocking_deque_2_timed_push_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct linked_blocking_deque_2_node * linked_blocking_deque_node,
    struct timespec * timespec
)
{
    intmax_t return_value = -1;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    while (do_push_back(linked_blocking_deque, linked_blocking_deque_node) != 0)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_deque->condition_push), & (linked_blocking_deque->mutex), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
    return_value = 0;
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_pop_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    while ((return_value = do_pop_front(linked_blocking_deque)) == 0)
    {
        value = cnd_wait(& (linked_blocking_deque->condition_pop), & (linked_blocking_deque->mutex));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_try_pop_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = do_pop_front(linked_blocking_deque);
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_timed_pop_front
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct timespec * timespec
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    while ((return_value = do_pop_front(linked_blocking_deque)) == 0)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_deque->condition_pop), & (linked_blocking_deque->mutex), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_pop_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    while ((return_value = do_pop_back(linked_blocking_deque)) == 0)
    {
        value = cnd_wait(& (linked_blocking_deque->condition_pop), & (linked_blocking_deque->mutex));
        if (value != thrd_success)
        {
            goto condition_wait_failure;
        }
    }
condition_wait_success:
    ;
condition_wait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_try_pop_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    return_value = do_pop_back(linked_blocking_deque);
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}

struct linked_blocking_deque_2_node *
linked_blocking_deque_2_timed_pop_back
(
    struct linked_blocking_deque_2 * linked_blocking_deque,
    struct timespec * timespec
)
{
    struct linked_blocking_deque_2_node * return_value = 0;
    intmax_t value = mtx_lock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
    struct timespec timestamp;
    struct timespec deadline;
    timespec_get(& timestamp, TIME_UTC);
    timespec_add(& timestamp, timespec, & deadline);
    while ((return_value = do_pop_back(linked_blocking_deque)) == 0)
    {
        struct timespec timeout;
        timespec_get(& timestamp, TIME_UTC);
        timespec_sub(& deadline, & timestamp, & timeout);
        if (timeout.tv_sec < 0)
        {
            goto mutex_lock_success;
        }
        value = cnd_timedwait(& (linked_blocking_deque->condition_pop), & (linked_blocking_deque->mutex), & timeout);
        if (value != thrd_success)
        {
            goto condition_timedwait_failure;
        }
    }
condition_timedwait_success:
    ;
condition_timedwait_failure:
    ;
mutex_lock_success:
    value = mtx_unlock(& (linked_blocking_deque->mutex));
    if (value != thrd_success)
    {
        goto mutex_lock_failure;
    }
mutex_lock_failure:
    return return_value;
}
