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
 * Structure for a matrix
 */
typedef struct
{
    float *labels	/**< Labels */
    char *srcs;		/**< Sources */
    int num;		/**< Number of strings */

    float *values;	/**< Similarity values */
    int xb, xe;		/**< Begin and end indices */
    int yb, ye;		/**< Begin and end indices */ 
    int sym:1;		/**< Symmetric flag */
} hmatrix_t;

void hmatrix_print(hmatrix_t);
hmatrix_t hmatrix_init(char *x, char *y, int s, int n);
void hmatrix_destroy(hmatrix_t);

#endif /* HMATRIX_H */
