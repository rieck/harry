/*
 * Harry - Similarity Measures for Strings
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
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
 * Generic implementation of functions for writing matrices of 
 * similarity/dissimilarity values.
 *
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "output.h"

/* Modules */
#include "output_text.h"

/**
 * Structure for output interface
 */
typedef struct
{
    int (*output_open) (char *);
    int (*output_write) (float *, int, int, int);
    void (*output_close) (void);
} func_t;
static func_t func;

/** 
 * Configure the output of Sally
 * @param format Name of output format
 */
void output_config(const char *format)
{

    if (!strcasecmp(format, "text")) {
        func.output_open = output_text_open;
        func.output_write = output_text_write;
        func.output_close = output_text_close;
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
 * @param x Dimension of matrix
 * @param y Dimension of matrix
 * @param t Set to 1 if only upper triangle given
 * @return Number of written values
 */
int output_write(float *m, int x, int y, int t)
{
    return func.output_write(m, x, y, t);
}

/**
 * Wrapper for closing the output destination. 
 */
void output_close(void)
{
    func.output_close();
}

/** @} */
