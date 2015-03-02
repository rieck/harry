/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
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
 * <em>raw</em>: The matrix of similarity/dissimilarity measures is written
 * to standard output in raw format.
 *
 * This module is designed for efficiently interfacing with other
 * environments.  The raw format of a similarity matrix has the form
 * <pre>
 * | xdim (uint32) | ydim (uint32) | len (uint32) | array (float) ... |
 * </pre>
 * where xdim and ydim are unsigned 32-bit integers in host byte order
 * specifing the dimensions of the matrix, and len is an unsigned 32-bit
 * integer indicating the length of the following array in single
 * floats (32 bit).
 *
 * Note that if len != xlen * ylen the matrix is stored in triangular form,
 * that is, only the upper triangle is given.
 *
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
 * Opens a file for writing raw format
 * @param fn File name (bogus)
 * @return true on sucess, false otherwise
 */
int output_raw_open(char *fn)
{
    if (!stdout) {
        error("Could not open <stdout>");
        return FALSE;
    }

    return TRUE;
}

/**
 * Write similarity matrux to output
 * @param m Matrix of similarity values
 * @return Number of written values
 */
int output_raw_write(hmatrix_t *m)
{
    assert(m);
    uint32_t ret, xdim, ydim, len;

    xdim = m->x.n - m->x.i;
    ydim = m->y.n - m->y.i;
    len = m->size / sizeof(float);

    ret = fwrite(&xdim, sizeof(xdim), 1, stdout);
    ret += fwrite(&ydim, sizeof(ydim), 1, stdout);
    ret += fwrite(&len, sizeof(len), 1, stdout);
    if (ret != 3) {
        error("Failed to write raw matrix header to stdout");
        return 0;
    }

    ret = fwrite(m->values, sizeof(float), len, stdout);
    if (ret != len) {
        error("Failed to write raw matrix data to stdout");
        return ret;
    }

    return ret;
}

/**
 * Closes an open output file.
 */
void output_raw_close()
{
    /* Do nothing */
}

/** @} */
