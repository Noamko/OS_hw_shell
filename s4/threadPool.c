#include "threadPool.h"

void threadLoop(void *pool) {
    ThreadPool *tp = (ThreadPool *)pool;
    while (1) {
        Task *task;
        int tookTask = 0;
        pthread_mutex_lock(&tp->lock);  // lock queue acsess
        if (osIsQueueEmpty(tp->queue) != 1) {
            task = (Task *)osDequeue(tp->queue);
            tookTask = 1;
        }
        pthread_mutex_unlock(&tp->lock);  // unlock queue acsess
        if (tookTask) {                   // if the task was enqeued run it
            task->computeFunc(task->param);
            free(task);  // free the task pointer
        }
    }
}
ThreadPool *tpCreate(int numOfThreads) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    pool->queue = osCreateQueue();
    pool->threads = malloc(sizeof(pthread_t) * numOfThreads);
    if (pool->threads == NULL) {
        perror("Error in system call");
    }
    pool->numOfThreads = numOfThreads;
    pool->isDestroyed = 0;

    int i;
    for (i = 0; i < numOfThreads; i++) {
        if (pthread_create(&pool->threads[i], NULL, (void *)threadLoop, pool)) {
            perror("Error in system call");
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
    int i;
    for (i = 0; i < threadPool->numOfThreads; i++) {
        if (pthread_cancel(threadPool->threads[i]) < 0) {
            perror("Error in system call");
        }
    }
    free(threadPool->threads);
    free(threadPool);
    osDestroyQueue(threadPool->queue);
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    if (threadPool->isDestroyed) {
        return -1;
    }
    Task *task = malloc(sizeof(Task));
    if (task == NULL) {
        perror("Error in system call");
    }
    task->computeFunc = computeFunc;
    task->param = param;
    osEnqueue(threadPool->queue, task);
    return 0;
}