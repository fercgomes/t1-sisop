CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run
	
run:
	./create_test
	./semaphore_test
	./join_test
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ create_test.c ../lib/libcthread.a -lcthread -o create_test
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ semaphore_test.c ../lib/libcthread.a -lcthread -o semaphore_test
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ join_test.c ../lib/libcthread.a -lcthread -o join_test

clean:
	rm -rf *.o $(TARGETS)
