# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

EXTRA_DIST            = harry.py.in gen_options.py

python_PYTHON 	      = harry.py
noinst_PYTHON 	      = benchmark.py

harry.py: harry.py.in gen_options.py $(top_srcdir)/src/options.txt
	$(PYTHON) gen_options.py $(top_srcdir)/src/options.txt \
	          harry.py $(bindir)
