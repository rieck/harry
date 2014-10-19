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
 * <em>matlab</em>: The similarity matrix is exported as a matlab file 
 * version 5. Depending on the configuration the indices, sources and 
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
static int save_indices = 0;
static int save_labels = 0;
static int save_sources = 0;
static int triangular = 0;

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
 * Writes a double to a file stream
 * @param i double value
 * @param f File pointer
 * @return number of bytes
 */
static int fwrite_double(double i, FILE *f)
{
    return fwrite(&i, 1, sizeof(i), f);
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
    config_lookup_bool(&cfg, "output.save_indices", &save_indices);
    config_lookup_bool(&cfg, "output.save_labels", &save_labels);
    config_lookup_bool(&cfg, "output.save_sources", &save_sources);
    config_lookup_bool(&cfg, "output.triangular", &triangular);

    f = fopen(fn, "w");
    if (!f) {
        error("Could not open output file '%s'.", fn);
        return FALSE;
    }

    return TRUE;
}

/**
 * Write similarity matrix to output
 * @param m Matrix/triangle of similarity values
 * @return Number of written values
 */
int output_matlab_write(hmatrix_t *m)
{
    warning("DUMMY DUMMY DUMMY");
    return 0;
}

/**
 * Closes an open output file.
 */
void output_matlab_close()
{
    fclose(f);
}

/** @} */
