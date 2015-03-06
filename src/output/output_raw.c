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
 * | xdim (uint32) | ydim (uint32) | array (float) ... |
 * </pre>
 * where xdim and ydim are unsigned 32-bit integers in host byte order
 * specifing the dimensions of the matrix and array holds the matrix
 * as single floats (32 bit).
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

/* Local variables */
static cfg_int precision = 0;

/**
 * Opens a file for writing raw format
 * @param fn File name (bogus)
 * @return true on sucess, false otherwise
 */
int output_raw_open(char *fn)
{
    config_lookup_int(&cfg, "output.precision", &precision);

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
    uint32_t ret, xdim, ydim, i, j;

    xdim = m->x.n - m->x.i;
    ydim = m->y.n - m->y.i;

    ret = fwrite(&xdim, sizeof(xdim), 1, stdout);
    ret += fwrite(&ydim, sizeof(ydim), 1, stdout);
    if (ret != 2) {
        error("Failed to write raw matrix header to stdout");
        return 0;
    }

    for (i = m->y.i; i < m->y.n; i++) {
        for (j = m->x.i; j < m->x.n; j++) {
            float val = hround(hmatrix_get(m, j, i), precision);
            ret = fwrite(&val, sizeof(float), 1, stdout);
            if (ret != 1) {
                error("Failed to write raw matrix data to stdout");
                return ret;
            }
        }
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
