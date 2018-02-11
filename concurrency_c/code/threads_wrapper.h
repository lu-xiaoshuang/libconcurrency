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

# ifndef _CONCURRENCY_C_THREADS_WRAPPER_H_
# define _CONCURRENCY_C_THREADS_WRAPPER_H_

# ifdef __STDC_NO_THREADS__

# include <pthread.h>

# include <stdint.h>
# include <time.h>

enum __thread_code_impl__
{
    thrd_success = 0,
    thrd_busy = 1,
    thrd_error = 2,
    thrd_nomem = 3,
    thrd_timedout = 4
};

struct __thread_impl__
{
    pthread_t thread_impl;
};

typedef struct __thread_impl__ thrd_t;

typedef intmax_t (* thrd_start_t)(void *);

enum __mutex_type_impl__
{
    mtx_plain = 0,
    mtx_recursive = 1,
    mtx_timed = 2
};

struct __mutex_impl__
{
    pthread_mutex_t mutex_impl;
};

typedef struct __mutex_impl__ mtx_t;

struct __condition_impl__
{
    pthread_cond_t cond_impl;
};

typedef struct __condition_impl__ cnd_t;

extern
intmax_t
thrd_create
(
    thrd_t * thread,
    thrd_start_t function,
    void * argument
);

extern
intmax_t
thrd_equal
(
    thrd_t thread_l,
    thrd_t thread_r
);

extern
thrd_t
thrd_current
(
    void
);

extern
intmax_t
thrd_sleep
(
    struct timespec const * duration,
    struct timespec * remaining
);

extern
void
thrd_yield
(
    void
);

extern
void
thrd_exit
(
    intmax_t result
);

extern
intmax_t
thrd_detach
(
    thrd_t thread
);

extern
intmax_t
thrd_join
(
    thrd_t thread,
    intmax_t * result
);

extern
intmax_t
mtx_init
(
    mtx_t * mutex,
    intmax_t type
);

extern
intmax_t
mtx_lock
(
    mtx_t * mutex
);

extern
intmax_t
mtx_trylock
(
    mtx_t * mutex
);

extern
intmax_t
mtx_timedlock
(
    mtx_t * mutex,
    struct timespec const * timespec
);

extern
intmax_t
mtx_unlock
(
    mtx_t * mutex
);

extern
void
mtx_destroy
(
    mtx_t * mutex
);

extern
intmax_t
cnd_init
(
    cnd_t * condition
);

extern
intmax_t
cnd_broadcast
(
    cnd_t * condition
);

extern
intmax_t
cnd_signal
(
    cnd_t * condition
);

extern
intmax_t
cnd_wait
(
    cnd_t * condition,
    mtx_t * mutex
);

extern
intmax_t
cnd_timedwait
(
    cnd_t * condition,
    mtx_t * mutex,
    struct timespec const * timespec
);

extern
void
cnd_destroy
(
    cnd_t * condition
);

# endif

# endif
