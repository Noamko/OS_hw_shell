#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osqueue.h"
#include "threadPool.h"
void hello(void* a) {
    printf("hello\n");
}

void testthread_pool_item_pool_sanity() {
    int i;

    ThreadPool* tp = tpCreate(5);
    if (tp == NULL) {
        perror("tp is NULL");
        exit(-1);
    }

    for (i = 0; i < 10; ++i) {
        if (tpInsertTask(tp, hello, NULL) < 0) {
            perror("failed to insert");
        }
    }
    tpDestroy(tp, 1);
}

int main() {
    testthread_pool_item_pool_sanity();

    return 0;
}
