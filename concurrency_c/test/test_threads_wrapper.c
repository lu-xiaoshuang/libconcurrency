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
# ifndef __STDC_NO_THREADS__
# include <threads.h>
# endif
# include <time.h>

# ifdef __STDC_NO_THREADS__
# include "../code/threads_wrapper.h"
# endif

static
intmax_t
worker
(
    void * argument
)
{
    printf("thread %ld:\n", thrd_current());
    return 0;
}

static
intmax_t
test
(
    void
)
{
    intmax_t thread_count = 5;
    thrd_t * thread_array = malloc(sizeof(thrd_t) * thread_count);
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_create(thread_array + i, (thrd_start_t)worker, 0);
    }
    for (intmax_t i = 0; i < thread_count; ++ i)
    {
        thrd_join(thread_array[i], 0);
    }
    free(thread_array);
    return 0;
}

intmax_t
test_threads_wrapper()
{
    printf("test_threads_wrapper prologue\n");
    test();
    printf("test_threads_wrapper epilogue\n");
}
