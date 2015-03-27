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
 * <em>raw</em>: The matrix of similarity/dissimilarity measures is written
 * to standard output in raw format.
 *
 * This module is designed for efficiently interfacing with other
 * environments.  The raw format of a similarity matrix has the form
 * <pre>
 * | rows (uint32) | cols (uint32) | fsize (uint32) | array (float) ... |
 * </pre>
 * where rows and cols are unsigned 32-bit integers specifing the dimensions
 * of the matrix, fsizes is the size of a float in bytes and array holds the
 * matrix of similarity values as floats.
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
    uint32_t ret, rows, cols, fsize, i, j;

    rows = m->row.end - m->row.start;
    cols = m->col.end - m->col.start;
    fsize = sizeof(float);

    ret = fwrite(&rows, sizeof(rows), 1, stdout);
    ret += fwrite(&cols, sizeof(cols), 1, stdout);
    ret += fwrite(&fsize, sizeof(fsize), 1, stdout);
    if (ret != 3) {
        error("Failed to write raw matrix header to stdout");
        return 0;
    }

    for (i = m->row.start; i < m->row.end; i++) {
        for (j = m->col.start; j < m->col.end; j++) {
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
