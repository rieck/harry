# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

EXTRA_DIST = harry.pod doxygen.cfg example.cfg harry.txt harry.pdf \
	     latex-pre.inc.in latex-post.inc README.md
	
man1_MANS = harry.man
doc_DATA = example.cfg

harry.man: harry.pod
	pod2man -c "User Manual" -s 1 -r "Harry $(VERSION)" $< > $@

harry.txt: harry.pod
	pod2text -w 70 harry.pod > $@

harry.pdf: harry.pod latex-pre.inc latex-post.inc
	pod2latex -prefile $(top_builddir)/doc/latex-pre.inc \
	          -postfile $(top_srcdir)/doc/latex-post.inc \
	          -full -out $(top_builddir)/doc/harry.tex \
	          $(top_srcdir)/doc/harry.pod
	pdflatex $(top_builddir)/doc/harry.tex
	pdflatex $(top_builddir)/doc/harry.tex	
	pdflatex $(top_builddir)/doc/harry.tex	
	rm -f *.tex *.log *.aux *.toc

latex-pre.inc: latex-pre.inc.in
	sed -e 's|__VERSION__|$(VERSION)|' \
		$(top_srcdir)/doc/latex-pre.inc.in \
		> $(top_builddir)/doc/latex-pre.inc

distclean-local:
	rm -f harry.man harry.txt harry.pdf latex-pre.inc
		
dist-hook: distclean-local harry.man harry.txt harry.pdf 
