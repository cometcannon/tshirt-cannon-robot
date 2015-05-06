LATEX = pdflatex

DOCS = overview sd2-report

all: ${DOCS}

overview:
	$(LATEX) overview.tex
	$(LATEX) overview.tex

sd2-report:
	$(LATEX) sd2-report.tex
	$(LATEX) sd2-report.tex

clean:
	rm -f *.pdf *.log *.aux
