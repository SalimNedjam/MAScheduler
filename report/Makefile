all: report biblio

report: report.tex
	pdflatex report

biblio: bibliography.bib report.aux
	bibtex report
	pdflatex report

clean:
	rm -f *.aux *.log *.pdf *.bbl *.blg *.toc *.dvi *.synctex.gz
