LATEX = pdflatex
FILENAME = overview

all: ${FILENAME}.pdf
${FILENAME}.pdf:
	$(LATEX) ${FILENAME}.tex

clean:
	rm -f ${FILENAME}.pdf ${FILENAME}.log ${FILENAME}.aux
