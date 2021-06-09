#ifndef _thread_pool_item_POOL__
#define _thread_pool_item_POOL__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osqueue.h"

typedef struct thread_pool {
    int numOfThreads;
    int isDestroyed;
    pthread_mutex_t lock;
    OSQueue* queue;
    pthread_t* threads;

} ThreadPool;

typedef struct task {
    void (*computeFunc)(void*);
    void* param;

} Task;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc)(void*), void* param);

#endif
