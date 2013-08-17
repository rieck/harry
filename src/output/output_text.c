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
 * <em>text</em>: The matrix of similarity/dissimilarity measures is stored
 * as a regular text file. No black magic.
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "output.h"
#include "harry.h"

#define output_printf(z, ...) \
    { if (zlib) { gzprintf(z, __VA_ARGS__); } \
      else { fprintf(z, __VA_ARGS__); } }

/* External variables */
extern config_t cfg;

/* Local variables */
static void *z = NULL;
static int zlib = 0;

/**
 * Opens a file for writing text format
 * @param fn File name
 * @return number of regular files
 */
int output_text_open(char *fn)
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
        harry_zversion(z, "# ", "Output module for text format");
    else
        harry_version(z, "# ", "Output module for text format");

    return TRUE;
}

/**
 * Writes a block of files to the output
 * @param m Matrix of similarity values 
 * @param x Dimension of matrix
 * @param y Dimension of matrix
 * @param t Set to 1 if only upper triangle given
 * @return Number of written values
 */
int output_text_write(float *m, int x, int y, int t)
{
    assert(x && x >= 0 && y >= 0);
    int i, j, k;

    if (t) {
        for (k = i = 0; i < x; i++) {
            for (j = i; j < y; j++) {
                output_printf(z, "%g ", m[k++]);
            }
            output_printf(z, "\n");
        }
    }

    return 0;
}

/**
 * Closes an open output file.
 */
void output_text_close()
{
    if (z) {
        if (zlib)
            gzclose(z);
        else
            fclose(z);
    }
}

/** @} */
