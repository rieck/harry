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
 *
 * Cilibrasi and Vitanyi. Clustering by compression, IEEE Transactions on
 * Information Theory, 51:4, 1523-1545, 2005.
 * @{
 */

/* External variables */
extern config_t cfg;
static cfg_int level = 0;

/**
 * Initializes the similarity measure
 */
void dist_compression_config()
{
    /* Configuration */
    config_lookup_int(&cfg, "measures.dist_compression.level", &level);
}


/**
 * Compress one string and return the length of the compressed data
 * @param x String x
 * @return length of the compressed data
 */
static float compress_str1(hstring_t x)
{
    unsigned long tmp, width;
    unsigned char *dst;

    width = x.type == TYPE_SYM ? sizeof(sym_t) : sizeof(char);
    tmp = compressBound(x.len * width);

    dst = malloc(tmp);
    if (!dst) {
        error("Failed to allocate memory for compression");
        return -1;
    }

    compress2(dst, &tmp, (void *) x.str.c, x.len * width, level);

    free(dst);
    return (float) tmp;
}

/**
 * Compress two strings and return the length of the compressed data
 * @param x String x
 * @param y String y
 * @return length of the compressed data.
 */
static float compress_str2(hstring_t x, hstring_t y)
{
    unsigned long tmp, width;
    unsigned char *src, *dst;

    assert(x.type == y.type);

    width = x.type == TYPE_SYM ? sizeof(sym_t) : sizeof(char);
    tmp = compressBound((x.len + y.len) * width);

    dst = malloc(tmp);
    src = malloc(tmp);
    if (!src || !dst) {
        error("Failed to allocate memory for compression");
        return -1;
    }

    /* Concatenate sequences y and x */
    memcpy(src, y.str.s, y.len * width);
    memcpy(src + y.len * width, x.str.s, x.len * width);

    compress2(dst, &tmp, src, (x.len + y.len) * width, level);

    free(dst);
    free(src);
    return (float) tmp;
}



/**
 * Computes the compression distance of two strings. 
 * @param x first string 
 * @param y second string
 * @return Compression distance
 */
float dist_compression_compare(hstring_t x, hstring_t y)
{
    float xl, yl, xyl, yxl;
    uint64_t xk, yk, xyk, yxk;

    xk = hstring_hash1(x);
    if (!vcache_load(xk, &xl, ID_DIST_COMPRESS)) {
        xl = compress_str1(x);
        vcache_store(xk, xl, ID_DIST_COMPRESS);
    }

    yk = hstring_hash1(y);
    if (!vcache_load(yk, &yl, ID_DIST_COMPRESS)) {
        yl = compress_str1(y);
        vcache_store(yk, yl, ID_DIST_COMPRESS);
    }

    xyk = hstring_hash2(x, y);
    if (!vcache_load(xyk, &xyl, ID_DIST_COMPRESS)) {
        xyl = compress_str2(x, y);
        vcache_store(xyk, xyl, ID_DIST_COMPRESS);
    }

    yxk = hstring_hash2(y, x);
    if (!vcache_load(yxk, &yxl, ID_DIST_COMPRESS)) {
        yxl = compress_str2(y, x);
        vcache_store(yxk, yxl, ID_DIST_COMPRESS);
    }

    /* Symmetric version of distance */
    return (0.5 * (xyl + yxl) - fmin(xl, yl)) / fmax(xl, yl);
}

/** @} */
