//Noam Koren 308192871
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osqueue.h"

typedef struct thread_pool {
    int numOfThreads;
    int isDestroyed;
    int exitloop;
    pthread_mutex_t flock;
    pthread_mutex_t slock;
    pthread_cond_t cond;
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
