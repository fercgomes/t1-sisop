#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"

CC=gcc

LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=testes

CFLAGS= -I include/ -g -Wall -Wno-return-type -D_REENTRANT
LDFLAGS= -L lib/

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard include/*.h)

TARGET = $(LIB_DIR)/libcthread.a
MOODLE_NAME = entrega.tar.gz

.PHONY: all cthread tests pack

all: cthread

cthread: $(OBJECTS)
	ar crs $(TARGET) $(OBJECTS) $(BIN_DIR)/support.o	

# Generic rules
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile tests
tests:
	$(MAKE) -C $(TEST_DIR)

clean:
	rm -rf $(TARGET) src/*.o $(LIB_DIR)/*.a $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

pack:
	@echo "Preparando para entrega..."
	tar -zcvf $(MOODLE_NAME) bin/ exemplos/ include/ lib/ src/ testes/ Makefile
