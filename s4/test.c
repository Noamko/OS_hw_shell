#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osqueue.h"
#include "threadPool.h"
void hello(void* a) {
    int t = *(int*)a;
    printf("%d\n", t);
    free(a);
}

void testthread_pool_item_pool_sanity() {
    int i;
    ThreadPool* tp = tpCreate(100);

    for (i = 0; i < 100; ++i) {
        int* t = malloc(sizeof(int));
        *t = i;
        tpInsertTask(tp, (void*)hello, t);
    }
    tpDestroy(tp, 1);
}

int main() {
    testthread_pool_item_pool_sanity();

    return 0;
}
