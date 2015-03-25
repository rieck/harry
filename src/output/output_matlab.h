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

#ifndef OUTPUT_MATLAB_H
#define OUTPUT_MATLAB_H

#define MAT_TYPE_INT8       1
#define MAT_TYPE_UINT8      2
#define MAT_TYPE_INT16      3
#define MAT_TYPE_UINT16     4
#define MAT_TYPE_INT32      5
#define MAT_TYPE_UINT32     6
#define MAT_TYPE_SINGLE     7
#define MAT_TYPE_DOUBLE     9
#define MAT_TYPE_INT64      12
#define MAT_TYPE_UINT64     13
#define MAT_TYPE_ARRAY      14

#define MAT_CLASS_CELL      1
#define MAT_CLASS_STRUCT    2
#define MAT_CLASS_OBJ       3
#define MAT_CLASS_CHAR      4
#define MAT_CLASS_SPARSE    5
#define MAT_CLASS_DOUBLE    6
#define MAT_CLASS_SINGLE    7
#define MAT_CLASS_INT8      8
#define MAT_CLASS_UINT8     9
#define MAT_CLASS_INT16     10
#define MAT_CLASS_UINT16    11
#define MAT_CLASS_INT32     12
#define MAT_CLASS_UINT32    13

/* matlab output module */
int output_matlab_open(char *);
int output_matlab_write(hmatrix_t *);
void output_matlab_close(void);

#endif /* OUTPUT_MATLAB_H */
