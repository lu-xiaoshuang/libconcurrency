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

# ifndef _CONCURRENCY_C_LINKED_BLOCKING_DEQUE_1_H_
# define _CONCURRENCY_C_LINKED_BLOCKING_DEQUE_1_H_

# include <stdint.h>
# ifndef __STDC_NO_THREADS__
# include <threads.h>
# endif
# include <time.h>

# include "memory_management.h"
# ifdef __STDC_NO_THREADS__
# include "threads_wrapper.h"
# endif

/**
 * linked_blocking_deque_1 takes care of memory management for each node
 * while users are in charge of memory management for elements.
 */

struct linked_blocking_deque_1_node;

typedef struct linked_blocking_deque_1_node linked_blocking_deque_1_node_t;

struct linked_blocking_deque_1
{
    intmax_t capacity;
 
    intmax_t count;
 
    struct linked_blocking_deque_1_node * head;
 
    struct linked_blocking_deque_1_node * tail;
 
    mtx_t mutex;
 
    cnd_t condition_push;
 
    cnd_t condition_pop;

    /**
     * for struct linked_blocking_deque_1_node only
     * linked_blocking_deque_1_node->element is excluded
     */
    struct memory_management memory_management;
};

typedef struct linked_blocking_deque_1 linked_blocking_deque_1_t;

/**
 * initialize a linked_blocking_deque_1
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_init
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    intmax_t capacity,
    struct memory_management * memory_management
);

/**
 * destroy a linked_blocking_deque_1
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_destroy
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

extern
intmax_t
linked_blocking_deque_1_capacity
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

extern
intmax_t
linked_blocking_deque_1_size
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

extern
intmax_t
linked_blocking_deque_1_remaining_capacity
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

extern
intmax_t
linked_blocking_deque_1_empty
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

/**
 * push front
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_push_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element
);

/**
 * push front
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_try_push_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element
);

/**
 * push front
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_timed_push_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element,
    struct timespec * timespec
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_push_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_try_push_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element
);

/**
 * push back
 *
 * Upon successful completion, a value of zero is returned.
 * Otherwise, a non-zero number is returned.
 */
extern
intmax_t
linked_blocking_deque_1_timed_push_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    void * element,
    struct timespec * timespec
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_pop_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_try_pop_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

/**
 * pop front
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_timed_pop_front
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    struct timespec * timespec
);

/**
 * pop back
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_pop_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

/**
 * pop back
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_try_pop_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque
);

/**
 * pop back
 *
 * Upon successful completion, a value of non-zero is returned.
 * Otherwise, a zero number is returned.
 */
extern
void *
linked_blocking_deque_1_timed_pop_back
(
    struct linked_blocking_deque_1 * linked_blocking_deque,
    struct timespec * timespec
);

# endif
