# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
  
AM_CPPFLAGS     		= -I$(top_srcdir)/src \
		          	  -I$(top_srcdir)/src/measures 

#EXTRA_DIST			= strings.txt
#TESTS_ENVIRONMENT 		= export TEST_FILE='$(srcdir)/strings.txt';
check_PROGRAMS			= check_levenshtein \
				  check_hamming \
				  check_jarowinkler \
				  check_lee \
				  check_wdegree \
				  check_damerau \
				  check_vcache
noinst_PROGRAMS			= $(check_PROGRAMS)
TESTS				= $(check_PROGRAMS)

check_levenshtein_SOURCES	= dist_levenshtein.c tests.h
check_levenshtein_LDADD		= $(top_builddir)/src/libharry.la

check_hamming_SOURCES		= dist_hamming.c tests.h
check_hamming_LDADD		= $(top_builddir)/src/libharry.la

check_jarowinkler_SOURCES	= dist_jarowinkler.c tests.h
check_jarowinkler_LDADD		= $(top_builddir)/src/libharry.la

check_lee_SOURCES		= dist_lee.c tests.h
check_lee_LDADD			= $(top_builddir)/src/libharry.la

check_wdegree_SOURCES		= kern_wdegree.c tests.h
check_wdegree_LDADD		= $(top_builddir)/src/libharry.la

check_damerau_SOURCES		= dist_damerau.c tests.h
check_damerau_LDADD		= $(top_builddir)/src/libharry.la

check_vcache_SOURCES		= vcache.c tests.h
check_vcache_LDADD		= $(top_builddir)/src/libharry.la

beautify:
		gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 -nut \
		-T FILE *.c 
