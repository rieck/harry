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
#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "vcache.h"

#include "dist_compression.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_compression</em>: Compression distance for strings.
 * @{
 */

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_compression_config()
{
}

/**
 * Computes the Compression distance of two strings. 
 * @param x first string 
 * @param y second string
 * @return Compression distance
 */
float dist_compression_compare(str_t x, str_t y)
{
    float xl, yl;
    uint64_t xk, yk;
    unsigned long max = compressBound(fmax(x.len, y.len) * sizeof(sym_t));
    unsigned char *dest = alloca(max);
    
    xk = str_hash1(x);
    if (!vcache_load(xk, &xl)) {
        unsigned long len = max;
        compress(dest, &len, (void *) x.str.s, x.len * sizeof(sym_t));
        xl = len;
        vcache_store(xk, xl);
    }
    
    yk = str_hash1(y);
    if (!vcache_load(yk, &yl)) {
        unsigned long len = max;
        compress(dest, &len, (void *) y.str.s, y.len * sizeof(sym_t));
        yl = len;
        vcache_store(yk, yl);
    }

    return 0;
}

/** @} */
