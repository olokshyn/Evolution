//
// Created by Oleg on 4/14/16.
//

#include <stdlib.h>

#include "Threads.h"

static size_t threads_count = 0;
static pthread_t* threads = NULL;
static thread_info* threads_data = NULL;
static int destroy_threads = 0;

static size_t tasks_count = 0;
static task_info* tasks_data = NULL;

static pthread_mutex_t mutex;
static pthread_cond_t cond_start;
static pthread_cond_t cond_end;


static void* _ThreadRoutine(void* thread_index);

static int _StartTasks(task_info** tasks, size_t* tasks_number);

static int _StartTasksByPointers(task_info*** tasks_pointers,
                                 size_t* tasks_number);

static int _AreThreadsRunning();


int InitThreads(size_t threads_number) {
    if (threads) {
        DestroyThreads();
    }

    threads_count = threads_number;
    threads = (pthread_t*)malloc(sizeof(pthread_t) * threads_count);
    threads_data = (thread_info*)calloc(threads_count, sizeof(thread_info));
    destroy_threads = 0;

    tasks_count = 0;
    tasks_data = NULL;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_start, NULL);
    pthread_cond_init(&cond_end, NULL);

    for (size_t i = 0; i < threads_count; ++i) {
        int status = pthread_create(&threads[i],
                                    NULL,
                                    _ThreadRoutine,
                                    (void*)i);
        if (status) {
            DestroyThreads();
            return status;
        }
    }
    return 0;
}


int DestroyThreads() {
    int status = JoinTasks();
    if (status) {
        return status;
    }

    pthread_mutex_lock(&mutex);
    destroy_threads = 1;
    pthread_cond_broadcast(&cond_start);
    pthread_mutex_unlock(&mutex);

    for (size_t i = 0; i < threads_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    threads_count = 0;
    free(threads);
    threads = NULL;
    free(threads_data);
    threads_data = NULL;

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_start);
    pthread_cond_destroy(&cond_end);
    return 0;
}


int AddTasks(task_info* tasks, size_t tasks_number) {
    int status = 0;
    if (tasks_data) {
        status = JoinTasks();
        if (status) {
            return status;
        }
    }

    pthread_mutex_lock(&mutex);
    tasks_count = tasks_number;
    tasks_data = tasks;
    for (size_t i = 0; i < tasks_count; ++i) {
        tasks_data[i].processed = 0;
    }

    task_info* new_tasks = tasks_data;
    size_t new_tasks_number = tasks_count;

    while ((status = _StartTasks(&new_tasks, &new_tasks_number)) == 0
                && new_tasks_number > 0) {
        pthread_cond_wait(&cond_end, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    return status;
}


int JoinTasks() {
    int status = 0;
    pthread_mutex_lock(&mutex);
    while (_AreThreadsRunning()) {
        pthread_cond_wait(&cond_end, &mutex);
    }

    size_t unprocessed_data_count = 0;
    for (size_t i = 0; i < tasks_count; ++i) {
        unprocessed_data_count += !tasks_data[i].processed;
    }
    if (unprocessed_data_count) {
        task_info** tasks_pointers = (task_info**)calloc(unprocessed_data_count,
                                                         sizeof(task_info*));
        size_t index = 0;
        for (size_t i = 0; i < tasks_count; ++i) {
            if (!tasks_data[i].processed) {
                tasks_pointers[index++] = &tasks_data[i];
            }
        }

        task_info** temp = tasks_pointers;
        while ((status = _StartTasksByPointers(&tasks_pointers,
                                               &unprocessed_data_count)) == 0
                   && unprocessed_data_count > 0) {
            pthread_cond_wait(&cond_end, &mutex);
        }
        while (_AreThreadsRunning()) {
            pthread_cond_wait(&cond_end, &mutex);
        }
        free(temp);
    }

    tasks_count = 0;
    tasks_data = NULL;
    pthread_mutex_unlock(&mutex);
    return status;
}


static void* _ThreadRoutine(void* thread_index) {
    size_t index = (size_t)thread_index;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (!threads_data[index].is_running && !destroy_threads) {
            pthread_cond_wait(&cond_start, &mutex);
        }
        if (destroy_threads) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);


        threads_data[index].task->result =
                threads_data[index].task->routine(threads_data[index].task->arg);

        pthread_mutex_lock(&mutex);
        threads_data[index].task->processed = 1;
        threads_data[index].is_running = 0;
        pthread_cond_broadcast(&cond_end);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}


static int _StartTasks(task_info** tasks, size_t* tasks_number) {
    task_info** tasks_pointers = (task_info**)calloc(*tasks_number,
                                                     sizeof(task_info*));
    for (size_t i = 0; i < *tasks_number; ++i) {
        tasks_pointers[i] = *tasks + i;
    }
    task_info** temp = tasks_pointers;
    size_t temp_size = *tasks_number;
    int status = _StartTasksByPointers(&tasks_pointers, tasks_number);
    if (!tasks_pointers || tasks_pointers >= temp + temp_size) {
        *tasks = NULL;
    }
    else {
        *tasks = *tasks_pointers;
    }
    free(temp);
    return status;
}


static int _StartTasksByPointers(task_info*** tasks_pointers,
                                 size_t* tasks_number) {
    if (!*tasks_pointers) {
        return 0;
    }
    for (size_t i = 0; i < threads_count && *tasks_number > 0; ++i) {
        if (!threads_data[i].is_running) {
            threads_data[i].task = *((*tasks_pointers)++);
            threads_data[i].task->processed = 0;
            threads_data[i].is_running = 1;
            --*tasks_number;
        }
    }
    pthread_cond_broadcast(&cond_start);
    return 0;
}


static int _AreThreadsRunning() {
    for (size_t i = 0; i < threads_count; ++i) {
        if (threads_data[i].is_running) {
            return 1;
        }
    }
    return 0;
}