LATEX = pdflatex

DOCS = overview sd1report

all: ${DOCS}

overview:
	$(LATEX) overview.tex
	$(LATEX) overview.tex
	$(LATEX) overview.tex

sd1report:
	$(LATEX) sd1report.tex
	$(LATEX) sd1report.tex
	$(LATEX) sd1report.tex

clean:
	rm -f *.pdf *.log *.aux
