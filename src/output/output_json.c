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
 * <em>json</em>: The matrix of similarity/dissimilarity measures is stored
 * as a JSON object.
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

static cfg_int precision = 0;
static int zlib = 0;
static int save_indices = 0;
static int save_labels = 0;
static int save_sources = 0;

#define output_printf(z, ...) (\
   zlib ? \
       gzprintf((gzFile) z, __VA_ARGS__) \
   : \
       fprintf((FILE *) z, __VA_ARGS__) \
)

/**
 * Opens a file for writing json format
 * @param fn File name
 * @return number of regular files
 */
int output_json_open(char *fn)
{
    assert(fn);

    config_lookup_int(&cfg, "output.precision", &precision);
    config_lookup_bool(&cfg, "output.save_indices", &save_indices);
    config_lookup_bool(&cfg, "output.save_labels", &save_labels);
    config_lookup_bool(&cfg, "output.save_sources", &save_sources);
    config_lookup_bool(&cfg, "output.compress", &zlib);

    if (zlib)
        z = gzopen(fn, "w9");
    else
        z = fopen(fn, "w");

    if (!z) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    output_printf(z, "{\n");

    return TRUE;
}

/**
 * Write similarity matrux to output
 * @param m Matrix of similarity values 
 * @return Number of written values
 */
int output_json_write(hmatrix_t *m)
{
    assert(m);
    int i, j, k = 0;

    if (save_indices) {
        output_printf(z, "  \"col_indices\": [");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, "%d", j);
            if (j < m->col.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n  \"row_indices\": [");
        for (j = m->row.start; j < m->row.end; j++) {
            output_printf(z, "%d", j);
            if (j < m->row.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n");
    }

    if (save_labels) {
        output_printf(z, "  \"col_labels\": [");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, " %g", m->labels[j]);
            if (j < m->col.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n  \"row_labels\": [");
        for (j = m->row.start; j < m->row.end; j++) {
            output_printf(z, "%g", m->labels[j]);
            if (j < m->row.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n");
    }

    if (save_sources) {
        output_printf(z, "  \"col_sources\": [");
        for (j = m->col.start; j < m->col.end; j++) {
            output_printf(z, "\"%s\"", m->srcs[j]);
            if (j < m->row.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n  \"row_sources\": [");
        for (j = m->row.start; j < m->row.end; j++) {
            output_printf(z, "\"%s\"", m->srcs[j]);
            if (j < m->row.end - 1)
                output_printf(z, ", ");
        }
        output_printf(z, "],\n");
    }

    output_printf(z, "  \"matrix\": [\n");
    for (i = m->row.start; i < m->row.end; i++) {
        output_printf(z, "    [");
        for (j = m->col.start; j < m->col.end; j++) {
            float val = hround(hmatrix_get(m, j, i), precision);
            output_printf(z, "%g", val);
            if (j < m->col.end - 1)
                output_printf(z, ", ");
            k++;
        }
        output_printf(z, "]");
        if (i < m->row.end - 1)
            output_printf(z, ",");
        output_printf(z, "\n");
    }
    output_printf(z, "  ]\n");
    return k;
}

/**
 * Closes an open output file.
 */
void output_json_close()
{
    output_printf(z, "}\n");

    if (zlib)
        gzclose(z);
    else
        fclose(z);
}

/** @} */
