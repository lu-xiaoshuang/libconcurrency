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

# ifdef __STDC_NO_THREADS__

# include <pthread.h>

# include <errno.h>
# include <stdint.h>
# include <time.h>

# include "threads_wrapper.h"

static
intmax_t
convert_code
(
    intmax_t code
)
{
    switch (code)
    {
        case 0:
            return thrd_success;
        case EBUSY:
            return thrd_busy;
        case ENOMEM:
            return thrd_nomem;
        case ETIMEDOUT:
            return thrd_timedout;
        default:
            return thrd_error;
    }
}

intmax_t
thrd_create
(
    thrd_t * thread,
    thrd_start_t function,
    void * argument
)
{
    /**
     * If the attributes specified by attr are modified later, the thread's attributes shall not be affected.
     *
     * https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_create.html
     */
    pthread_attr_t threadattr;
    intmax_t value = pthread_attr_init(& threadattr);
    value = pthread_create(& (thread->thread_impl), & threadattr, (void * (*)(void *))function, argument);
    pthread_attr_destroy(& threadattr);
    return convert_code(value);
}

intmax_t
thrd_equal
(
    thrd_t thread_l,
    thrd_t thread_r
)
{
    return pthread_equal(thread_l.thread_impl, thread_r.thread_impl);
}

thrd_t
thrd_current
(
    void
)
{
    thrd_t thread;
    thread.thread_impl = pthread_self();
    return thread;
}

intmax_t
thrd_sleep
(
    struct timespec const * duration,
    struct timespec * remaining
)
{
    // TODO
    return -1;
}

void
thrd_yield
(
    void
)
{
    // TODO
}

void
thrd_exit
(
    intmax_t result
)
{
    pthread_exit((void *)((uintmax_t)result));
}

intmax_t
thrd_detach
(
    thrd_t thread
)
{
    intmax_t value = pthread_detach(thread.thread_impl);
    return convert_code(value);
}

intmax_t
thrd_join
(
    thrd_t thread,
    intmax_t * result
)
{
    intmax_t * temporary;
    intmax_t value = pthread_join(thread.thread_impl, (void **)(& temporary));
    if (result != 0)
    {
        * result = (intmax_t)temporary;
    }
    return convert_code(value);
}

intmax_t
mtx_init
(
    mtx_t * mutex,
    intmax_t type
)
{
    /**
     * If the attributes specified by attr are modified later, the mutex's attributes shall not be affected.
     *
     * https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_init.html
     */
    pthread_mutexattr_t mutexattr;
    intmax_t value = pthread_mutexattr_init(& mutexattr);
    switch (type)
    {
        case (mtx_plain):
        case (mtx_timed):
            pthread_mutexattr_settype(& mutexattr, PTHREAD_MUTEX_NORMAL);
            break;
        case (mtx_plain | mtx_recursive):
        case (mtx_timed | mtx_recursive):
            pthread_mutexattr_settype(& mutexattr, PTHREAD_MUTEX_RECURSIVE);
            break;
        default:
            pthread_mutexattr_settype(& mutexattr, PTHREAD_MUTEX_NORMAL);
            break;
    }
    value = pthread_mutex_init(& (mutex->mutex_impl), & mutexattr);
    pthread_mutexattr_destroy(& mutexattr);
    return convert_code(value);
}

intmax_t
mtx_lock
(
    mtx_t * mutex
)
{
    intmax_t value = pthread_mutex_lock(& (mutex->mutex_impl));
    return convert_code(value);
}

intmax_t
mtx_trylock
(
    mtx_t * mutex
)
{
    intmax_t value = pthread_mutex_trylock(& (mutex->mutex_impl));
    return convert_code(value);
}

intmax_t
mtx_timedlock
(
    mtx_t * mutex,
    struct timespec const * timespec
)
{
    // TODO
    // intmax_t value = pthread_mutex_timedlock(& (mutex->mutex_impl), timespec);
    // return convert_code(value);
    return -1;
}

intmax_t
mtx_unlock
(
    mtx_t * mutex
)
{
    intmax_t value = pthread_mutex_unlock(& (mutex->mutex_impl));
    return convert_code(value);
}

void
mtx_destroy
(
    mtx_t * mutex
)
{
    intmax_t value = pthread_mutex_destroy(& (mutex->mutex_impl));
}

intmax_t
cnd_init
(
    cnd_t * condition
)
{
    /**
     * If the attributes specified by attr are modified later, the cond's attributes shall not be affected.
     *
     * https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_init.html
     */
    pthread_condattr_t condattr;
    intmax_t value = pthread_condattr_init(& condattr);
    value = pthread_cond_init(& (condition->cond_impl), & condattr);
    pthread_condattr_destroy(& condattr);
    return convert_code(value);
}

intmax_t
cnd_broadcast
(
    cnd_t * condition
)
{
    intmax_t value = pthread_cond_broadcast(& (condition->cond_impl));
    return convert_code(value);
}

intmax_t
cnd_signal
(
    cnd_t * condition
)
{
    intmax_t value = pthread_cond_signal(& (condition->cond_impl));
    return convert_code(value);
}

intmax_t
cnd_wait
(
    cnd_t * condition,
    mtx_t * mutex
)
{
    intmax_t value = pthread_cond_wait(& (condition->cond_impl), & (mutex->mutex_impl));
    return convert_code(value);
}

intmax_t
cnd_timedwait
(
    cnd_t * condition,
    mtx_t * mutex,
    struct timespec const * timespec
)
{
    intmax_t value = pthread_cond_timedwait(& (condition->cond_impl), & (mutex->mutex_impl), timespec);
    return convert_code(value);
}

void
cnd_destroy
(
    cnd_t * condition
)
{
    intmax_t value = pthread_cond_destroy(& (condition->cond_impl));
}

# endif
