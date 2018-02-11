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
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>

# include "../code/linked_blocking_deque_1.h"
# include "../code/memory_management.h"

static
intmax_t
test1
(
    void
)
{
    intmax_t n = 1000;
    intmax_t * array = malloc(sizeof(intmax_t) * n * 2);
    for (intmax_t i = 0; i < n * 2; ++ n)
    {
        array[i] = i;
    }
    struct linked_blocking_deque_1 deque;
    linked_blocking_deque_1_init(& deque, n * 2, & memory_management_default);
    for (intmax_t i = 0; i < n; ++ i)
    {
        linked_blocking_deque_1_push_front(& deque, array + i);
    }
    for (intmax_t i = 0; i < n; ++ i)
    {
        linked_blocking_deque_1_push_back(& deque, array + n + i);
    }
    for (intmax_t i = 1; i <= n; ++ i)
    {
        intmax_t * item = (intmax_t *)linked_blocking_deque_1_pop_front(& deque);
        printf("%03ld ", * item);
        if (i % 20 == 0)
        {
            printf("\n");
        }
    }
    for (intmax_t i = 1; i <= n; ++ i)
    {
        intmax_t * item = (intmax_t *)linked_blocking_deque_1_pop_back(& deque);
        printf("%03ld ", * item);
        if (i % 20 == 0)
        {
            printf("\n");
        }
    }
    linked_blocking_deque_1_destroy(& deque);
    free(array);
    return 0;
}

struct object
{
    struct linked_blocking_deque_1 * deque;
    atomic_intmax_t * count;
};

static
intmax_t
push_front_worker
(
    void * argument
)
{
    struct object * object = (struct object *)argument;
    for (intmax_t i = 0; atomic_fetch_sub(object->count, 1) > 0; ++ i)
    {
        linked_blocking_deque_1_push_front(object->deque, (void *)i);
        if ((i & ((1 << 12) - 1)) == 0)
        {
            printf("thread %ld: push front %ld\n", thrd_current(), i);
        }
    }
    return 0;
}

static
intmax_t
push_back_worker
(
    void * argument
)
{
    struct object * object = (struct object *)argument;
    for (intmax_t i = 0; atomic_fetch_sub(object->count, 1) > 0; ++ i)
    {
        linked_blocking_deque_1_push_back(object->deque, (void *)i);
        if ((i & ((1 << 12) - 1)) == 0)
        {
            printf("thread %ld: push back %ld\n", thrd_current(), i);
        }
    }
    return 0;
}

static
intmax_t
pop_front_worker
(
    void * argument
)
{
    struct object * object = (struct object *)argument;
    for (intmax_t i = 0; atomic_fetch_sub(object->count, 1) > 0; ++ i)
    {
        void * element = linked_blocking_deque_1_pop_front(object->deque);
        if ((i & ((1 << 12) - 1)) == 0)
        {
            printf("thread %ld: pop front %ld\n", thrd_current(), element);
        }
    }
    return 0;
}

static
intmax_t
pop_back_worker
(
    void * argument
)
{
    struct object * object = (struct object *)argument;
    for (intmax_t i = 0; atomic_fetch_sub(object->count, 1) > 0; ++ i)
    {
        void * element = linked_blocking_deque_1_pop_back(object->deque);
        if ((i & ((1 << 12) - 1)) == 0)
        {
            printf("thread %ld: pop back %ld\n", thrd_current(), element);
        }
    }
    return 0;
}

static
intmax_t
test2
(
    void
)
{
    intmax_t element_count = 1000000;
    intmax_t thread_count = 5;

    atomic_intmax_t push_count;
    atomic_init(& push_count, element_count);

    atomic_intmax_t pop_count;
    atomic_init(& pop_count, element_count);

    struct linked_blocking_deque_1 deque;
    linked_blocking_deque_1_init(& deque, 1000, & memory_management_default);

    struct object push_object;
    push_object.deque = & deque;
    push_object.count = & push_count;

    struct object pop_object;
    pop_object.deque = & deque;
    pop_object.count = & pop_count;

    thrd_t * push_front_thread_array = malloc(sizeof(thrd_t) * thread_count);
    thrd_t * push_back_thread_array = malloc(sizeof(thrd_t) * thread_count);
    thrd_t * pop_front_thread_array = malloc(sizeof(thrd_t) * thread_count);
    thrd_t * pop_back_thread_array = malloc(sizeof(thrd_t) * thread_count);
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_create(push_front_thread_array + i, (thrd_start_t)push_front_worker, & push_object);
        thrd_create(push_back_thread_array + i, (thrd_start_t)push_back_worker, & push_object);
        thrd_create(pop_front_thread_array + i, (thrd_start_t)pop_front_worker, & pop_object);
        thrd_create(pop_back_thread_array + i, (thrd_start_t)pop_back_worker, & pop_object);
    }
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_join(push_front_thread_array[i], 0);
    }
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_join(push_back_thread_array[i], 0);
    }
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_join(pop_front_thread_array[i], 0);
    }
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_join(pop_back_thread_array[i], 0);
    }
    free(push_front_thread_array);
    free(push_back_thread_array);
    free(pop_front_thread_array);
    free(pop_back_thread_array);
    if (linked_blocking_deque_1_size(& deque) != 0)
    {
        printf("test2 failed.\n");
    }
    else
    {
        printf("test2 succeeded.\n");
    }
    linked_blocking_deque_1_destroy(& deque);
    return 0;
}

intmax_t
test_linked_blocking_deque_1()
{
    printf("test_linked_blocking_deque_1 prologue\n");
    test1();
    test2();
    printf("test_linked_blocking_deque_1 epilogue\n");
    return 0;
}
