CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS = 
LDFLAGS = 

all: $(TARGETS)
	echo $(TARGETS)

$(TARGETS): $(SRC_FILES)

%.c:
	$(CC) $(CFLAGS) $@ -o $< $(LDFLAGS)

clean:
	rm -rf *.o $(TARGETS)