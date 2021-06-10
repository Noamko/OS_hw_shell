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

    ThreadPool* tp = tpCreate(2);

    for (i = 0; i < 3; ++i) {
        tpInsertTask(tp, hello, NULL);
    }
    tpDestroy(tp, 1);
}

int main() {
    testthread_pool_item_pool_sanity();

    return 0;
}
