CC = gcc
OBJ = main.o vm_primitives.o
CFLAGS += -O2 -Wall -Werror

all: nodebug

nodebug : CFLAGS += -D DEBUG=0

debug : CFLAGS += -D DEBUG=1

nodebug: main

debug: main

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(DEBUGFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -fv *.o main
