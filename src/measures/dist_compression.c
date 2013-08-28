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
static int level = 0;
static int symmetric = 0;

/**
 * Initializes the similarity measure
 */
void dist_compression_config()
{
    /* Configuration */
    config_lookup_int(&cfg, "measures.dist_compression.level", &level);
    config_lookup_int(&cfg, "measures.dist_compression.symmetric",
                      &symmetric);
}

/**
 * Computes the compression distance of two strings. 
 * @param x first string 
 * @param y second string
 * @return Compression distance
 */
float dist_compression_compare(str_t x, str_t y)
{
    float xl, yl, xyl;
    uint64_t xk, yk;
    unsigned long len, tmp;
    unsigned char *dst, *src;
    int ret;

    /* Allocate memory for compression */
    len = compressBound((x.len + y.len) * sizeof(sym_t));
    dst = malloc(len);
    src = malloc(len);
    if (!src || !dst) {
        error("Failed to allocate memory for compression");
        return -1;
    }

    xk = str_hash1(x);
    #pragma omp critical (vcache)
    ret = vcache_load(xk, &xl);
    if (!ret) {
        /* Compress sequence x */
        tmp = len;
        compress2(dst, &tmp, (void *) x.str.s, x.len * sizeof(sym_t), level);
        xl = tmp;
        #pragma omp critical (vcache)
        vcache_store(xk, xl);
    }

    yk = str_hash1(y);
    #pragma omp critical (vcache)
    ret = vcache_load(yk, &yl);
    if (!ret) {
        /* Compress sequence y */
        tmp = len;
        compress2(dst, &tmp, (void *) y.str.s, y.len * sizeof(sym_t), level);
        yl = tmp;
        #pragma omp critical (vcache)
        vcache_store(yk, yl);
    }

    /* Concatenate sequences x and y */
    memcpy(src, x.str.s, x.len * sizeof(sym_t));
    memcpy(src + x.len * sizeof(sym_t), y.str.s, y.len * sizeof(sym_t));

    /* Compress both sequences */
    tmp = len;
    compress2(dst, &tmp, src, (x.len + y.len) * sizeof(sym_t), level);
    xyl = tmp;

    if (symmetric) {
        /* Concatenate sequences y and x */
        memcpy(src, y.str.s, y.len * sizeof(sym_t));
        memcpy(src + y.len * sizeof(sym_t), x.str.s, x.len * sizeof(sym_t));

        /* Compress both sequences */
        tmp = len;
        compress(dst, &tmp, src, (x.len + y.len) * sizeof(sym_t));
        xyl += tmp;
    } else {
        xyl *= 2.0;
    }

    /* Free memory */
    free(dst);
    free(src);
    return xyl / (xl + yl) - 1.0;
}

/** @} */
