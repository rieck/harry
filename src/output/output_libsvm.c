/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
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
 * <em>libsvm</em>: The matrix of similarity/dissimilarity measures is stored
 * as a regular libsvm file. No black magic.
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
static void *z = NULL;
static int zlib = 0;

#define output_printf(z, ...) (\
   zlib ? \
       gzprintf((gzFile *) z, __VA_ARGS__) \
   : \
       fprintf((FILE *) z, __VA_ARGS__) \
)

/**
 * Opens a file for writing libsvm format
 * @param fn File name
 * @return number of regular files
 */
int output_libsvm_open(char *fn)
{
    assert(fn);

    config_lookup_int(&cfg, "output.compress", &zlib);

    if (zlib)
        z = gzopen(fn, "w9");
    else
        z = fopen(fn, "w");

    if (!z) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    /* Write harry header */
    if (zlib)
        harry_zversion(z, "# ", "Output module for libsvm format");
    else
        harry_version(z, "# ", "Output module for libsvm format");

    return TRUE;
}

/**
 * Write similarity matrux to output
 * @param m Matrix/triangle of similarity values 
 * @param x Dimension of matrix
 * @param y Dimension of matrix
 * @param t 0 if matrix given, 1 for upper-right triangle
 * @return Number of written values
 */
int output_libsvm_write(float *m, int x, int y, int t)
{
    assert(x && x >= 0 && y >= 0);
    int i, j, k, r;

    for (k = i = 0; i < x; i++) {
        output_printf(z, "<label> 0:%d", i);
        for (j = 0; j < y; j++) {
            if (t)
                r = output_printf(z, " %d:%g", j + 1, m[tr_index(i,j,x)]);
            else
                r = output_printf(z, " %d:%g", j + 1, m[k++]);
            if (r < 0) {
                error("Could not write to output file");
                return -k;
            }
        }
        output_printf(z, "\n");
    }

    return k;
}

/**
 * Closes an open output file.
 */
void output_libsvm_close()
{
    if (z) {
        if (zlib)
            gzclose(z);
        else
            fclose(z);
    }
}

/** @} */
