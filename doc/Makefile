LATEX = pdflatex

DOCS = overview sd2report

all: ${DOCS}

overview:
	$(LATEX) overview.tex
	$(LATEX) overview.tex
	$(LATEX) overview.tex

sd2report:
	$(LATEX) sd2report.tex
	$(LATEX) sd2report.tex
	$(LATEX) sd2report.tex

clean:
	rm -f *.pdf *.log *.aux
