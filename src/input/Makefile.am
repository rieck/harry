# Harry - Similarity Measures for Strings
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS             = -I$(top_srcdir)/src 
EXTRA_DIST		= README.md

noinst_LTLIBRARIES	= libinput.la

libinput_la_SOURCES	= input.h input.c input_arc.c input_arc.h \
			  input_dir.c input_dir.h input_lines.c input_lines.h \
                          input_fasta.c input_fasta.h 
                          
beautify:
			gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 \
			        -nut -T string_t -T gzFile $(libinput_la_SOURCES)
		