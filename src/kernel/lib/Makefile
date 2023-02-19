BIN_DIR=../../../bin/kernel_lib
SC=i686-elf-gcc
SCC=i686-elf-g++
FLAGS=-w -m32 -Wa,--32  -g -nostdlib -nostdinc -nodefaultlibs -trigraphs  -I ./include -c
OBJS= ./src/string.c \
	  ./src/strings.c \
	  ./src/stdlib.c \
	  ./src/ctype.c

OBJO = $(OBJS:.c=.o) 
OBJ=libc.a

all: $(OBJ)
	make install

libc.a: $(OBJO)
	ar rcs $@ *.o

%.o: %.c
	$(SC) $(FLAGS) $<

%.o: %.cc
	$(SCC) $(FLAGS) $<

install:
ifeq ("$(wildcard $(BIN_DIR))", "")
	mkdir $(BIN_DIR)
endif
	cp $(OBJ) $(BIN_DIR)

clean:
	rm -f $(OBJ) *.o *.a