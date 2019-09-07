CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g
LDFLAGS = 

all:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ test1.c ../lib/libcthread.a -lcthread -o test1

run:
	./test1

clean:
	rm -rf *.o $(TARGETS)