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


/* External variables */
extern config_t cfg;

/* Local variables */
static void *z = NULL;
static int zlib = 0;

static int save_indices = 0;
static int save_labels = 0;
static int save_sources = 0;
static int upper_triangle = 0;

static const char *separator = ",";

#define output_printf(z, ...) (\
   zlib ? \
       gzprintf((gzFile *) z, __VA_ARGS__) \
   : \
       fprintf((FILE *) z, __VA_ARGS__) \
)

/**
 * Opens a file for writing text format
 * @param fn File name
 * @return number of regular files
 */
int output_text_open(char *fn)
{
    assert(fn);

    config_lookup_int(&cfg, "output.save_indices", &save_indices);
    config_lookup_int(&cfg, "output.save_labels", &save_labels);
    config_lookup_int(&cfg, "output.save_sources", &save_sources);
    config_lookup_int(&cfg, "output.upper_triangle", &upper_triangle);
    config_lookup_string(&cfg, "output.separator", &separator);
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
 * Write similarity matrux to output
 * @param m Matrix/triangle of similarity values 
 * @return Number of written values
 */
int output_text_write(hmatrix_t *m)
{
    assert(m);
    int i, j, r, k = 0;

    if (save_indices) {
        output_printf(z, "#");
        for (j = m->y.i; j < m->y.n; j++) {
            output_printf(z, " %d", j);
        }
        output_printf(z, "\n");
    }

    if (save_labels) {
        output_printf(z, "#");
        for (j = m->y.i; j < m->y.n; j++) {
            output_printf(z, " %g", m->labels[j]);
        }
        output_printf(z, "\n");
    }

    if (save_sources) {
        output_printf(z, "#");
        for (j = m->y.i; j < m->y.n; j++) {
            output_printf(z, " %s", m->srcs[j]);
        }
        output_printf(z, "\n");
    }

    for (i = m->x.i; i < m->x.n; i++) {
        for (j = m->y.i; j < m->y.n; j++) {
            /* Cut off lower triangle */
            if (upper_triangle && j < i) {
                output_printf(z, "%s", separator);
                continue;
            }

            r = output_printf(z, "%g", hmatrix_get(m, i, j));
            if (r < 0) {
                error("Could not write to output file");
                return -k;
            }

            if (j < m->y.n - 1)
                output_printf(z, "%s", separator);

            k++;
        }

        if (save_indices || save_labels || save_sources)
            output_printf(z, " #");

        if (save_indices)
            output_printf(z, " %d", i);

        if (save_labels)
            output_printf(z, " %g", m->labels[i]);

        if (save_sources)
            output_printf(z, " %s", m->srcs[i]);

        output_printf(z, "\n");
    }

    return k;
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
