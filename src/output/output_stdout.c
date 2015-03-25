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
 * <em>stdout</em>: The matrix of similarity/dissimilarity measures is written
 * to standard output. No black magic.
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
static int save_indices = 0;
static int save_labels = 0;
static int save_sources = 0;
static int zlib = 0;
static cfg_int precision = 0;

static void *z = NULL;
static const char *separator = ",";

/* Dirty hack to support compression */
#define output_printf(z, ...) (\
   zlib ? \
       gzprintf((gzFile) z, __VA_ARGS__) \
   : \
       fprintf((FILE *) z, __VA_ARGS__) \
)


/**
 * Opens a file for writing stdout format
 * @param fn File name (bogus)
 * @return true on success, false otherwise
 */
int output_stdout_open(char *fn)
{
    config_lookup_bool(&cfg, "output.save_indices", &save_indices);
    config_lookup_bool(&cfg, "output.save_labels", &save_labels);
    config_lookup_bool(&cfg, "output.save_sources", &save_sources);
    config_lookup_bool(&cfg, "output.compress", &zlib);
    config_lookup_string(&cfg, "output.separator", &separator);
    config_lookup_int(&cfg, "output.precision", &precision);

    /* Init source */
    if (zlib) {
        z = gzdopen(2, "w9");
    } else {
        z = stdout;
    }

    if (!z) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    /* Write harry header */
    if (zlib) {
        harry_zversion(z, "# ", "Output module for stdout format");
    } else {
        harry_version(z, "# ", "Output module for stdout format");
    }

    return TRUE;
}

/**
 * Write similarity matrux to output
 * @param m Matrix of similarity values 
 * @return Number of written values
 */
int output_stdout_write(hmatrix_t *m)
{
    assert(m);
    int i, j, r, k = 0;

    if (save_indices) {
        output_printf(z, "#");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, " %d", j);
        }
        output_printf(z, "\n");
    }

    if (save_labels) {
        output_printf(z, "#");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, " %g", m->labels[j]);
        }
        output_printf(z, "\n");
    }

    if (save_sources) {
        output_printf(z, "#");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, " %s", m->srcs[j]);
        }
        output_printf(z, "\n");
    }

    for (i = m->row.start; i < m->row.end; i++) {
        for (j = m->col.start; j < m->col.end; j++) {
            float val = hround(hmatrix_get(m, j, i), precision);
            r = output_printf(z, "%g", val);
            if (r < 0) {
                error("Could not write to output file");
                return -k;
            }

            if (j < m->col.end - 1)
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
void output_stdout_close()
{
    if (zlib)
        gzclose(z);
    else
        fclose(z);
}

/** @} */
