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

#ifndef HMATRIX_H
#define HMATRIX_H

#include "hstring.h"

/** 
 * Range for matrix 
 */
typedef struct
{
    int start;    /**< Start of range (inclusive) */
    int end;      /**< End of range (exclusive) */
} range_t;

#define RANGE_LENGTH(r) (r.end -r.start)

/**
 * Structure for a matrix
 */
typedef struct
{
    float *labels;      /**< Labels */
    char **srcs;        /**< Sources */
    int num;            /**< Number of strings */

    float *values;      /**< Similarity values */
    int size;           /**< Size of memory */
    int calcs;          /**< Required calculations */
    range_t col;        /**< Column range */
    range_t row;        /**< Row range */
    int triangular;     /**< Flag for triangular storage */
} hmatrix_t;


/**
 * Detailed structural specification of matrices.
 *
 * These values are of special interest of matrix slices which
 * may represent subranges along or completely away the diagonal.
 * Such ranges can be represented as a composition of three
 * different parts: (1) Top, (2) Middle and (3) Bottom.
 * The top and bottom parts represents ranges that need to be
 * fully computed whereas the middle range is cut be the matrix'
 * diagonal and therefore, contains "duplicate" values, i.e. values
 * from above the diagonal equal those from below it.
 *
 * <code>$ harry --col_range 2:6 --row_range 1:8
 *
 *
 *      0   1 | 2   3   4   5 | 6   7   8
 *            |               |
 * 0    x   . | .   .   .   . | .   .   .
 * -----------+---------------+--------------
 * 1    .   x | .   .   .   . | .   .   .   b_top
 *            |               +--------------
 * 2    .   . | x   .   .   . | .   .   .
 *            |               |
 * 3    .   . | .   x   .   . | .   .   .
 *            |               |             a
 * 4    .   . | .   .   x   . | .   .   .
 *            |               |
 * 5    .   . | .   .   .   x | .   .   .
 *            |               +--------------
 * 6    .   . | .   .   .   . | x   .   .
 *            |               |             b_bottom
 * 7    .   . | .   .   .   . | .   x   .
 * -----------+---------------+--------------
 * 8    .   . | .   .   .   . | .   .   x
 *            |               |
 *                 width
 *
 * </code>
 *
 * Please note that \a A_middle is not necessarily squared
 * but may itself have regions that need to be fully computed.
 * In the following example these parts are defined by \a b_left
 * and \a b_right respectively.
 *
 * <code>$ harry --col_range 1:8 --row_range 2:6
 *
 *      0 | 1   2   3   4   5   6   7 | 8
 *        |                           |
 * 0    x | .   .   .   .   .   .   . | .
 *        |                           |
 * 1    . | x   .   .   .   .   .   . | .
 * -------+---------------------------+------
 * 2    . | .   x   .   .   .   .   . | .
 *        |                           |
 * 3    . | .   .   x   .   .   .   . | .
 *        |                           |    height
 * 4    . | .   .   .   x   .   .   . | .
 *        |                           |
 * 5    . | .   .   .   .   x   .   . | .
 * -------+---+---------------+-------+------
 * 6    . | . | .   .   .   . | x   . | .
 *        |   |               |       |
 * 7    . | . | .   .   .   . | .   x | .
 *        |   |               |       |
 * 8    . | . | .   .   .   . | .   . | x
 *        |   |               |       |
 *       b_left       a        b_right
 *
 * </code>
 */
typedef struct
{
    unsigned int n;

    unsigned int n_top;
    unsigned int n_mid;
    unsigned int n_bottom;

    unsigned int a;
    int b_top;
    int b_bottom;
    int b_left;
    int b_right;
} hmatrixspec_t;

hmatrix_t *hmatrix_init(hstring_t *, int);
void hmatrix_col_range(hmatrix_t *, char *);
void hmatrix_row_range(hmatrix_t *, char *);
void hmatrix_inferspec(const hmatrix_t *, hmatrixspec_t *);
void hmatrix_split(hmatrix_t *, char *);
void hmatrix_split_ex(hmatrix_t *, const int, const int);
float *hmatrix_alloc(hmatrix_t *);
float hmatrix_get(hmatrix_t *, int, int);
void hmatrix_set(hmatrix_t *, int, int, float);
void hmatrix_compute(hmatrix_t *, hstring_t *,
                     double (*measure) (hstring_t, hstring_t));
void hmatrix_destroy(hmatrix_t *);
float hmatrix_benchmark(hmatrix_t *, hstring_t *,
                        double (*measure) (hstring_t, hstring_t), double);

#endif /* HMATRIX_H */
