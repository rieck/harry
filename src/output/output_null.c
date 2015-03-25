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
 * @addtogroup output
 * <hr>
 * <em>null</em>: Null output for benchmarking
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "output.h"
#include "harry.h"


/* External variables */
extern config_t cfg;


/**
 * Opens a file for writing null format
 * @param fn File name
 * @return number of regular files
 */
int output_null_open(char *fn)
{
    assert(fn);
    return TRUE;
}

/**
 * Write similarity matrix to output
 * @param m Matrix of similarity values 
 * @return Number of written values
 */
int output_null_write(hmatrix_t *m)
{
    return m->size;
}

/**
 * Closes an open output file.
 */
void output_null_close()
{
    /* Do nothing */
}

/** @} */
