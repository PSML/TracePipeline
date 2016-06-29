How to build the fig-forth for the Apple II 080207 dkrueger

Install cc65 and a2tools.

Console:

ca65 figforth.s -l

ld65 -C apple2x.cfg figforth.o -o figforth.bin

a2tools in -r b MASTER.DSK FIGFORTH figforth.bin