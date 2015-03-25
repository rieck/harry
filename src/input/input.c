/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org);
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details.
 */

/**
 * @defgroup input Input interface
 *
 * Interface and functions for reading strings.
 *
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"

/* Modules */
#include "input_arc.h"
#include "input_dir.h"
#include "input_lines.h"
#include "input_fasta.h"
#include "input_stdin.h"
#include "input_raw.h"

/* Other stuff */
#include "uthash.h"

/**
 * Structure for input interface
 */
typedef struct
{
    int (*input_open) (char *);
    int (*input_read) (hstring_t *, int);
    void (*input_close) (void);
} input_t;
static input_t func;

/**< Delimiter table */
extern char delim[256];
/** External variables */
extern config_t cfg;

/**
 * Configure the input of Harry
 * @param format Name of input format
 */
void input_config(const char *format)
{
    if (!strcasecmp(format, "dir")) {
        func.input_open = input_dir_open;
        func.input_read = input_dir_read;
        func.input_close = input_dir_close;
    } else if (!strcasecmp(format, "lines")) {
        func.input_open = input_lines_open;
        func.input_read = input_lines_read;
        func.input_close = input_lines_close;
    } else if (!strcasecmp(format, "fasta")) {
        func.input_open = input_fasta_open;
        func.input_read = input_fasta_read;
        func.input_close = input_fasta_close;
    } else if (!strcasecmp(format, "stdin")) {
        func.input_open = input_stdin_open;
        func.input_read = input_stdin_read;
        func.input_close = input_stdin_close;
    } else if (!strcasecmp(format, "raw")) {
        func.input_open = input_raw_open;
        func.input_read = input_raw_read;
        func.input_close = input_raw_close;
    } else if (!strcasecmp(format, "arc")) {
#ifdef HAVE_LIBARCHIVE
        func.input_open = input_arc_open;
        func.input_read = input_arc_read;
        func.input_close = input_arc_close;
#else
        error("Harry has been compiled without support for libarchive.");
#endif
    } else {
        error("Unknown input format '%s', using 'lines' instead.", format);
        input_config("lines");
    }
}

/**
 * Wrapper for opening the input source.
 * @param name Name of input source, e.g., directory or file name
 * @return Number of available entries or -1 on error
 */
int input_open(char *name)
{
    return func.input_open(name);
}

/**
 * Wrapper for reading a block from the input source.
 * @param strs Allocated array for string data
 * @param len Length of allocated arrays
 * @return Number of read strings
 */
int input_read(hstring_t *strs, int len)
{
    return func.input_read(strs, len);
}

/**
 * Wrapper for closing the input source.
 */
void input_close(void)
{
    func.input_close();
}

/**
 * Free a chunk of input strings
 */
void input_free(hstring_t *strs, int len)
{
    assert(strs);

    int j;
    for (j = 0; j < len; j++)
        hstring_destroy(&strs[j]);
}

/** @} */
