// Noam Koren 308192871
#include "threadPool.h"

// #define DEBUG

void log(const char *t, const char *log) {
#ifdef DEBUG
    printf(t, log);
#endif
}
void threadLoop(void *t) {
    ThreadPool *pool = (ThreadPool *)t;  //cast generic Pointer to ThreadPool*
    while (1) {
        Task *task;
        pthread_mutex_lock(&pool->flock);  //Lock before enetring critical section
        pthread_mutex_lock(&pool->slock);
        if (pool->exitloop) {  // check if should continue or threadpool finished work.
            log("%s\n", "waited thread skipped because TheadPool is destroyed.");
            pthread_mutex_unlock(&pool->slock);
            pthread_mutex_unlock(&pool->flock);
            break;
        }
        //CRITIAL SECTION//
        while (osIsQueueEmpty(pool->queue) == 1 && !pool->isDestroyed) {
            log("%ld blocked.\n", gettid());
            pthread_cond_wait(&pool->cond, &pool->slock);
            log("%ld released.\n", gettid());
            if (pool->exitloop) {
                log("%s\n", "exited after signal");
                pthread_mutex_unlock(&pool->slock);
                break;
            }
        }
        if (pool->isDestroyed) {
            if (osIsQueueEmpty(pool->queue) == 1) {
                pool->exitloop = 1;
                log("%ld", gettid());
                log(" %s\n", "calling all threads to stop.");
                pthread_mutex_unlock(&pool->slock);
                pthread_mutex_unlock(&pool->flock);
                break;
            }
        }
        task = (Task *)osDequeue(pool->queue);
        //END OF CRITICAL SECTION//
        pthread_mutex_unlock(&pool->flock);
        pthread_mutex_unlock(&pool->slock);
        if (task != NULL) {  // if the task was enqeued run it
            task->computeFunc(task->param);
            free(task);  // free the task pointer
        }
    }
    log("%ld finished\n", gettid());
}

ThreadPool *tpCreate(int numOfThreads) {
    if (numOfThreads <= 0) {
        perror("invalid thread count");
        return NULL;
    }

    ThreadPool *pool = malloc(sizeof(ThreadPool));  //allocate treadpool on the heap
    if (pool == NULL) {
        perror("malloc failed");
        return NULL;
    }

    pool->queue = osCreateQueue();  // we assume this works

    pool->threads = malloc(sizeof(pthread_t) * numOfThreads);
    if (pool->threads == NULL) {
        perror("malloc failed");
        return NULL;
    }
    pool->numOfThreads = numOfThreads;
    pool->isDestroyed = 0;
    pool->exitloop = 0;
    pthread_mutex_init(&pool->flock, NULL);  //init locks and conds
    pthread_mutex_init(&pool->slock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    int i;
    for (i = 0; i < numOfThreads; i++) {
        if (pthread_create(&pool->threads[i], NULL, (void *)threadLoop, pool)) {
            perror("failed to create thread");
            return NULL;
        }
    }

    return pool;
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    threadPool->isDestroyed = 1;
    if (!shouldWaitForTasks) {
        threadPool->exitloop = 1;
    }
    pthread_cond_broadcast(&threadPool->cond);
    int i;
    for (i = 0; i < threadPool->numOfThreads; i++) {
        log("waiting for %d\n", i);
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
    Task *task = malloc(sizeof(Task));
    if (task == NULL) {
        perror("error failed to malloc");
        exit(-1);
    }
    task->computeFunc = computeFunc;
    task->param = param;
    pthread_cond_signal(&threadPool->cond);
    pthread_mutex_lock(&threadPool->slock);
    osEnqueue(threadPool->queue, task);
    pthread_mutex_unlock(&threadPool->slock);
    pthread_cond_signal(&threadPool->cond);
    return 0;
}