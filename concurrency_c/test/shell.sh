#!/bin/sh

CC="gcc"
CFLAGS="-std=c17 -Wall -pedantic"

rm -rf *.o test
${CC} ${CFLAGS} -c ../code/concurrent_hash_map.c -o concurrent_hash_map.o
${CC} ${CFLAGS} -c ../code/concurrent_linked_deque_1.c -o concurrent_linked_deque_1.o
${CC} ${CFLAGS} -c ../code/concurrent_linked_deque_2.c -o concurrent_linked_deque_2.o
${CC} ${CFLAGS} -c ../code/concurrent_linked_queue_1.c -o concurrent_linked_queue_1.o
${CC} ${CFLAGS} -c ../code/concurrent_linked_queue_2.c -o concurrent_linked_queue_2.o
${CC} ${CFLAGS} -c ../code/future.c -o future.o
${CC} ${CFLAGS} -c ../code/linked_blocking_deque_1.c -o linked_blocking_deque_1.o
${CC} ${CFLAGS} -c ../code/linked_blocking_deque_2.c -o linked_blocking_deque_2.o
${CC} ${CFLAGS} -c ../code/linked_blocking_queue_1.c -o linked_blocking_queue_1.o
${CC} ${CFLAGS} -c ../code/linked_blocking_queue_2.c -o linked_blocking_queue_2.o
${CC} ${CFLAGS} -c ../code/memory_management.c -o memory_management.o
${CC} ${CFLAGS} -c ../code/thread_pool_executor.c -o thread_pool_executor.o
${CC} ${CFLAGS} -c ../code/threads_wrapper.c -o threads_wrapper.o
${CC} ${CFLAGS} -c ../code/time_utility.c -o time_utility.o
${CC} ${CFLAGS} -c test_concurrent_hash_map.c -o test_concurrent_hash_map.o
${CC} ${CFLAGS} -c test_concurrent_linked_deque_1.c -o test_concurrent_linked_deque_1.o
${CC} ${CFLAGS} -c test_concurrent_linked_deque_2.c -o test_concurrent_linked_deque_2.o
${CC} ${CFLAGS} -c test_concurrent_linked_queue_1.c -o test_concurrent_linked_queue_1.o
${CC} ${CFLAGS} -c test_concurrent_linked_queue_2.c -o test_concurrent_linked_queue_2.o
${CC} ${CFLAGS} -c test_future.c -o test_future.o
${CC} ${CFLAGS} -c test_linked_blocking_deque_1.c -o test_linked_blocking_deque_1.o
${CC} ${CFLAGS} -c test_linked_blocking_deque_2.c -o test_linked_blocking_deque_2.o
${CC} ${CFLAGS} -c test_linked_blocking_queue_1.c -o test_linked_blocking_queue_1.o
${CC} ${CFLAGS} -c test_linked_blocking_queue_2.c -o test_linked_blocking_queue_2.o
${CC} ${CFLAGS} -c test_thread_pool_executor.c -o test_thread_pool_executor.o
${CC} ${CFLAGS} -c test_threads_wrapper.c -o test_threads_wrapper.o
${CC} ${CFLAGS} -c test_time_utility.c -o test_time_utility.o
${CC} ${CFLAGS} -c test.c -o test.o
${CC} \
    concurrent_hash_map.o \
    concurrent_linked_deque_1.o \
    concurrent_linked_deque_2.o \
    concurrent_linked_queue_1.o \
    concurrent_linked_queue_2.o \
    future.o \
    linked_blocking_deque_1.o \
    linked_blocking_deque_2.o \
    linked_blocking_queue_1.o \
    linked_blocking_queue_2.o \
    memory_management.o \
    thread_pool_executor.o \
    threads_wrapper.o \
    time_utility.o \
    test_concurrent_hash_map.o \
    test_concurrent_linked_deque_1.o \
    test_concurrent_linked_deque_2.o \
    test_concurrent_linked_queue_1.o \
    test_concurrent_linked_queue_2.o \
    test_future.o \
    test_linked_blocking_deque_1.o \
    test_linked_blocking_deque_2.o \
    test_linked_blocking_queue_1.o \
    test_linked_blocking_queue_2.o \
    test_thread_pool_executor.o \
    test_threads_wrapper.o \
    test_time_utility.o \
    test.o \
    -lpthread \
    -o test
./test
