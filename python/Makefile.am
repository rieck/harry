# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

EXTRA_DIST           = harry.py.in

python_PYTHON 	      = harry.py
noinst_PYTHON 	      = benchmark.py

harry.py: harry.py.in
	$(SED) "s|__BINDIR__|@bindir@|" $< >$@
