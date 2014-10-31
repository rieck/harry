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
#include "norm.h"
#include "kern_subsequence.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_subsequence</em>: Subsequence kernel
 *
 * Lodhi, Saunders, Shawe-Taylor, Cristianini, and Watkins. Text
 * classification using string kernels. Journal of Machine Learning
 * Research, 2:419-444, 2002.
 * @{
 */

/* External variables */
extern config_t cfg;

/* Normalizations */
static knorm_t n = KN_NONE;

/* Local variables */
static cfg_int length = 3;     /**< Maximum length */
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
    config_lookup_string(&cfg, "measures.kern_subsequence.norm", &str);
    n = knorm_get(str);
}


/* Ugly macros to access arrays */
#define DP(i,j)     dp[(i) * (y.len + 1) + (j)]
#define DPS(i,j)    dps[(i) * y.len + (j)]

/**
 * Internal computation of subsequence kernel
 * @param x first string
 * @param y second string
 * @return subsequence kernel
 */
static float kernel(hstring_t x, hstring_t y)
{
    float *dps, *dp;
    float kern[length];
    int i, j, l;

    /* Case a: both sequences empty */
    if (x.len == 0 && y.len == 0)
        return 1.0;

    /* Case b: one sequence empty */
    if (x.len == 0 || y.len == 0)
        return 0.0;

    /* Allocate temporary memory */
    dp = malloc(sizeof(float) * (x.len + 1) * (y.len + 1));
    dps = malloc(sizeof(float) * x.len * y.len);
    if (!dp || !dps) {
        error("Could not allocate memory for subsequence kernel");
        return 0;
    }

    /* Initalize dps */
    for (i = 0; i < x.len; i++)
        for (j = 0; j < y.len; j++) {
            if (!hstring_compare(x, i, y, j))
                DPS(i,j) = lambda * lambda;
            else
                DPS(i,j) = 0;
        }

    /* Initialize dp */
    for (i = 0; i < x.len + 1; i++)
        DP(i, 0) = 0;
    for (j = 0; j < y.len + 1; j++)
        DP(0, j) = 0;

    for (l = 0; l < length; l++) {
        kern[l] = 0;
        for (i = 0; i < x.len; i++) {
            for (j = 0; j < y.len; j++) {
                DP(i + 1, j + 1) = DPS(i,j) + lambda * DP(i, j + 1) +
                    lambda * DP(i + 1, j) - lambda * lambda * DP(i, j);
                if (!hstring_compare(x, i, y, j)) {
                    kern[l] = kern[l] + DPS(i,j);
                    DPS(i,j) = lambda * lambda * DP(i, j);
                }
            }
        }
    }
    
    free(dps);
    free(dp);
    
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
    float k = kernel(x, y);
    return knorm(n, k, x, y, kernel);
}

/** @} */
