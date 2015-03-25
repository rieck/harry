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
 * <em>matlab</em>: The similarity matrix is exported as a matlab file
 * version 5. Depending on the configura.starton the indices, sources and
 * labels are also exported.
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "output.h"
#include "harry.h"
#include "output_matlab.h"

/* External variables */
extern config_t cfg;

/* Local variables */
static FILE *f = NULL;
static cfg_int precision = 0;
static int save_indices = 0;
static int save_labels = 0;
static int save_sources = 0;

/**
 * Pads the output stream
 * @param f File pointer
 */
static int fpad(FILE *f)
{
    int i, r = ftell(f) % 8;
    if (r != 0)
        r = 8 - r;

    for (i = 0; i < r; i++)
        fputc(0, f);

    return r;
}

/**
 * Writes a 16-bit integer to a file stream
 * @param i Integer value
 * @param f File pointer
 * @return number of bytes
 */
static int fwrite_uint16(uint16_t i, FILE *f)
{
    return fwrite(&i, 1, sizeof(i), f);
}

/**
 * Writes a 32-bit integer to a file stream
 * @param i Integer value
 * @param f File pointer
 * @return number of bytes
 */
static int fwrite_uint32(uint32_t i, FILE *f)
{
    return fwrite(&i, 1, sizeof(i), f);
}

/**
 * Writes a float to a file stream
 * @param i float value
 * @param f File pointer
 * @return number of bytes
 */
static int fwrite_float(float i, FILE *f)
{
    return fwrite(&i, 1, sizeof(i), f);
}

/**
 * Writes the dimensions of an array to a mat file
 * @param n Number of dimensions
 * @param m Number of dimensions
 * @param f File pointer
 * @return bytes written
 */
static int fwrite_array_dim(uint32_t n, uint32_t m, FILE *f)
{
    fwrite_uint32(MAT_TYPE_INT32, f);
    fwrite_uint32(8, f);
    fwrite_uint32(n, f);
    fwrite_uint32(m, f);

    return 16;
}

/**
 * Writes the name of an array to a mat file
 * @param n Name of arry
 * @param f File pointer
 * @return bytes written
 */
static int fwrite_array_name(char *n, FILE *f)
{
    int r, l = strlen(n);

    if (l <= 4) {
        /* Compressed format */
        fwrite_uint16(MAT_TYPE_INT8, f);
        fwrite_uint16(l, f);
        fwrite(n, l, 1, f);
        r = fpad(f);
        return 4 + l + r;
    } else {
        /* Regular format */
        fwrite_uint32(MAT_TYPE_INT8, f);
        fwrite_uint32(l, f);
        fwrite(n, l, 1, f);
        r = fpad(f);
        return 8 + l + r;
    }
}

/**
 * Writes the flags of an array to a mat file
 * @param n Flags
 * @param c Class
 * @param z Non-zero elements
 * @param f File pointer
 * @return bytes written
 */
static int fwrite_array_flags(uint8_t n, uint8_t c, uint32_t z, FILE *f)
{
    fwrite_uint32(MAT_TYPE_UINT32, f);
    fwrite_uint32(8, f);
    fwrite_uint32(n << 16 | c, f);
    fwrite_uint32(z, f);

    return 16;
}

/**
 * Writes a string
 * @param s string
 * @param f file pointer
 * @return number of bytes
 */
static int fwrite_string(char *s, FILE *f)
{
    int r = 0, l, i;
    if (s)
        l = strlen(s);
    else
        l = 0;

    /* Tag */
    fwrite_uint32(MAT_TYPE_ARRAY, f);
    fwrite_uint32(0, f);

    /* Header */
    r += fwrite_array_flags(0, MAT_CLASS_CHAR, 0, f);
    r += fwrite_array_dim(1, l, f);
    r += fwrite_array_name("str", f);
    r += fwrite_uint32(MAT_TYPE_UINT16, f);
    r += fwrite_uint32(l * 2, f);

    /* Write characters */
    for (i = 0; i < l; i++)
        r += fwrite_uint16(s[i], f);
    r += fpad(f);

    /* Update size in tag */
    fseek(f, -(r + 4), SEEK_CUR);
    fwrite_uint32(r, f);
    fseek(f, r, SEEK_CUR);

    return r + 8;
}

/**
 * Opens a file for writing text format
 * @param fn File name
 * @return true on success, false otherwise
 */
int output_matlab_open(char *fn)
{
    int r = 0;

    config_lookup_int(&cfg, "output.precision", &precision);
    config_lookup_bool(&cfg, "output.save_indices", &save_indices);
    config_lookup_bool(&cfg, "output.save_labels", &save_labels);
    config_lookup_bool(&cfg, "output.save_sources", &save_sources);

    f = fopen(fn, "w");
    if (!f) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    /* Write matlab header */
    r += harry_version(f, "", "Output module for Matlab format (v5)");
    while (r < 124 && r > 0)
        r += fprintf(f, " ");

    /* Write version header */
    r += fwrite_uint16(0x0100, f);
    r += fwrite_uint16(0x4d49, f);
    if (r != 128) {
        error("Could not write header to output file '%s'.", fn);
        return FALSE;
    }

    return TRUE;
}

/**
 * Write a similarity matrix in matlab format
 * @param m Matrix of similarity values
 * @return Number of written bytes
 */
static int fwrite_matrix(hmatrix_t *m)
{
    int r = 0, x, y, i, j;

    x = m->col.end - m->col.start;
    y = m->row.end - m->row.start;

    /* Write tag */
    fwrite_uint32(MAT_TYPE_ARRAY, f);
    fwrite_uint32(0, f);

    /* Write header */
    r += fwrite_array_flags(0, MAT_CLASS_SINGLE, 0, f);
    r += fwrite_array_dim(x, y, f);
    r += fwrite_array_name("matrix", f);
    r += fwrite_uint32(MAT_TYPE_SINGLE, f);
    r += fwrite_uint32(x * y * sizeof(float), f);

    /* Write data */
    for (i = m->row.start; i < m->row.end; i++) {
        for (j = m->col.start; j < m->col.end; j++) {
            float val = hround(hmatrix_get(m, j, i), precision);
            r += fwrite_float(val, f);
        }
    }
    r += fpad(f);

    /* Update size in tag */
    fseek(f, -(r + 4), SEEK_CUR);
    fwrite_uint32(r, f);
    fseek(f, r, SEEK_CUR);

    return r + 8;
}

/**
 * Write range in matlab format
 * @param ra Range structure
 * @param name Name of range
 * @return Number of written bytes
 */
static int fwrite_range(range_t ra, char *name)
{
    int r = 0, i;

    /* Write tag */
    fwrite_uint32(MAT_TYPE_ARRAY, f);
    fwrite_uint32(0, f);

    /* Write header */
    r += fwrite_array_flags(0, MAT_CLASS_UINT32, 0, f);
    r += fwrite_array_dim(ra.end - ra.start, 1, f);
    r += fwrite_array_name(name, f);
    r += fwrite_uint32(MAT_TYPE_UINT32, f);
    r += fwrite_uint32((ra.end - ra.start) * sizeof(uint32_t), f);

    /* Write data */
    for (i = ra.start; i < ra.end; i++)
        r += fwrite_uint32((uint32_t) i, f);
    r += fpad(f);

    /* Update size in tag */
    fseek(f, -(r + 4), SEEK_CUR);
    fwrite_uint32(r, f);
    fseek(f, r, SEEK_CUR);

    return r + 8;
}

/**
 * Write labels in matlab format
 * @param ra Range structure
 * @param labels Array of all labels
 * @param name Name of labels
 * @return Number of written bytes
 */
static int fwrite_labels(range_t ra, float *labels, char *name)
{
    int r = 0, i;

    /* Write tag */
    fwrite_uint32(MAT_TYPE_ARRAY, f);
    fwrite_uint32(0, f);

    /* Write header */
    r += fwrite_array_flags(0, MAT_CLASS_SINGLE, 0, f);
    r += fwrite_array_dim(ra.end - ra.start, 1, f);
    r += fwrite_array_name(name, f);
    r += fwrite_uint32(MAT_TYPE_SINGLE, f);
    r += fwrite_uint32((ra.end - ra.start) * sizeof(float), f);

    /* Write data */
    for (i = ra.start; i < ra.end; i++)
        r += fwrite_float(labels[i], f);
    r += fpad(f);

    /* Update size in tag */
    fseek(f, -(r + 4), SEEK_CUR);
    fwrite_uint32(r, f);
    fseek(f, r, SEEK_CUR);

    return r + 8;
}

/**
 * Write sources in matlab format
 * @param ra Range structure
 * @param sources Array of all sources
 * @param name Name of sources
 * @return Number of written bytes
 */
static int fwrite_sources(range_t ra, char **sources, char *name)
{
    int r = 0, i;

    /* Write tag */
    fwrite_uint32(MAT_TYPE_ARRAY, f);
    fwrite_uint32(0, f);

    /* Write header */
    r += fwrite_array_flags(0, MAT_CLASS_CELL, 0, f);
    r += fwrite_array_dim(ra.end - ra.start, 1, f);
    r += fwrite_array_name(name, f);

    /* Write data */
    for (i = ra.start; i < ra.end; i++)
        r += fwrite_string(sources[i], f);
    r += fpad(f);

    /* Update size in tag */
    fseek(f, -(r + 4), SEEK_CUR);
    fwrite_uint32(r, f);
    fseek(f, r, SEEK_CUR);

    return r + 8;
}


/**
 * Write similarity matrix to output
 * @param m Matrix/triangle of similarity values
 * @return Number of written values
 */
int output_matlab_write(hmatrix_t *m)
{
    int r = 0;

    /* Write similarity matrix */
    r += fwrite_matrix(m);

    /* Save indices as vectors */
    if (save_indices) {
        r += fwrite_range(m->col, "x_indices");
        r += fwrite_range(m->row, "y_indices");
    }

    /* Save labels as vectors */
    if (save_labels) {
        r += fwrite_labels(m->col, m->labels, "x_labels");
        r += fwrite_labels(m->row, m->labels, "y_labels");
    }

    /* Save sources as cell array */
    if (save_sources) {
        r += fwrite_sources(m->col, m->srcs, "x_sources");
        r += fwrite_sources(m->row, m->srcs, "y_sources");
    }

    return r;
}

/**
 * Closes an open output file.
 */
void output_matlab_close()
{
    if (!f)
        return;

    fclose(f);
}

/** @} */
