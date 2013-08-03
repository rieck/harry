# Harry - Similarity Measures for Strings
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

EXTRA_DIST = harry.pod doxygen.cfg example.cfg 

man1_MANS = harry.man
doc_DATA = example.cfg

harry.man: harry.pod
	pod2man -c "User Manual" -s 1 -r "Harry $(VERSION)" $< > $@

distclean-local:
	rm -f harry.man 
