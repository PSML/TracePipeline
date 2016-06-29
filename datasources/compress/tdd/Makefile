CFLAGS = -Wall -Werror -Wshadow -g -O$(O)
CC = gcc
O = 3

targets = tdd

all: $(targets)
.PHONY: all

tdd: tdd.c tdd.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *~ *.o $(targets)
