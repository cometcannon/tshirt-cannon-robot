LATEX = pdflatex

DOCS = overview sd1report proposal

all: ${DOCS}

overview:
	$(LATEX) overview.tex
	$(LATEX) overview.tex
	$(LATEX) overview.tex

sd1report:
	$(LATEX) sd1report.tex
	$(LATEX) sd1report.tex
	$(LATEX) sd1report.tex

proposal:
	$(LATEX) proposal.tex
	$(LATEX) proposal.tex
	$(LATEX) proposal.tex


clean:
	rm -f *.pdf *.log *.aux
