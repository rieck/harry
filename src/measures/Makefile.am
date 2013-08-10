# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS          	   = -I$(srcdir)/.. -I$(srcdir)/../input

noinst_LTLIBRARIES         = libmeasures.la

libmeasures_la_SOURCES     = measures.h measures.c \
			     dist_hamming.c dist_hamming.h \
			     dist_levenshtein.c dist_levenshtein.h

beautify:
			   gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce \
			   	   -nfc1 -nut -T string_t -T FILE *.c *.h
