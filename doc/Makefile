TARGETS = clr.pdf

all: $(TARGETS)

clean:
	rm -f *.aux */*.aux *.log *.out *.toc $(TARGETS)

clr.pdf: clr.tex preamble.tex
	xelatex $<

#	xelatex $< && rm $@ && xelatex $<
