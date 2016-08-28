//
// Created by Oleg on 4/14/16.
//

#ifndef EVOLUTION_THREADS_H
#define EVOLUTION_THREADS_H

#include <pthread.h>

typedef void* (*thread_routine)(void*);

typedef struct {
    thread_routine routine;
    void* arg;
    void* result;
    int processed;
} task_info;

typedef struct {
    int is_running;
    task_info* task;
} thread_info;


int InitThreads(size_t threads_number);

int DestroyThreads();

int AddTasks(task_info* tasks, size_t tasks_number);

int JoinTasks();


#endif //EVOLUTION_THREADS_H
