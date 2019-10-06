CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run clean
	
run:
	./create_test
	./semaphore_test
	./join_test	
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ create_test.c -lcthread -o create_test
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ semaphore_test.c -lcthread -o semaphore_test
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ join_test.c -lcthread -o join_test

clean:
	rm -rf *.o $(TARGETS) ./generated_data/*.txt
