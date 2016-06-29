BIN = ~/6502/fibexp/bin

targets = fib_fibtbl.io.pdf fib_fibglb.io.pdf fibtbl_fibtblglb.io.pdf fibtbl_shift.io.pdf fibtbl_shift.io_mismatch.pdf fibtbl_shift.io_diff.pdf

all: $(targets)
.PHONY: all

fibtbl_shift.diff1_ALL.mismatch: fibtbl/fibtbl.diff1_ALL.hist fibtbl_shift/fibtbl.diff1_ALL.hist fibtbl/fibtbl.diff1_ALL.md5 fibtbl_shift/fibtbl.diff1_ALL.md5
	$(BIN)/mkmismatch fibtbl shift $^ $@

fibtbl.fibtbl_shift.diff1_ALL.mismatch.md5: fibtbl_shift.diff1_ALL.mismatch fibtbl/fibtbl.diff1_ALL.md5
	$(BIN)/mk6502md5 fibtbl.$<.idx $(word 2,$^) > $@

shift.fibtbl_shift.diff1_ALL.mismatch.md5: fibtbl_shift.diff1_ALL.mismatch fibtbl_shift/fibtbl.diff1_ALL.md5
	$(BIN)/mk6502md5 shift.$<.idx $(word 2,$^) > $@

fibtbl_shift.diff1_ALL.diff.idx: fibtbl/fibtbl.diff1_ALL.md5 fibtbl_shift/fibtbl.diff1_ALL.md5
	diff $^ | grep '^< ' | cut -d' ' -f2 > fibtbl.$@
	diff $^ | grep '^> ' | cut -d' ' -f2 > shift.$@

fibtbl.fibtbl_shift.diff1_ALL.diff.md5: fibtbl_shift.diff1_ALL.diff.idx fibtbl/fibtbl.diff1_ALL.md5
	$(BIN)/mk6502md5 fibtbl.$< $(word 2,$^) > $@

shift.fibtbl_shift.diff1_ALL.diff.md5: fibtbl_shift.diff1_ALL.diff.idx fibtbl_shift/fibtbl.diff1_ALL.md5
	$(BIN)/mk6502md5 shift.$< $(word 2,$^) > $@

fib_fibtbl.diff1_ALL.hist: fib/fib.diff1_ALL.md5 fibtbl/fibtbl.diff1_ALL.md5
	cat $^ | $(BIN)/mkhist > $@

fib_fibglb.diff1_ALL.hist: fib/fib.diff1_ALL.md5 fibglb/fibglb.diff1_ALL.md5
	cat $^ | $(BIN)/mkhist > $@

fibtbl_fibtblglb.diff1_ALL.hist: fibtbl/fibtbl.diff1_ALL.md5 fibtblglb/fibtblglb.diff1_ALL.md5
	cat $^ | $(BIN)/mkhist > $@

fibtbl_shift.diff1_ALL.hist: fibtbl/fibtbl.diff1_ALL.md5 fibtbl_shift/fibtbl.diff1_ALL.md5
	cat $^ | $(BIN)/mkhist > $@

fib.fib_fibtbl.diff1_ALL.histmapped: fib/fib.diff1_ALL.md5 fib_fibtbl.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fibtbl.fib_fibtbl.diff1_ALL.histmapped: fibtbl/fibtbl.diff1_ALL.md5 fib_fibtbl.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fib.fib_fibglb.diff1_ALL.histmapped: fib/fib.diff1_ALL.md5 fib_fibglb.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fibglb.fib_fibglb.diff1_ALL.histmapped: fibglb/fibglb.diff1_ALL.md5 fib_fibglb.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fibtbl.fibtbl_fibtblglb.diff1_ALL.histmapped: fibtbl/fibtbl.diff1_ALL.md5 fibtbl_fibtblglb.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fibtblglb.fibtbl_fibtblglb.diff1_ALL.histmapped: fibtblglb/fibtblglb.diff1_ALL.md5 fibtbl_fibtblglb.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fibtbl.fibtbl_shift.diff1_ALL.histmapped: fibtbl/fibtbl.diff1_ALL.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

shift.fibtbl_shift.diff1_ALL.histmapped: fibtbl_shift/fibtbl.diff1_ALL.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmap $(word 2,$^) > $@

fib_io.fib_fibtbl.ALL.histmapped: fib/fib_ALL.io.md5 fib_fibtbl.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtbl_io.fib_fibtbl.ALL.histmapped: fibtbl/fibtbl_ALL.io.md5 fib_fibtbl.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fib_io.fib_fibglb.ALL.histmapped: fib/fib_ALL.io.md5 fib_fibglb.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibglb_io.fib_fibglb.ALL.histmapped: fibglb/fibglb_ALL.io.md5 fib_fibglb.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtbl_io.fibtbl_fibtblglb.ALL.histmapped: fibtbl/fibtbl_ALL.io.md5 fibtbl_fibtblglb.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtblglb_io.fibtbl_fibtblglb.ALL.histmapped: fibtblglb/fibtblglb_ALL.io.md5 fibtbl_fibtblglb.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtbl_io.fibtbl_shift.ALL.histmapped: fibtbl/fibtbl_ALL.io.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

shift_io.fibtbl_shift.ALL.histmapped: fibtbl_shift/fibtbl_ALL.io.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtbl_mismatch.fibtbl_shift.ALL.histmapped: fibtbl.fibtbl_shift.diff1_ALL.mismatch.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

shift_mismatch.fibtbl_shift.ALL.histmapped: shift.fibtbl_shift.diff1_ALL.mismatch.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fibtbl_diff.fibtbl_shift.ALL.histmapped: fibtbl.fibtbl_shift.diff1_ALL.diff.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

shift_diff.fibtbl_shift.ALL.histmapped: shift.fibtbl_shift.diff1_ALL.diff.md5 fibtbl_shift.diff1_ALL.hist
	cat $< | $(BIN)/histmapinfo $(word 2,$^) > $@

fib_fibtbl.io.pdf: fib.fib_fibtbl.diff1_ALL.histmapped fibtbl.fib_fibtbl.diff1_ALL.histmapped fib_io.fib_fibtbl.ALL.histmapped fibtbl_io.fib_fibtbl.ALL.histmapped
	$(BIN)/pltio2 fib fibtbl $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fib_fibglb.io.pdf: fib.fib_fibglb.diff1_ALL.histmapped fibglb.fib_fibglb.diff1_ALL.histmapped fib_io.fib_fibglb.ALL.histmapped fibglb_io.fib_fibglb.ALL.histmapped
	$(BIN)/pltio2 fib fibglb $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fibtbl_fibtblglb.io.pdf: fibtbl.fibtbl_fibtblglb.diff1_ALL.histmapped fibtblglb.fibtbl_fibtblglb.diff1_ALL.histmapped fibtbl_io.fibtbl_fibtblglb.ALL.histmapped fibtblglb_io.fibtbl_fibtblglb.ALL.histmapped
	$(BIN)/pltio2 fibtbl fibtblglb $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fibtbl_shift.io.pdf: fibtbl.fibtbl_shift.diff1_ALL.histmapped shift.fibtbl_shift.diff1_ALL.histmapped fibtbl_io.fibtbl_shift.ALL.histmapped shift_io.fibtbl_shift.ALL.histmapped
	$(BIN)/pltio2 fibtbl shift $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fibtbl_shift.io_mismatch.pdf: fibtbl.fibtbl_shift.diff1_ALL.histmapped shift.fibtbl_shift.diff1_ALL.histmapped fibtbl_io.fibtbl_shift.ALL.histmapped shift_io.fibtbl_shift.ALL.histmapped fibtbl_mismatch.fibtbl_shift.ALL.histmapped shift_mismatch.fibtbl_shift.ALL.histmapped
	$(BIN)/pltio2_mismatch fibtbl shift $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fibtbl_shift.io_diff.pdf: fibtbl.fibtbl_shift.diff1_ALL.histmapped shift.fibtbl_shift.diff1_ALL.histmapped fibtbl_io.fibtbl_shift.ALL.histmapped shift_io.fibtbl_shift.ALL.histmapped fibtbl_diff.fibtbl_shift.ALL.histmapped shift_diff.fibtbl_shift.ALL.histmapped fibtbl_mismatch.fibtbl_shift.ALL.histmapped shift_mismatch.fibtbl_shift.ALL.histmapped
	$(BIN)/pltio2_diff fibtbl shift $^ $@ > $@.oct
	octave $@.oct > /dev/null 2> /dev/null

fib_fibtbl.diff1_ALL.uniq: fib/fib.diff1_ALL.md5 fibtbl/fibtbl.diff1_ALL.md5
	cat $^ | $(BIN)/mkuniq > $@

fib_fibglb.diff1_ALL.uniq: fib/fib.diff1_ALL.md5 fibglb/fibglb.diff1_ALL.md5
	cat $^ | $(BIN)/mkuniq > $@

fibtbl_fibtblglb.diff1_ALL.uniq: fibtbl/fibtbl.diff1_ALL.md5 fibtblglb/fibtblglb.diff1_ALL.md5
	cat $^ | $(BIN)/mkuniq > $@

fibtbl_shift.diff1_ALL.uniq: fibtbl/fibtbl.diff1_ALL.md5 fibtbl_shift/fibtbl.diff1_ALL.md5
	cat $^ | $(BIN)/mkuniq > $@

fib_fibtbl.diff0_ALL.dup: fib/fib.diff0_ALL.uniq fibtbl/fibtbl.diff0_ALL.uniq
	cat $^ | $(BIN)/mkdup > $@

fib_fibtbl.diff1_ALL.dup: fib/fib.diff1_ALL.uniq fibtbl/fibtbl.diff1_ALL.uniq
	cat $^ | $(BIN)/mkdup > $@

clean:
	rm -rf *.mismatch *.idx *.md5 *.hist *.histmapped *.oct *.pdf *.uniq *.dup
