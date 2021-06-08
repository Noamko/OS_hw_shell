#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osqueue.h"

typedef struct thread_instance {
    pthread_t thread;
    int isRunning;

} _Thread;

typedef struct thread_pool {
    int numOfThreads;
    pthread_t main_thread;
    OSQueue* queue;
    _Thread** threads;

} ThreadPool;

typedef struct task {
    void (*computeFunc)(void*);
    void* param;

} Task;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc)(void*), void* param);

#endif
