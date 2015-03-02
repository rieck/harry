# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

AM_CPPFLAGS             = -I$(top_srcdir)/src 
EXTRA_DIST		= README.md

noinst_LTLIBRARIES	= libinput.la

libinput_la_SOURCES	= input.h input.c input_arc.c input_arc.h \
			  input_dir.c input_dir.h input_lines.c \
			  input_lines.h input_fasta.c input_fasta.h \
			  input_stdin.c input_stdin.h input_raw.c \
			  input_raw.h
                          
beautify:
			gindent -i4 -npsl -di0 -br -d0 -cli0 -npcs -ce -nfc1 \
			        -nut -T str_t -T gzFile -T hstring_t \
			        $(libinput_la_SOURCES)
		
