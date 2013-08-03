# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS             = -I$(top_srcdir)/src \
			  -I$(top_srcdir)/src/fvec
			  
EXTRA_DIST		= README.md

noinst_LTLIBRARIES	= liboutput.la

liboutput_la_SOURCES	= output.c output.h \
                          output_text.c output_text.h 

beautify:
			gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 -nut \
				-T string_t -T gzFile -T sm_t -T FILE \
				$(liboutput_la_SOURCES)                          
