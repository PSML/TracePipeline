.phony: all
all: grades.txt

grades.txt: grades/report.txt
	cp grades/report.txt grades.txt

handin:
	./gethandins handin

grades/report.txt: handin
	mkdir grades
	./grade handin grades

clean:
	rm -rf grades grades.txt