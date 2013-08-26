# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS          	   = -I$(srcdir)/.. -I$(srcdir)/../input

noinst_LTLIBRARIES         = libmeasures.la

libmeasures_la_SOURCES     = measures.h measures.c \
			     dist_hamming.c dist_hamming.h \
			     dist_levenshtein.c dist_levenshtein.h \
			     dist_jarowinkler.c dist_jarowinkler.h \
			     dist_lee.c dist_lee.h \
			     kern_wdegree.c kern_wdegree.h \
			     dist_damerau.c dist_damerau.h \
			     kern_subsequence.c kern_subsequence.h \
			     dist_compression.c	dist_compression.h

beautify:
			   gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce \
			   	   -nfc1 -nut -T string_t -T FILE *.c *.h
