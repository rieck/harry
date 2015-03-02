# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS             = -I$(top_srcdir)/src \
			  -I$(top_srcdir)/src/fvec
			  
EXTRA_DIST		= README.md

noinst_LTLIBRARIES	= liboutput.la

liboutput_la_SOURCES	= output.c output.h output_stdout.c output_stdout.h \
                          output_text.c output_text.h output_null.c \
                          output_null.h output_libsvm.c output_libsvm.h \
                          output_json.c output_json.h output_matlab.c \
                          output_matlab.h output_raw.c output_raw.h

beautify:
			gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 -nut \
				-T string_t -T gzFile -T sm_t -T FILE \
				-T hmatrix_t $(liboutput_la_SOURCES)
