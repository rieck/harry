# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

EXTRA_DIST            = harry.py.in gen_options.py

python_PYTHON 	      = harry.py
noinst_PYTHON 	      = benchmark.py
CLEANFILES            = harry.py

harry.py: harry.py.in gen_options.py $(top_srcdir)/src/options.txt
	$(PYTHON) $(srcdir)/gen_options.py $(top_srcdir)/src/options.txt \
	          $(bindir) $(srcdir)/harry.py.in $(builddir)/harry.py

# Make sure harry.py is available during test runs
all-local: harry.py

# Clean compiled code
distclean-local:
	rm -f $(builddir)/harry.pyc

# Make sure harry.py is removed from the distribution
dist-hook:
	rm -f $(distdir)/harry.py
	
