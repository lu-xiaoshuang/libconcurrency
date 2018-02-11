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

# include <stdio.h>
# include <stdlib.h>

# include "test.h"

intmax_t
assert_false
(
    intmax_t value,
    char const * const file,
    char const * const line,
    char const * const message
)
{
    if (! value)
    {
        return 0;
    }
    printf("expected: true, actual: %d, file: %s, line: %d, message: %s\n", value, file, line, message);
    exit(EXIT_FAILURE);
    return 0;
}

intmax_t
assert_true
(
    intmax_t value,
    char const * const file,
    char const * const line,
    char const * const message
)
{
    if (value)
    {
        return 0;
    }
    printf("expected: true, actual: %d, file: %s, line: %d, message: %s\n", value, file, line, message);
    exit(EXIT_FAILURE);
    return 0;
}

/**
 * https://github.com/openjdk/jdk/tree/master/test/jdk/java/util/concurrent/tck
 */

int main(void)
{
    test_concurrent_hash_map();
    test_concurrent_linked_deque_1();
    test_concurrent_linked_deque_2();
    test_concurrent_linked_queue_1();
    test_concurrent_linked_queue_2();
    test_future();
    test_linked_blocking_deque_1();
    test_linked_blocking_deque_2();
    test_linked_blocking_queue_1();
    test_linked_blocking_queue_2();
    test_thread_pool_executor();
    test_threads_wrapper();
    test_time_utility();
    return 0;
}
