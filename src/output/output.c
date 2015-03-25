/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

/** 
 * @defgroup output Output interface
 *
 * Interface and functions for writing matrices of similarity values
 *
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "output.h"

/* Modules */
#include "output_text.h"
#include "output_libsvm.h"
#include "output_stdout.h"
#include "output_null.h"
#include "output_json.h"
#include "output_matlab.h"
#include "output_raw.h"

/**
 * Structure for output interface
 */
typedef struct
{
    int (*output_open) (char *);
    int (*output_write) (hmatrix_t *);
    void (*output_close) (void);
} output_t;
static output_t func;

/** 
 * Configure the output of Harry
 * @param format Name of output format
 */
void output_config(const char *format)
{

    if (!strcasecmp(format, "text")) {
        func.output_open = output_text_open;
        func.output_write = output_text_write;
        func.output_close = output_text_close;
    } else if (!strcasecmp(format, "stdout")) {
        func.output_open = output_stdout_open;
        func.output_write = output_stdout_write;
        func.output_close = output_stdout_close;
    } else if (!strcasecmp(format, "libsvm")) {
        func.output_open = output_libsvm_open;
        func.output_write = output_libsvm_write;
        func.output_close = output_libsvm_close;
    } else if (!strcasecmp(format, "null")) {
        func.output_open = output_null_open;
        func.output_write = output_null_write;
        func.output_close = output_null_close;
    } else if (!strcasecmp(format, "json")) {
        func.output_open = output_json_open;
        func.output_write = output_json_write;
        func.output_close = output_json_close;
    } else if (!strcasecmp(format, "matlab")) {
        func.output_open = output_matlab_open;
        func.output_write = output_matlab_write;
        func.output_close = output_matlab_close;
    } else if (!strcasecmp(format, "raw")) {
        func.output_open = output_raw_open;
        func.output_write = output_raw_write;
        func.output_close = output_raw_close;
    } else {
        error("Unknown ouptut format '%s', using 'text' instead.", format);
        output_config("text");
    }
}

/**
 * Wrapper for opening the output desctination.
 * @param name Name of output destination, e.g., directory or file name
 * @return 1 on success, 0 otherwise.
 */
int output_open(char *name)
{
    return func.output_open(name);
}

/**
 * Wrapper for writing a block to the output destination.
 * @param m Matrix of similarity values 
 * @return Number of written values
 */
int output_write(hmatrix_t *m)
{
    return func.output_write(m);
}

/**
 * Wrapper for closing the output destination. 
 */
void output_close(void)
{
    func.output_close();
}

/** @} */
