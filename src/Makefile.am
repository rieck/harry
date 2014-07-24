# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

SUBDIRS		     = 	measures input output
AM_CPPFLAGS          = 	-I$(srcdir)/measures -I$(srcdir)/input \
		       	-I$(srcdir)/output

bin_PROGRAMS         = 	harry
harry_SOURCES        = 	harry.c harry.h
harry_LDADD          = 	libharry.la
harry_DEPENDENCIES   = 	libharry.la

noinst_LTLIBRARIES   = 	libharry.la
libharry_la_SOURCES  = 	common.h util.c util.h hconfig.c hconfig.h \
                        md5.c md5.h murmur.c murmur.h hstring.c hstring.h \
                        vcache.c vcache.h uthash.h rwlock.c rwlock.h \
                        hmatrix.c hmatrix.h
libharry_la_LIBADD   =  input/libinput.la measures/libmeasures.la \
		       	output/liboutput.la
beautify:
			gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce \
			        -nfc1 -nut -T FILE -T hstring_t -T rwlock_t \
			        -T hmatrix_t *.c *.h
