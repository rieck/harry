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
 * <em>libsvm</em>: The matrix of similarity/dissimilarity measures is
 * stored as a pre-computed kernel for libsvm. 
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

/* Dirty hack to support compression */
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

    config_lookup_bool(&cfg, "output.compress", &zlib);

    if (zlib)
        z = gzopen(fn, "w9");
    else
        z = fopen(fn, "w");

    if (!z) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    return TRUE;
}

/**
 * Write similarity matrux to output
 * @param m Matrix/triangle of similarity values 
 * @return Number of written values
 */
int output_libsvm_write(hmatrix_t *m)
{
    assert(m);
    int i, j, r, k = 0;

    for (i = m->x.i; i < m->x.n; i++) {
        output_printf(z, "%d 0:%d", (int) m->labels[i], i + 1);
        for (j = m->y.i; j < m->y.n; j++) {
            r = output_printf(z, " %d:%g", j + 1, hmatrix_get(m, i, j));
            if (r < 0) {
                error("Could not write to output file");
                return -k;
            }
            k++;
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
