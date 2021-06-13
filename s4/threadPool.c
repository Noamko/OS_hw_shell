#include "threadPool.h"

void threadLoop(void *t) {
    ThreadPool *pool = (ThreadPool *)t;
    while (pool->running) {
        Task *task;
        int tookTask = 0;
        pthread_mutex_lock(&pool->lock);
        if (osIsQueueEmpty(pool->queue) != 1) {
            task = (Task *)osDequeue(pool->queue);
            if (task == NULL) {
                perror("Task error");
            } else
                tookTask = 1;
        } else {
            //pthread_cond_wait(&pool->cond, &pool->lock);
        }
        pthread_mutex_unlock(&pool->lock);
        if (tookTask) {  // if the task was enqeued run it
            task->computeFunc(task->param);
            free(task);  // free the task pointer
        }
    }
}

ThreadPool *tpCreate(int numOfThreads) {
    if (numOfThreads <= 0) {
        perror("invalid thread count");
        return NULL;
    }
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    if (pool == NULL) {
        perror("failed to create thread pool (malloc failed)");
    }
    pool->queue = osCreateQueue();
    pool->threads = malloc(sizeof(pthread_t) * numOfThreads);
    if (pool->threads == NULL) {
        perror("Error in system call");
    }
    pool->numOfThreads = numOfThreads;
    pool->isDestroyed = 0;
    pool->running = 1;
    int i;
    for (i = 0; i < numOfThreads; i++) {
        if (pthread_create(&pool->threads[i], NULL, (void *)threadLoop, pool)) {
            perror("Error in system call");
            exit(-1);
        }
    }
    return pool;
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    threadPool->isDestroyed = 1;
    if (shouldWaitForTasks) {
        while (osIsQueueEmpty(threadPool->queue) != 1) {
            //do nothing wait for threads to finish the queue.
        }
    }
    threadPool->running = 0;
    int i;
    for (i = 0; i < threadPool->numOfThreads; i++) {
        //pthread_cond_signal(&threadPool->cond);
        pthread_join(threadPool->threads[i], NULL);
    }
    osDestroyQueue(threadPool->queue);
    free(threadPool->threads);
    free(threadPool);
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    if (threadPool->isDestroyed) {
        return -1;
    }
    pthread_mutex_lock(&threadPool->lock);
    Task *task = malloc(sizeof(Task));
    if (task == NULL) {
        perror("Error in system call");
        exit(-1);
    }
    task->computeFunc = computeFunc;
    task->param = param;
    osEnqueue(threadPool->queue, task);
    pthread_cond_signal(&threadPool->cond);
    pthread_mutex_unlock(&threadPool->lock);
    return 0;
}