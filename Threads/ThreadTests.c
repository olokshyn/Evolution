//
// Created by Oleg on 4/19/16.
//

#include <unistd.h>
#include <stdio.h>

#include "Threads.h"
#include "ThreadTests.h"


int test_routine1(void* arg) {
    int n = 5;
    while (n-- > 0) {
        printf("Test routine %d\n", (int)arg);
        sleep(1);
    }
    return 0;
}

int threads_tests() {
    int status = 0;
    status = InitThreads(2);
    printf("InitThreads returned %d\n", status);

    const task_info tasks[] = {
            {test_routine1, (void*)1},
            {test_routine1, (void*)2},
            {test_routine1, (void*)3},
            {test_routine1, (void*)4},
            {test_routine1, (void*)5}
    };

    status = AddTasks(tasks, sizeof(tasks) / sizeof(task_info));
    printf("InitThreads returned %d\n", status);

    status = JoinTasks();
    printf("JoinTasks returned %d\n", status);

    status = DestroyThreads();
    printf("DestroyTreads returned %d\n", status);

    return status;
}