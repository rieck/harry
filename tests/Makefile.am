# SIMONE - Similarity Measures for Structured Data
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS     	= -I$(top_srcdir)/src \
		          -I$(top_srcdir)/src/modules \
                  	  -I$(top_srcdir)/src/modules/strings
EXTRA_DIST		= strings.txt
TESTS_ENVIRONMENT 	= export TEST_FILE='$(srcdir)/strings.txt';
check_PROGRAMS		= check_hamming check_edit check_subseq
noinst_PROGRAMS		= $(check_PROGRAMS)
TESTS			= $(check_PROGRAMS)

check_hamming_SOURCES	= dist_hamming.c tests.h
check_hamming_LDADD	= $(top_builddir)/src/libsimone.la

check_edit_SOURCES	= dist_edit.c tests.h
check_edit_LDADD	= $(top_builddir)/src/libsimone.la

check_subseq_SOURCES	= kern_subseq.c tests.h
check_subseq_LDADD	= $(top_builddir)/src/libsimone.la

beautify:
		gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 -nut \
		-T FILE *.c 
