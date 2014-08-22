CC = gcc
OBJ = whose_mem.o vm_primitives.o
CFLAGS += -O2 -Wall -Werror

all: nodebug

nodebug : CFLAGS += -D DEBUG=0

debug : CFLAGS += -D DEBUG=1

nodebug: whose_mem

debug: whose_mem

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(DEBUGFLAGS)

whose_mem: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -fv *.o core whose_mem
