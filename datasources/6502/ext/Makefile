.PHONY: all
all: install

export prefix:=$(shell pwd)/install

install: cc65/Makefile
	make -C cc65
	make -C cc65 install

cc65/Makefile:
	git submodule init
	git submodule update

clean:
	-rm -rf install
	make -C cc65 $@

%::
	make -C cc65 $@
