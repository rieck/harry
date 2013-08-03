# Harry - Similarity Measures for Strings
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

SUBDIRS		     = measures
AM_CPPFLAGS          = -I$(srcdir)/measures

bin_PROGRAMS         = harry
harry_SOURCES        = harry.c harry.h
harry_LDADD          = libharry.la
harry_DEPENDENCIES   = libharry.la

noinst_LTLIBRARIES   = libharry.la
libharry_la_SOURCES  = common.h util.c util.h default.c default.h
libharry_la_LIBADD   = measures/libmeasures.la

beautify:
		gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 -nut \
		-T FILE *.c *.h
