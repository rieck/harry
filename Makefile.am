# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src tests doc .
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = README.md COPYING CHANGES harry.png TODO

clean-local:
	rm -f *~

CHANGES:
	$(srcdir)/git2changes.py
	        
distclean-local:
	rm -f CHANGES

dist-hook: 

	
