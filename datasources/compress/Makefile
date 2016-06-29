CFLAGS = -Wall -Werror -Wshadow -g -O$(O)
CC = gcc
O = 3

targets = 6502act-getdict bochs32act-getdict
#targets = com6502sv decom6502sv com6502act combochs32 6502act-getdict bochs32act-getdict comhuff6502sv

all: $(targets)
.PHONY: all

com6502sv: com6502sv.c tools/bitxor.o uthash/src/uthash.h
	$(CC) $(CFLAGS) -mmmx $< tools/bitxor.o -o $@

decom6502sv: decom6502sv.c
	$(CC) $(CFLAGS) $< -o $@

com6502act: com6502act.c uthash/src/uthash.h
	$(CC) $(CFLAGS) $< -o $@

combochs32: combochs32.c uthash/src/uthash.h
	$(CC) $(CFLAGS) $< -o $@

6502act-getdict: act-getdict.c uthash/src/uthash.h
	$(CC) $(CFLAGS) -D__6502__ $< -o $@

bochs32act-getdict: act-getdict.c uthash/src/uthash.h
	$(CC) $(CFLAGS) -D__BOCHS32__ $< -o $@

comhuff6502sv: comhuff6502sv.c tools/bitxor.o rbtree.o
	$(CC) $(CFLAGS) -mmmx $< tools/bitxor.o rbtree.o -o $@

uthash/src/uthash.h:
	git clone git://github.com/troydhanson/uthash.git

tools/bitxor.o: 
	make -C tools bitxor.o

rbtree.o: rbtree.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf *~ *.o $(targets)
	make -C cst_v_1_1 clean
	make -C sequitur clean
	make -C tdd clean
	make -C tools clean
