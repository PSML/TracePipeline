CFLAGS = -Wall -Werror -Wshadow -g -O$(O)
CC = gcc
O = 3

targets = stats6502xor stats6502act statsbochs32 filter6502act filterbochs32 bitxor bitxorseg bitset

all: $(targets)
.PHONY: all

stats6502xor: statscomtrc.c
	$(CC) $(CFLAGS) -D__6502__ -D__XOR__ $< -o $@

stats6502act: statscomtrc.c
	$(CC) $(CFLAGS) -D__6502__ -D__ACTION__ $< -o $@

statsbochs32: statscomtrc.c
	$(CC) $(CFLAGS) -D__BOCHS32__ -D__ACTION__ $< -o $@

filter6502act: filteracttrc.c
	$(CC) $(CFLAGS) -D__6502__ $< -o $@

filterbochs32: filteracttrc.c
	$(CC) $(CFLAGS) -D__BOCHS32__ $< -o $@

bitxor: bitxor.c
	$(CC) $< $(CFLAGS) -lcrypto -mmmx -D __BITXOR_STAND_ALONE__ -o $@

bitxorseg: bitxorseg.c
	$(CC) $(CFLAGS) -mmmx -D __BITXORSEG_STAND_ALONE__ $< -o $@

bitset: bitset.c
	$(CC) $(CFLAGS) $< -o $@

bitxor.o: bitxor.c
	$(CC) $(CFLAGS) -mmmx $< -c

bitxorseg.o: bitxorseg.c
	$(CC) $(CFLAGS) -mmmx  $< -c 

clean:
	-rm -rf *~ *.o $(targets)
