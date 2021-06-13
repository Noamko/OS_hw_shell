all: test clean

test: threadPool.o
	gcc -g test.c threadPool.o osqueue.o -lpthread

threadPool.o: osqueue.o
	gcc -g -c threadPool.c

osqueue.o:
	gcc -g -c osqueue.c

clean: 
	rm -f *.o