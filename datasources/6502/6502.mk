archdep = arch/$(arch)/lib6502.a

arch/$(arch)/lib6502.a:
	make -C arch/$(arch) lib6502.a