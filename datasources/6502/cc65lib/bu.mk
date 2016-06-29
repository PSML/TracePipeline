override TARGETS := none

include ../../ext/cc65/libsrc/Makefile
.DEFAULT_GOAL :=

AR65 := ../../ext/install/bin/ar65
CA65 := ../../ext/install/bin/ca65
CC65 := ../../ext/install/bin/cc65
CO65 := ../../ext/install/bin/co65
LD65 := ../../ext/install/bin/ld65

export CC65_HOME := $(abspath ../../ext/install)

.PHONY: prep 

prep: ${CC65}
	cp -r ../../ext/cc65/libsrc/* .

${CC65}: 
	make -C ../../ext
	
