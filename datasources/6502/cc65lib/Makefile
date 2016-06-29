.PHONY: all clean

all:    libsrc wrk/none lib libsrc/bu.mk
	make -C libsrc -f bu.mk prep
	make -C libsrc -f bu.mk none

libsrc:
	mkdir -p libsrc

wrk/none:
	mkdir -p wrk/none

lib:
	mkdir -p lib

libsrc/bu.mk: bu.mk
	cp bu.mk libsrc/bu.mk

clean:
	-rm -rf libsrc wrk lib
