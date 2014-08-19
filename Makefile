CC = gcc
OBJ = whose_mem.o
CFLAGS = -O2 -Wall -Werror

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: whose_mem

whose_mem: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -fv *.o core whose_mem
