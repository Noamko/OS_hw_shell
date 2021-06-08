#include "threadPool.h"

void runTask(void **args) {
    Task *task = (Task *)args[0];
    void *self = args[1];
    task->computeFunc(task->param);
    ((_Thread *)self)->isRunning = 0;
    free(task);
}

void *threadLoop(void *pool) {
    ThreadPool *_threadpool = (ThreadPool *)pool;
    while (1) {
        sleep(1);
        if (!osIsQueueEmpty(_threadpool->queue)) {
            for (int i = 0; i < _threadpool->numOfThreads; i++) {
                if (!_threadpool->threads[i]->isRunning) {
                    _threadpool->threads[i]->isRunning = 1;
                    Task *task = (Task *)osDequeue(_threadpool->queue);
                    _Thread *t = _threadpool->threads[i];
                    void *args[2] = {task, t};
                    if (pthread_create(&_threadpool->threads[i]->thread, NULL, (void *)runTask, args)) {
                        perror("error");
                    }
                    break;
                }
            }
        }
    }
}
ThreadPool *tpCreate(int numOfThreads) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    pool->queue = osCreateQueue();
    pool->threads = malloc(sizeof(_Thread *) * numOfThreads);
    pool->numOfThreads = numOfThreads;
    for (int i = 0; i < numOfThreads; i++) {
        pool->threads[i] = malloc(sizeof(_Thread));
        pool->threads[i]->isRunning = 0;
    }

    if (pthread_create(&pool->main_thread, NULL, threadLoop, pool) < 0) {
        perror("failed to create thread");
    }
    return pool;
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    if (shouldWaitForTasks) {
        while (osIsQueueEmpty(threadPool->queue) != 1) {
        }
        pthread_cancel(threadPool->main_thread);
    } else {
        pthread_cancel(threadPool->main_thread);
    }
    pthread_join(threadPool->main_thread, NULL);
    osDestroyQueue(threadPool->queue);
    for (int i = 0; i < threadPool->numOfThreads; i++) {
        free(threadPool->threads[i]);
    }
    free(threadPool);
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    Task *task = malloc(sizeof(Task));
    task->computeFunc = computeFunc;
    task->param = param;
    osEnqueue(threadPool->queue, task);
    return 0;
}