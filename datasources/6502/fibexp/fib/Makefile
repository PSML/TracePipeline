EXP = fib
BIN = ~/6502/fibexp/bin
BLOCK = 65544

targets = $(EXP).diff1.hist.pdf $(EXP).io.pdf

all: $(targets)
.PHONY: all

$(EXP).diff0: $(EXP).img
	$(BIN)/mk6502trace $(EXP) $< $@

$(EXP).diff1: $(EXP).diff0
	$(BIN)/bdiff $(BLOCK) $< $@

$(EXP)_ALL.io: $(EXP).diff1
	$(BIN)/mkio $(EXP) $< $@

$(EXP)_ALL.pc: $(EXP).diff1
	$(BIN)/mkreg $(EXP) pc $< $@

$(EXP)_ALL.global: $(EXP).diff1
	$(BIN)/mkaddr $(EXP) 0x02C0 4 $< $@

$(EXP).diff0_ALL.md5: $(EXP).diff0
	$(BIN)/mk6502md5 $< > $@
	$(BIN)/verifymd5 $@ $< $(BLOCK)

$(EXP).diff1_ALL.md5: $(EXP).diff1
	$(BIN)/mk6502md5 $< > $@
	$(BIN)/verifymd5 $@ $< $(BLOCK)

$(EXP)_ALL.io.md5: $(EXP)_ALL.io $(EXP).diff1_ALL.md5
	$(BIN)/mk6502md5 $<.idx $(word 2,$^) > $@

$(EXP)_ALL.pc.md5: $(EXP)_ALL.pc $(EXP).diff1_ALL.md5
	$(BIN)/mk6502md5 $<.idx $(word 2,$^) > $@

$(EXP)_ALL.global.md5: $(EXP)_ALL.global $(EXP).diff1_ALL.md5
	$(BIN)/mk6502md5 $<.idx $(word 2,$^) > $@

$(EXP).diff0_ALL.uniq: $(EXP).diff0_ALL.md5
	cat $< | $(BIN)/mkuniq > $@

$(EXP).diff1_ALL.uniq: $(EXP).diff1_ALL.md5
	cat $< | $(BIN)/mkuniq > $@

$(EXP).diff0_ALL.hist: $(EXP).diff0_ALL.md5
	cat $< | $(BIN)/mkhist > $@

$(EXP).diff1_ALL.hist: $(EXP).diff1_ALL.md5
	cat $< | $(BIN)/mkhist > $@

$(EXP).diff1_ALL.histcnt: $(EXP).diff1_ALL.hist
	cut -d' ' -f1 $< > $@

$(EXP).diff1.hist.pdf: $(EXP).diff1_ALL.histcnt
	$(BIN)/plthist $(EXP) $< $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

$(EXP).diff1_ALL.histmapped: $(EXP).diff1_ALL.md5 $(EXP).diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

$(EXP)_ALL.io.histmapped: $(EXP)_ALL.io.md5 $(EXP).diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

$(EXP)_ALL.global.histmapped: $(EXP)_ALL.global.md5 $(EXP).diff1_ALL.hist 
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

$(EXP).io.pdf: $(EXP).diff1_ALL.histmapped $(EXP)_ALL.io.histmapped
	$(BIN)/pltio $(EXP) $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

$(EXP).io_global.pdf: $(EXP).diff1_ALL.histmapped $(EXP)_ALL.io.histmapped $(EXP)_ALL.global.histmapped
	$(BIN)/pltio_global $(EXP) $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

clean:
	rm -rf time.info $(EXP).oimg $(EXP).stdout $(EXP).stderr *.diff0 *.diff1 *.io *.pc *.global *.idx *.info *.svnums *.md5 *.uniq *.hist *.histcnt *.histmapped *.pdf
