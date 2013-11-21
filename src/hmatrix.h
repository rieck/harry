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

#ifndef HMATRIX_H
#define HMATRIX_H

/** 
 * Range for matrix 
 */
typedef struct 
{
    int i;    /**< Start of range */
    int n;    /**< End of range */
} range_t;

/**
 * Structure for a matrix
 */
typedef struct
{
    float *labels;      /**< Labels */
    char **srcs;        /**< Sources */
    int num;            /**< Number of strings */

    float *values;      /**< Similarity values */
    range_t x;          /**< Range x */
    range_t y;          /**< Range y */
} hmatrix_t;

hmatrix_t hmatrix_init();
hmatrix_t hmatrix_range(hmatrix_t m, char *x, char *y);
hmatrix_t hmatrix_alloc(hmatrix_t m);
void hmatrix_destroy(hmatrix_t);

#endif /* HMATRIX_H */
