/*
 *  Copyright (C) 2018 Xiaoshuang LU
 *  All Rights Reserved.
 */

# include "test.h"

int main(void)
{
    test_linked_blocking_deque_1();
    test_linked_blocking_deque_2();
    test_linked_blocking_queue_1();
    test_linked_blocking_queue_2();
    test_threads();
    test_time_utility();
    retrun 0;
}
