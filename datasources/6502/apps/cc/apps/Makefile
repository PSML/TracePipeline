CC65BINDIR = ../../../ext/install/bin

CC = $(CC65BINDIR)/cc65
AS = $(CC65BINDIR)/ca65
LD = $(CC65BINDIR)/ld65
AR = $(CC65BINDIR)/ar65

BU6502OBJS = bu6502interrupt.o crt0.o ctype.o cputc.o cgetc.o oserror.o

BU6502LIB = bu6502.lib 
BU6502CFG = bu6502.cfg 
SYSLIB = ../../../cc65lib/lib/none.lib

all: ${BU6502LIB} memcpy.img hello.img concpy.img collatz.img tests.img loop.img p1.img p2.img p3.img fib.img fibtbl.img  otter.img readtest.img figforth.img native

.PHONY: all native

memcpy.img: memcpy
	cp memcpy memcpy.img

memcpy: memcpy.o  ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o memcpy -C ${BU6502CFG} memcpy.o  ${BU6502LIB}  -m memcpy.map

memcpy.o: memcpy.s
	$(AS) --cpu 6502 memcpy.s -l memcpy.lst

memcpy.s: memcpy.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 memcpy.c

memcpy.res: ../../../6502 memcpy.img
	bash -c '../../../6502 memcpy.img memcpy.img.out < /dev/null  > /dev/null  2> memcpy.trc'
	echo "INPUT:" > memcpy.res
	../../../img dump memcpy.img >> memcpy.res
	echo "OUTPUT:" >> memcpy.res
	../../../img dump memcpy.img.out >> memcpy.res
	cat memcpy.res
	@bash -c 'num=$$(grep Hello memcpy.res | wc -l);\
	 if (( num == 3 )); then echo MEMCOPY: PASS; else echo MEMCOPY: FAIL; fi' 

########

memcpy1.img: memcpy1
	cp memcpy1 memcpy1.img

memcpy1: memcpy1.o  ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o memcpy1 -C ${BU6502CFG} memcpy1.o  ${BU6502LIB}  -m memcpy1.map

memcpy1.o: memcpy1.s
	$(AS) --cpu 6502 memcpy1.s -l memcpy1.lst

memcpy1.s: memcpy.c
	$(CC) -D__6502__ -DSTR=1 -t none -O -Oi --cpu 6502 memcpy.c -o memcpy1.s

memcpy1.res: ../../../6502 memcpy1.img
	bash -c '../../../6502 memcpy1.img memcpy1.img.out < /dev/null  > /dev/null  2> memcpy1.trc'
	echo "INPUT:" > memcpy1.res
	../../../img dump memcpy1.img >> memcpy1.res
	echo "OUTPUT:" >> memcpy1.res
	../../../img dump memcpy1.img.out >> memcpy1.res
	cat memcpy1.res
	@bash -c 'num=$$(grep JoNaT memcpy1.res | wc -l);\
	 if (( num == 3 )); then echo MEMCOPY1: PASS; else echo MEMCOPY1: FAIL; fi' 

memcpy1.sv: ../../../6502 memcpy1.img
	bash -c '../../../6502 -t sv -o memcpy1.sv memcpy1.img memcpy1.img.out < /dev/null  > /dev/null  2> memcpy1.trc'
	../../../2bmp -e0,7 -r 1068 65544 memcpy1.sv

#######

memcpy2.img: memcpy2
	cp memcpy2 memcpy2.img

memcpy2: memcpy2.o  ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o memcpy2 -C ${BU6502CFG} memcpy2.o  ${BU6502LIB}  -m memcpy2.map

memcpy2.o: memcpy2.s
	$(AS) --cpu 6502 memcpy2.s -l memcpy2.lst

memcpy2.s: memcpy.c
	$(CC) -D__6502__ -DSTR=2 -t none -O -Oi --cpu 6502 memcpy.c -o memcpy2.s

memcpy2.res: ../../../6502 memcpy2.img
	bash -c '../../../6502 memcpy2.img memcpy2.img.out < /dev/null  > /dev/null  2> memcpy2.trc'
	echo "INPUT:" > memcpy2.res
	../../../img dump memcpy2.img >> memcpy2.res
	echo "OUTPUT:" >> memcpy2.res
	../../../img dump memcpy2.img.out >> memcpy2.res
	cat memcpy2.res
	@bash -c 'num=$$(grep 012TO memcpy2.res | wc -l);\
	 if (( num == 3 )); then echo MEMCOPY2: PASS; else echo MEMCOPY2: FAIL; fi' 

memcpy2.sv: ../../../6502 memcpy2.img
	bash -c '../../../6502 -t sv -o memcpy2.sv memcpy2.img memcpy2.img.out < /dev/null  > /dev/null  2> memcpy2.trc'
	../../../2bmp -e0,7 -r 1068 65544 memcpy2.sv

#######

memcpy3.img: memcpy3
	cp memcpy3 memcpy3.img

memcpy3: memcpy3.o  ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o memcpy3 -C ${BU6502CFG} memcpy3.o  ${BU6502LIB}  -m memcpy3.map

memcpy3.o: memcpy3.s
	$(AS) --cpu 6502 memcpy3.s -l memcpy3.lst

memcpy3.s: memcpy.c
	$(CC) -D__6502__ -DSTR=3 -t none -O -Oi --cpu 6502 memcpy.c -o memcpy3.s

memcpy3.res: ../../../6502 memcpy3.img
	bash -c '../../../6502 memcpy3.img memcpy3.img.out < /dev/null  > /dev/null  2> memcpy3.trc'
	echo "INPUT:" > memcpy3.res
	../../../img dump memcpy3.img >> memcpy3.res
	echo "OUTPUT:" >> memcpy3.res
	../../../img dump memcpy3.img.out >> memcpy3.res
	cat memcpy3.res
	@bash -c 'num=$$(grep E+uR, memcpy3.res | wc -l);\
	 if (( num == 3 )); then echo MEMCOPY3: PASS; else echo MEMCOPY3: FAIL; fi' 

memcpy3.sv: ../../../6502 memcpy3.img
	bash -c '../../../6502 -t sv -o memcpy3.sv memcpy3.img memcpy3.img.out < /dev/null  > /dev/null  2> memcpy3.trc'
	../../../2bmp -e0,7 -r 1068 65544 memcpy3.sv

#######
concpy.img: concpy
	cp concpy concpy.img

concpy: concpy.o ${BU6502CFG}   ${BU6502LIB}
	$(LD) -o concpy -C ${BU6502CFG}  concpy.o  ${BU6502LIB}  -m concpy.map

concpy.o: concpy.s
	$(AS) --cpu 6502 concpy.s -l concpy.lst

concpy.s: concpy.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 concpy.c

concpy.res: ../../../6502 concpy.img
	bash -c 'echo "Hello World" | ../../../6502 concpy.img concpy.img.out  > concpy.con  2> concpy.trc'
	cat concpy.con | grep -v '^6502Lab' > concpy.res
	cat concpy.res
	@bash -c 'res=$$(cat concpy.res);\
	 if [[ $$res = "Hello World" ]]; then echo CONCPY: PASS; else echo CONCPY: FAIL; fi' 

#######

readtest.img: readtest
	cp readtest readtest.img

readtest: readtest.o ${BU6502CFG} ${BU6502OBJS} ${BU6502LIB}
	$(LD) -o readtest -C ${BU6502CFG} ${BU6502OBJS} readtest.o  ${BU6502LIB}  -m readtest.map

readtest.o: readtest.s
	$(AS) --cpu 6502 readtest.s -l readtest.lst

readtest.s: readtest.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 readtest.c

readtest.res: ../../../6502 readtest.img
	 bash -c '../../../6502 readtest.img readtest.oimg'

#######

collatz.img: collatz
	cp collatz collatz.img

collatz: collatz.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o collatz -C ${BU6502CFG}  collatz.o  ${BU6502LIB}  -m collatz.map

collatz.o: collatz.s
	$(AS) --cpu 6502 collatz.s -l collatz.lst

collatz.s: collatz.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 collatz.c

collatz.res: ../../../6502 collatz.img
	bash -c '../../../6502 collatz.img collatz.oimg'

#######

loop.img: loop
	cp loop loop.img

loop: loop.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o loop -C ${BU6502CFG}  loop.o  ${BU6502LIB}  -m loop.map

loop.o: loop.s
	$(AS) --cpu 6502 loop.s -l loop.lst

loop.s: loop.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 loop.c

loop.res: ../../../6502 loop.img
	bash -c '../../../6502 loop.img loop.oimg'

#######

p1.img: p1
	cp p1 p1.img

p1: p1.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o p1 -C ${BU6502CFG}  p1.o  ${BU6502LIB}  -m p1.map

p1.o: p1.s
	$(AS) --cpu 6502 p1.s -l p1.lst

p1.s: p1.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 p1.c

p1.res: ../../../6502 p1.img
	bash -c '../../../6502 p1.img p1.oimg'

#######

p2.img: p2
	cp p2 p2.img

p2: p2.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o p2 -C ${BU6502CFG}  p2.o  ${BU6502LIB}  -m p2.map

p2.o: p2.s
	$(AS) --cpu 6502 p2.s -l p2.lst

p2.s: p2.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 p2.c

p2.res: ../../../6502 p2.img
	bash -c '../../../6502 p2.img p2.oimg'

#######

p3.img: p3
	cp p3 p3.img

p3: p3.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o p3 -C ${BU6502CFG}  p3.o  ${BU6502LIB}  -m p3.map

p3.o: p3.s
	$(AS) --cpu 6502 p3.s -l p3.lst

p3.s: p3.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 p3.c

p3.res: ../../../6502 p3.img
	bash -c '../../../6502 p3.img p3.oimg'

#######

fib.img: fib
	cp fib fib.img

fib: fib.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o fib -C ${BU6502CFG}  fib.o  ${BU6502LIB}  -m fib.map

fib.o: fib.s
	$(AS) --cpu 6502 fib.s -l fib.lst

fib.s: fib.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 fib.c

fib.sv.trc: ../../../6502 fib.img fib-20,10.dat
	bash -c '../../../6502 -ts -O fib.sv.trc fib.img fib.oimg < fib-20,10.dat > fib-sv_fib-20,10.dat.out 2> fib-sv_fib-20,10.dat.log'

fib.er.trc: 
	bash -c '../../../6502 -te -O fib.er.trc fib.img fib.oimg < fib-20,10.dat > fib-er_fib-20,10.dat.out 2> fib2-er_fib-20,10.dat.log'

#######

fibtbl.img: fibtbl
	cp fibtbl fibtbl.img

fibtbl: fibtbl.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o fibtbl -C ${BU6502CFG}  fibtbl.o  ${BU6502LIB}  -m fibtbl.map

fibtbl.o: fibtbl.s
	$(AS) --cpu 6502 fibtbl.s -l fibtbl.lst

fibtbl.s: fibtbl.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 fibtbl.c

fib-20,10.dat:
	./mkfibdata 20 10 > fib-20,10.dat

#######

fibiter.img: fibiter
	cp fibiter fibiter.img

fibiter: fibiter.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o fibiter -C ${BU6502CFG}  fibiter.o  ${BU6502LIB}  -m fibiter.map

fibiter.o: fibiter.s
	$(AS) --cpu 6502 fibiter.s -l fibiter.lst

fibiter.s: fibiter.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 fibiter.c

#######

fibglb.img: fibglb
	cp fibglb fibglb.img

fibglb: fibglb.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o fibglb -C ${BU6502CFG}  fibglb.o  ${BU6502LIB}  -m fibglb.map

fibglb.o: fibglb.s
	$(AS) --cpu 6502 fibglb.s -l fibglb.lst

fibglb.s: fibglb.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 fibglb.c

#######

fibtblglb.img: fibtblglb
	cp fibtblglb fibtblglb.img

fibtblglb: fibtblglb.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o fibtblglb -C ${BU6502CFG}  fibtblglb.o  ${BU6502LIB}  -m fibtblglb.map

fibtblglb.o: fibtblglb.s
	$(AS) --cpu 6502 fibtblglb.s -l fibtblglb.lst

fibtblglb.s: fibtblglb.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 fibtblglb.c

#######

ising.img: ising
	cp ising ising.img

ising: ising.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o ising -C ${BU6502CFG} ising.o  ${BU6502LIB}  -m ising.map

ising.o: ising.s
	$(AS) --cpu 6502 ising.s -l ising.lst

ising.s: ising.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 ising.c

#######

otter.img: otter
	cp otter otter.img

otter: otter.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o otter -C ${BU6502CFG}  otter.o  ${BU6502LIB}  -m otter.map

otter.o: otter.s
	$(AS) --cpu 6502 otter.s -l otter.lst

otter.s: otter.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 otter.c


#######

inner-static-0.0-8192.img: inner-static-0.0-8192
	cp inner-static-0.0-8192 inner-static-0.0-8192.img


inner-static-0.0-8192: inner-static-0.0-8192.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o inner-static-0.0-8192 -C ${BU6502CFG}  inner-static-0.0-8192.o  ${BU6502LIB}  -m inner-static-0.0-8192.map

inner-static-0.0-8192.o: inner-static-0.0-8192.s
	$(AS) --cpu 6502 inner-static-0.0-8192.s -l inner-static-0.0-8192.lst

inner-static-0.0-8192.s: inner-static.c data/u_0.0_8192_1.h data/v_0.0_8192_1.h
	cp data/u_0.0_8192_1.h u.h
	cp data/v_0.0_8192_1.h v.h
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 inner-static.c -DN=8192 -o inner-static-0.0-8192.s

#######

inner-static-0.0-256.img: inner-static-0.0-256
	cp inner-static-0.0-256 inner-static-0.0-256.img


inner-static-0.0-256: inner-static-0.0-256.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o inner-static-0.0-256 -C ${BU6502CFG}  inner-static-0.0-256.o  ${BU6502LIB}  -m inner-static-0.0-256.map

inner-static-0.0-256.o: inner-static-0.0-256.s
	$(AS) --cpu 6502 inner-static-0.0-256.s -l inner-static-0.0-256.lst

inner-static-0.0-256.s: inner-static.c data/u_0.0_256_1.h data/v_0.0_256_1.h
	cp data/u_0.0_256_1.h u.h
	cp data/v_0.0_256_1.h v.h
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 inner-static.c -DN=256 -o inner-static-0.0-256.s


#######

inner-static-0.1-256-1-10.img: inner-static-0.1-256-1-10
	cp inner-static-0.1-256-1-10 inner-static-0.1-256-1-10.img


inner-static-0.1-256-1-10: inner-static-0.1-256-1-10.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o inner-static-0.1-256-1-10 -C ${BU6502CFG}  inner-static-0.1-256-1-10.o  ${BU6502LIB}  -m inner-static-0.1-256-1-10.map

inner-static-0.1-256-1-10.o: inner-static-0.1-256-1-10.s
	$(AS) --cpu 6502 inner-static-0.1-256-1-10.s -l inner-static-0.1-256-1-10.lst

inner-static-0.1-256-1-10.s: inner-static.c data/u_0.1_256_1.h data/v_0.1_256_10.h
	cp data/u_0.1_256_1.h u.h
	cp data/v_0.1_256_10.h v.h
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 inner-static.c -DN=256 -o inner-static-0.1-256-1-10.s

#######

hello.img: hello
	cp hello hello.img

hello: hello.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o hello -C ${BU6502CFG}  hello.o   ${BU6502LIB}  -m hello.map

hello.o: hello.s
	$(AS) --cpu 6502 hello.s -l hello.lst

hello.s: hello.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 hello.c

hello.res: ../../../6502 hello.img
	bash -c '../../../6502 hello.img hello.oimg'

cc65tests: heaptest.res cprintftest.res atoi-test.res shift-test.res strtoul-test.res strtol-test.res strnicmp-test.res strncmp-test.res strdup-test.res strchr-test.res div-test.res
cc65longtests: mul-test.res moddiv-test.res
#######
heaptest.img: heaptest
	cp heaptest heaptest.img

heaptest: heaptest.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o heaptest -C ${BU6502CFG}  heaptest.o  ${BU6502LIB}  -m heaptest.map

heaptest.o: heaptest.s
	$(AS) --cpu 6502 heaptest.s -l heaptest.lst

heaptest.s: heaptest.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 heaptest.c

heaptest.res: ../../../6502 heaptest.img
	bash -c '../../../6502 heaptest.img heaptest.oimg > heaptest.con 2> heaptest.trc'
	cat heaptest.con | grep 'SUCCESS' 
	cat heaptest.con | grep 'SUCCESS' > heaptest.res
	@bash -c 'res=$$(cat heaptest.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo heaptest: PASS; else echo heaptest: FAIL; fi' 

#######
cprintftest.img: cprintftest
	cp cprintftest cprintftest.img

cprintftest: cprintftest.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o cprintftest -C ${BU6502CFG}  cprintftest.o  ${BU6502LIB}  -m cprintftest.map

cprintftest.o: cprintftest.s
	$(AS) --cpu 6502 cprintftest.s -l cprintftest.lst

cprintftest.s: cprintftest.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 cprintftest.c

cprintftest.res: ../../../6502 cprintftest.img
	bash -c '../../../6502 cprintftest.img cprintftest.oimg > cprintftest.con 2> cprintftest.trc'
	cat cprintftest.con | grep 'SUCCESS' 
	cat cprintftest.con | grep 'SUCCESS' > cprintftest.res
	@bash -c 'res=$$(cat cprintftest.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo cprintftest: PASS; else echo cprintftest: FAIL; fi' 

#######

atoi-test.img: atoi-test
	cp atoi-test atoi-test.img

atoi-test: atoi-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o atoi-test -C ${BU6502CFG}  atoi-test.o  ${BU6502LIB}  -m atoi-test.map

atoi-test.o: atoi-test.s
	$(AS) --cpu 6502 atoi-test.s -l atoi-test.lst

atoi-test.s: atoi-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 atoi-test.c

atoi-test.res: ../../../6502 atoi-test.img
	bash -c '../../../6502 atoi-test.img atoi-test.oimg > atoi-test.con 2> atoi-test.trc'
	cat atoi-test.con | grep 'SUCCESS' 
	cat atoi-test.con | grep 'SUCCESS' > atoi-test.res
	@bash -c 'res=$$(cat atoi-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo atoi-test: PASS; else echo atoi-test: FAIL; fi' 

#######

shift-test.img: shift-test
	cp shift-test shift-test.img

shift-test: shift-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o shift-test -C ${BU6502CFG}  shift-test.o  ${BU6502LIB}  -m shift-test.map

shift-test.o: shift-test.s
	$(AS) --cpu 6502 shift-test.s -l shift-test.lst

shift-test.s: shift-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 shift-test.c

shift-test.res: ../../../6502 shift-test.img
	bash -c '../../../6502 shift-test.img shift-test.oimg > shift-test.con 2> shift-test.trc'
	cat shift-test.con | grep 'SUCCESS' 
	cat shift-test.con | grep 'SUCCESS' > shift-test.res
	@bash -c 'res=$$(cat shift-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo shift-test: PASS; else echo shift-test: FAIL; fi' 

#######

strtoul-test.img: strtoul-test
	cp strtoul-test strtoul-test.img

strtoul-test: strtoul-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strtoul-test -C ${BU6502CFG}  strtoul-test.o  ${BU6502LIB}  -m strtoul-test.map

strtoul-test.o: strtoul-test.s
	$(AS) --cpu 6502 strtoul-test.s -l strtoul-test.lst

strtoul-test.s: strtoul-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strtoul-test.c

strtoul-test.res: ../../../6502 strtoul-test.img
	bash -c '../../../6502 strtoul-test.img strtoul-test.oimg > strtoul-test.con 2> strtoul-test.trc'
	cat strtoul-test.con | grep 'SUCCESS'
	cat strtoul-test.con | grep 'SUCCESS' > strtoul-test.res
	@bash -c 'res=$$(cat strtoul-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strtoul-test: PASS; else echo strtoul-test: FAIL; fi' 

#######

strtol-test.img: strtol-test
	cp strtol-test strtol-test.img

strtol-test: strtol-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strtol-test -C ${BU6502CFG}  strtol-test.o  ${BU6502LIB}  -m strtol-test.map

strtol-test.o: strtol-test.s
	$(AS) --cpu 6502 strtol-test.s -l strtol-test.lst

strtol-test.s: strtol-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strtol-test.c

strtol-test.res: ../../../6502 strtol-test.img
	bash -c '../../../6502 strtol-test.img strtol-test.oimg > strtol-test.con 2> strtol-test.trc'
	cat strtol-test.con | grep 'SUCCESS' 
	cat strtol-test.con | grep 'SUCCESS' > strtol-test.res
	@bash -c 'res=$$(cat strtol-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strtol-test: PASS; else echo strtol-test: FAIL; fi' 


#######

strnicmp-test.img: strnicmp-test
	cp strnicmp-test strnicmp-test.img

strnicmp-test: strnicmp-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strnicmp-test -C ${BU6502CFG}  strnicmp-test.o  ${BU6502LIB}  -m strnicmp-test.map

strnicmp-test.o: strnicmp-test.s
	$(AS) --cpu 6502 strnicmp-test.s -l strnicmp-test.lst

strnicmp-test.s: strnicmp-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strnicmp-test.c

strnicmp-test.res: ../../../6502 strnicmp-test.img
	bash -c '../../../6502 strnicmp-test.img strnicmp-test.oimg > strnicmp-test.con 2> strnicmp-test.trc'
	cat strnicmp-test.con | grep 'SUCCESS' 
	cat strnicmp-test.con | grep 'SUCCESS' > strnicmp-test.res
	@bash -c 'res=$$(cat strnicmp-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strnicmp-test: PASS; else echo strnicmp-test: FAIL; fi' 
#######

strncmp-test.img: strncmp-test
	cp strncmp-test strncmp-test.img

strncmp-test: strncmp-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strncmp-test -C ${BU6502CFG}  strncmp-test.o  ${BU6502LIB}  -m strncmp-test.map

strncmp-test.o: strncmp-test.s
	$(AS) --cpu 6502 strncmp-test.s -l strncmp-test.lst

strncmp-test.s: strncmp-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strncmp-test.c

strncmp-test.res: ../../../6502 strncmp-test.img
	bash -c '../../../6502 strncmp-test.img strncmp-test.oimg > strncmp-test.con 2> strncmp-test.trc'
	cat strncmp-test.con | grep 'SUCCESS' 
	cat strncmp-test.con | grep 'SUCCESS' > strncmp-test.res
	@bash -c 'res=$$(cat strncmp-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strncmp-test: PASS; else echo strncmp-test: FAIL; fi' 
#######

strdup-test.img: strdup-test
	cp strdup-test strdup-test.img

strdup-test: strdup-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strdup-test -C ${BU6502CFG}  strdup-test.o  ${BU6502LIB}  -m strdup-test.map

strdup-test.o: strdup-test.s
	$(AS) --cpu 6502 strdup-test.s -l strdup-test.lst

strdup-test.s: strdup-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strdup-test.c

strdup-test.res: ../../../6502 strdup-test.img
	bash -c '../../../6502 strdup-test.img strdup-test.oimg > strdup-test.con 2> strdup-test.trc'
	cat strdup-test.con | grep 'SUCCESS' 
	cat strdup-test.con | grep 'SUCCESS' > strdup-test.res
	@bash -c 'res=$$(cat strdup-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strdup-test: PASS; else echo strdup-test: FAIL; fi' 
	if (count == 0) printf("\nSUCCESS\n");

#######

strchr-test.img: strchr-test
	cp strchr-test strchr-test.img

strchr-test: strchr-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o strchr-test -C ${BU6502CFG}  strchr-test.o  ${BU6502LIB}  -m strchr-test.map

strchr-test.o: strchr-test.s
	$(AS) --cpu 6502 strchr-test.s -l strchr-test.lst

strchr-test.s: strchr-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 strchr-test.c

strchr-test.res: ../../../6502 strchr-test.img
	bash -c '../../../6502 strchr-test.img strchr-test.oimg > strchr-test.con 2> strchr-test.trc'
	cat strchr-test.con | grep 'SUCCESS' 
	cat strchr-test.con | grep 'SUCCESS' > strchr-test.res
	@bash -c 'res=$$(cat strchr-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo strchr-test: PASS; else echo strchr-test: FAIL; fi' 
	if (count == 0) printf("\nSUCCESS\n");


######
mul-test.img: mul-test
	cp mul-test mul-test.img

mul-test: mul-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o mul-test -C ${BU6502CFG}  mul-test.o  ${BU6502LIB}  -m mul-test.map

mul-test.o: mul-test.s
	$(AS) --cpu 6502 mul-test.s -l mul-test.lst

mul-test.s: mul-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 mul-test.c

mul-test.res: ../../../6502 mul-test.img
	bash -c '../../../6502 mul-test.img mul-test.oimg > mul-test.con 2> mul-test.trc'
	cat mul-test.con | grep 'SUCCESS' 
	cat mul-test.con | grep 'SUCCESS' > mul-test.res
	@bash -c 'res=$$(cat mul-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo mul-test: PASS; else echo mul-test: FAIL; fi' 
	if (count == 0) printf("\nSUCCESS\n");

######
div-test.img: div-test
	cp div-test div-test.img

div-test: div-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o div-test -C ${BU6502CFG}  div-test.o  ${BU6502LIB}  -m div-test.map

div-test.o: div-test.s
	$(AS) --cpu 6502 div-test.s -l div-test.lst

div-test.s: div-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 div-test.c

div-test.res: ../../../6502 div-test.img
	bash -c '../../../6502 div-test.img div-test.oimg > div-test.con 2> div-test.trc'
	cat div-test.con | grep 'SUCCESS' 
	cat div-test.con | grep 'SUCCESS' > div-test.res
	@bash -c 'res=$$(cat div-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo div-test: PASS; else echo div-test: FAIL; fi' 
	if (count == 0) printf("\nSUCCESS\n");


######
moddiv-test.img: moddiv-test
	cp moddiv-test moddiv-test.img

moddiv-test: moddiv-test.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o moddiv-test -C ${BU6502CFG}  moddiv-test.o  ${BU6502LIB}  -m moddiv-test.map

moddiv-test.o: moddiv-test.s
	$(AS) --cpu 6502 moddiv-test.s -l moddiv-test.lst

moddiv-test.s: moddiv-test.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 moddiv-test.c

moddiv-test.res: ../../../6502 moddiv-test.img
	bash -c '../../../6502 moddiv-test.img moddiv-test.oimg > moddiv-test.con 2> moddiv-test.trc'
	cat moddiv-test.con | grep 'SUCCESS'
	cat moddiv-test.con | grep 'SUCCESS' > moddiv-test.res
	@bash -c 'res=$$(cat moddiv-test.res);\
	 if [[ $$res = "SUCCESS" ]]; then echo moddiv-test: PASS; else echo moddiv-test: FAIL; fi'
	if (count == 0) printf("\nSUCCESS\n");


######

tests.img: tests
	cp tests tests.img

tests: tests.o ${BU6502CFG}  ${BU6502LIB}
	$(LD) -o tests -C ${BU6502CFG}  tests.o  ${BU6502LIB}  -m tests.map

tests.o: tests.s
	$(AS) --cpu 6502 tests.s -l tests.lst

tests.s: tests.c
	$(CC) -D__6502__ -t none -O -Oi --cpu 6502 tests.c

tests.res: ../../../6502 tests.img
	bash -c '../../../6502 tests.img tests.oimg'

######
figforth1.1a.img: figforth1.1a
	cp figforth1.1a figforth1.1a.img

figforth1.1a: figforth1.1a.o figforth1.1a.cfg
	$(LD) -o figforth1.1a -C figforth1.1a.cfg  figforth1.1a.o  -m figforth1.1a.map

figforth1.1a.o: figforth1.1a.s
	$(AS) -t none --cpu 6502 figforth1.1a.s -l figforth1.1a.lst

figforth1.1aclean:
	rm figforth1.1a.img figforth1.1a figforth1.1a.o

######

my-figforth.img: my-figforth.o figforth.cfg
	$(LD) -o $@ -C figforth.cfg $< -m my-figforth.map

my-figforth.o: my-figforth.s
	$(AS) -t none --cpu 6502 $< -l my-figforth.lst

######
figforth.img: figforth
	cp figforth figforth.img

figforth: figforth.o figforth.cfg
	$(LD) -o figforth -C figforth.cfg  figforth.o -m figforth.map

figforth.o: figforth.s
	$(AS) -t none --cpu 6502 figforth.s -l figforth.lst

figforthclean:
	rm figforth.img figforth figforth.o

######
fib-figforth.img: fib-figforth
	cp fib-figforth fib-figforth.img

fib-figforth: fib-figforth.o figforth.cfg
	$(LD) -o fib-figforth -C figforth.cfg  fib-figforth.o -m fib-figforth.map

fib-figforth.o: fib-figforth.s
	$(AS) -t none --cpu 6502 fib-figforth.s -l fib-figforth.lst

fib-figforthclean:
	rm fib-figforth.img fib-figforth fib-figforth.o


#######

fibtbl-figforth.img: fibtbl-figforth
	cp fibtbl-figforth fibtbl-figforth.img

fibtbl-figforth: fibtbl-figforth.o figforth.cfg
	$(LD) -o fibtbl-figforth -C figforth.cfg  fibtbl-figforth.o -m fibtbl-figforth.map

fibtbl-figforth.o: fibtbl-figforth.s
	$(AS) -t none --cpu 6502 fibtbl-figforth.s -l fibtbl-figforth.lst

fibtbl-figforthclean:
	rm fibtbl-figforth.img fibtbl-figforth fibtbl-figforth.o


#######

bu6502interrupt.o:
	$(AS) --cpu 6502 bu6502interrupt.s -l bu6502interrupt.lst

vectors.o:
	$(AS) --cpu 6502 vectors.s -l vectors.lst

crt0.o:
	$(AS) --cpu 6502 crt0.s -l crt0.lst

ctype.o:
	$(AS) --cpu 6502 ctype.s -l ctype.lst

cputc.o: cputc.s
	$(AS) --cpu 6502 cputc.s -l cputc.lst

cgetc.o: cgetc.s
	$(AS) --cpu 6502 cgetc.s -l cgetc.lst

oserror.o: oserror.s
	$(AS) --cpu 6502 oserror.s -l oserror.lst

#######

native: native_tests native_collatz native_fibtbl native_fib

native_tests: tests.c
	gcc tests.c -o native_tests

native_collatz: collatz.c
	gcc collatz.c -o native_collatz

native_fibtbl: fibtbl.c
	gcc fibtbl.c -o native_fibtbl

native_fib: fib.c
	gcc fib.c -o native_fib

fib-90,30.dat:
	./mkfibdata 90 30 > fib-90,30.dat 

native_fib_res: native_fib-fib-90,30.dat.res native_fibtbl-fib-90,30.dat.res
	diff native_fib-fib-90,30.dat.res native_fibtbl-fib-90,30.dat.res > native_fib_res

native_fib-fib-90,30.dat.res: native_fib fib-90,30.dat
	./native_fib < fib-90,30.dat > native_fib-fib-90,30.dat.res

native_fibtbl-fib-90,30.dat.res: native_fibtbl fib-90,30.dat
	./native_fibtbl < fib-90,30.dat > native_fibtbl-fib-90,30.dat.res


# build a base library from the cc65 sources 
bu6502.lib: ${SYSLIB} ${BU6502OBJS}
	cp ${SYSLIB} bu6502.lib
	$(AR) a bu6502.lib $(BU6502OBJS)

${SYSLIB}:
	make -C ../../../cc65lib


../../../6502:
	make -C ../../..

clean:
	-rm -rf $(wildcard) *~ *.o *.map *.img *.lst *.res *.out *.oimg *.con bu6502.lib hello.s hello collatz.s collatz tests.s tests memcpy.s memcpy concpy.s concpy loop.s loop p1.s p1 p2.s p2 p3.s p3 fib.s fib fibtbl.s fibtbl fibglb.s fibglb fibtblglb.s fibtblglb ising.s ising readtest.s readtest heaptest heaptest.o heaptest.s native* 6502.trc.bin otter otter.img otter.s otter.o figforth

distclean: clean
