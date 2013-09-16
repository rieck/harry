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

#include "kern_subsequence.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_subsequence</em>: Subsequence kernel
 * @{
 */

/* External variables */
extern config_t cfg;

/* Normalizations */
enum norm_type
{ NORM_NONE, NORM_L2 };
static enum norm_type norm = NORM_NONE;

/* Local variables */
static int length = 3;         /**< Maximum length */
static double lambda = 0.1;    /**< Weight for gaps */

/**
 * Initializes the similarity measure
 */
void kern_subsequence_config()
{
    const char *str;

    config_lookup_int(&cfg, "measures.kern_subsequence.length", &length);
    config_lookup_float(&cfg, "measures.kern_subsequence.lambda", &lambda);

    /* Normalization */
    config_lookup_string(&cfg, "measures.kern_wdegree.norm", &str);
    if (!strcasecmp(str, "none")) {
        norm = NORM_NONE;
    } else if (!strcasecmp(str, "l2")) {
        norm = NORM_L2;
    } else {
        warning("Unknown norm '%s'. Using 'none' instead.", str);
    }
}

/**
 * Internal computation of subsequence kernel
 * @param x first string
 * @param y second string
 * @return subsequence kernel
 */
static float kernel(hstring_t x, hstring_t y)
{
    float dps[x.len][y.len];
    float dp[x.len + 1][y.len + 1];
    float kern[length];
    int i, j, l;

    /* Case a: both sequences empty */
    if (x.len == 0 && y.len == 0)
        return 1.0;

    /* Case b: one sequence empty */
    if (x.len == 0 || y.len == 0)
        return 0.0;

    /* Initalize dps */
    for (i = 0; i < x.len; i++)
        for (j = 0; j < y.len; j++)
            if (!hstring_compare(x, i, y, j))
                dps[i][j] = lambda * lambda;
            else
                dps[i][j] = 0;

    /* Initialize dp */
    for (i = 0; i < x.len + 1; i++)
        dp[i][0] = 0;
    for (j = 0; j < y.len + 1; j++)
        dp[0][j] = 0;

    for (l = 0; l < length; l++) {
        kern[l] = 0;
        for (i = 0; i < x.len; i++) {
            for (j = 0; j < y.len; j++) {
                dp[i + 1][j + 1] = dps[i][j] + lambda * dp[i][j + 1] +
                    lambda * dp[i + 1][j] - lambda * lambda * dp[i][j];
                if (!hstring_compare(x, i, y, j)) {
                    kern[l] = kern[l] + dps[i][j];
                    dps[i][j] = lambda * lambda * dp[i][j];
                }
            }
        }
    }
    return kern[length - 1];
}

/**
 * Compute the subsequence kernel by Lodhi et al. (2002). The implementation
 * has been taken from the book by Cristianini & Shawe-Taylor.
 * @param x first string 
 * @param y second string
 * @return subsequence kernel
 */
float kern_subsequence_compare(hstring_t x, hstring_t y)
{
    uint64_t xk, yk;
    float xv, yv;
    int ret;

    float k = kernel(x, y);

    /* Normalization */
    switch (norm) {
    case NORM_L2:
        xk = hstring_hash1(x);
#pragma omp critical (vcache)
        ret = vcache_load(xk, &xv);
        if (!ret) {
            xv = kernel(x, x);
#pragma omp critical (vcache)
            vcache_store(xk, xv);
        }

        yk = hstring_hash1(y);
#pragma omp critical (vcache)
        ret = vcache_load(yk, &yv);
        if (!ret) {
            yv = kernel(y, y);
#pragma omp critical (vcache)
            vcache_store(yk, yv);
        }
        return k / sqrt(xv * yv);

    case NORM_NONE:
    default:
        return k;
    }
}

/** @} */
