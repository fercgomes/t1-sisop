CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g
LDFLAGS = 

all:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ create_test.c ../lib/libcthread.a -lcthread -o create_test

run:
	./create_test

clean:
	rm -rf *.o $(TARGETS)
