//
// Created by Oleg on 4/14/16.
//

#ifndef WISHART_THREADS_H
#define WISHART_THREADS_H

#include <pthread.h>

typedef int (*thread_routine)(void*);

typedef struct {
    thread_routine routine;
    void* arg;
    int processed;
} task_info;

typedef struct {
    int is_running;
    task_info* task;
} thread_info;


int InitThreads(size_t threads_number);

int DestroyThreads();

int AddTasks(const task_info* tasks, size_t tasks_number);

int JoinTasks();


#endif //WISHART_THREADS_H
