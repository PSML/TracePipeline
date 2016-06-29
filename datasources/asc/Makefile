AR = gcc-ar
ARFLAGS = cr
CPPFLAGS = -D _GNU_SOURCE -I .
WFLAGS = -Wall -Wshadow -Wextra -Wno-unused-parameter -Wno-unused-label -Werror
CFLAGS = -ggdb3 $(WFLAGS)
LDFLAGS = -L .
LDLIBS = -lasc -lssv -lfann -lgsl -lgslcblas -lcrypto -lgmp -lm
BENCHMARKS = nops counter loop collatz factor 3sum warshall

ifeq ($(ASAN),1)
CFLAGS += -fno-omit-frame-pointer -fsanitize=address
LDFLAGS += -fsanitize=address
endif

all: benchmarks asc

debug: clean all

release: CFLAGS += -Ofast -march=native -flto -DNDEBUG
release: clean all

asc: main.o help.o command/maps.o command/perf.o command/train.o         \
     libasc.a libssv.a
	$(CC) $(strip $(filter %.o, $^) $(CFLAGS) $(LDFLAGS) $(LDLIBS)) -o $@

main.o: VERSION=$(shell git describe --abbrev=4 --dirty --always --tags)
main.o: CPPFLAGS += -D VERSION=\"$(VERSION)\"
main.o: display/default.h display/recurrence.h display/mispredict.h

libasc.a: maps/lexer.o maps/parser.o ptrace/start.o ptrace/determinize.o \
          ptrace/pc.o peek/gpr.o peek/fpr.o peek/region.o poke/gpr.o     \
          poke/fpr.o poke/region.o perf/attach.o perf/breakpoint.o       \
          perf/instructions.o online/update.o online/loss.o              \
          online/predict.o online/save.o online/resume.o fann/append.o   \
          fann/save.o gsl/bernoulli.o posix/timestamp.o ssv/bind.o       \
          ssv/gather.o
	$(AR) $(ARFLAGS) $@ $?

libssv.a: ssv/calloc.o ssv/free.o ssv/cmp.o ssv/hash.o                   \
          ssv/scan1.o ssv/swap.o ssv/pc.o ssv/qword.o ssv/scatter.o      \
          ssv/xor.o ssv/set.o ssv/popcount.o ssv/tstbit.o ssv/combit.o   \
          ssv/copy.o ssv/setbit.o ssv/init.o
	$(AR) $(ARFLAGS) $@ $?

%.o: %.c asc.h
	$(CC) $(strip $< $(CPPFLAGS) $(CFLAGS)) -c -o $@

maps/lexer.c: maps/lexer.l maps/parser.h maps/parser.o
	$(LEX) -o $@ $<

maps/parser.h: maps/parser.o

maps/parser.c: maps/parser.y
	$(YACC) -o $@ --defines=maps/parser.h $<

benchmarks: $(BENCHMARKS)

k-CPPFLAGS = -nostdinc -isystem /usr/include/diet
k-CFLAGS = -ggdb3
k-LDFLAGS = -static -nostdlib -L /usr/lib/diet/lib
k-LDLIBS = -lc -lm

%: kernels/crt0.s kernels/%.c
	$(CC) $(strip $^ $(k-CPPFLAGS) $(k-CFLAGS) $(k-LDFLAGS) $(k-LDLIBS)) -o $@

%: kernels/%.s
	$(CC) $(strip $^ $(k-CPPFLAGS) $(k-CFLAGS) $(k-LDFLAGS) $(k-LDLIBS)) -o $@

clean:
	$(RM) $(wildcard *.a *.o */*.o *.[ch]~ */*.[ch]~ *.so maps/parser.[ch] asc)

mrproper: clean
	$(RM) $(wildcard *.net *.train *.stdout *.log $(BENCHMARKS))

.PHONY: all debug release benchmarks clean mrproper
