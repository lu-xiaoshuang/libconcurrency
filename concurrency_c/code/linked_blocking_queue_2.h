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

# ifndef _CONCURRENCY_C_LINKED_BLOCKING_QUEUE_2_H_
# define _CONCURRENCY_C_LINKED_BLOCKING_QUEUE_2_H_

# include <stdatomic.h>
# ifndef __STDC_NO_THREADS__
# include <threads.h>
# endif
# include <time.h>

# ifdef __STDC_NO_THREADS__
# include "threads_wrapper.h"
# endif

/**
 * Users shall take care of memory management for each node.
 */

struct linked_blocking_queue_2_node;

struct linked_blocking_queue_2_node
{
    struct linked_blocking_queue_2_node * next;
};

typedef struct linked_blocking_queue_1_node linked_blocking_queie_1_node_t;

struct linked_blocking_queue_2
{
    intmax_t capacity;
 
    atomic_intmax_t count;
 
    struct linked_blocking_queue_2_node node;
 
    struct linked_blocking_queue_2_node * head;
 
    struct linked_blocking_queue_2_node * tail;
 
    mtx_t mutex_push;
 
    cnd_t condition_push;
 
    mtx_t mutex_pop;
 
    cnd_t condition_pop;

    /**
     * lock for concurrent read and write when there is only one node in the queue.
     * use spin lock?
     */
    mtx_t mutex;
};

typedef struct linked_blocking_queue_2 linked_blocking_queue_2_t;

/**
 * initialize a linked_blocking_queue_2
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_queue_2_init
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    intmax_t capacity
);

/**
 * destroy a linked_blocking_queue_2
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_queue_2_destroy
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

extern
intmax_t
linked_blocking_queue_2_capacity
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

extern
intmax_t
linked_blocking_queue_2_size
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

extern
intmax_t
linked_blocking_queue_2_remaining_capacity
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

extern
intmax_t
linked_blocking_queue_2_empty
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_queue_2_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_queue_2_try_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_queue_2_timed_push
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct linked_blocking_queue_2_node * linked_blocking_queue_node,
    struct timespec * timespec
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
struct linked_blocking_queue_2_node *
linked_blocking_queue_2_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
struct linked_blocking_queue_2_node *
linked_blocking_queue_2_try_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
struct linked_blocking_queue_2_node *
linked_blocking_queue_2_timed_pop
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    struct timespec * timespec
);

extern
intmax_t
linked_blocking_queue_2_for_each
(
    struct linked_blocking_queue_2 * linked_blocking_queue,
    void (* function)(struct linked_blocking_queue_2_node *)
);

extern
intmax_t
linked_blocking_queue_2_clear
(
    struct linked_blocking_queue_2 * linked_blocking_queue
);

# endif
