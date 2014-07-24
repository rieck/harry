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

/**
 * @addtogroup normalization
 * Functions for normalization of similarity values
 */

/**
 * Parse string for length normalization
 * @param str String for normalization
 * @return normalization type
 */
lnorm_t lnorm_get(const char *str)
{
    if (!strcasecmp(str, "none")) {
        return LN_NONE;
    } else if (!strcasecmp(str, "min")) {
        return LN_MIN;
    } else if (!strcasecmp(str, "max")) {
        return LN_MAX;
    } else if (!strcasecmp(str, "avg")) {
        return LN_AVG;
    }

    warning("Unknown length norm '%s'. Using 'none' instead.", str);
    return LN_NONE;
}

/**
 * Normalize a similarity value by string length
 * @param n Normalization type
 * @param d Similarity value
 * @param x String
 * @param y String
 * @return Normalized similarity value
 */
float lnorm(lnorm_t n, float d, hstring_t x, hstring_t y)
{
    switch (n) {
    case LN_MIN:
        return d / fmin(x.len, y.len);
    case LN_MAX:
        return d / fmax(x.len, y.len);
    case LN_AVG:
        return d / (0.5 * (x.len + y.len));
    case LN_NONE:
    default:
        return d;
    }
}

/**
 * Parse string for kernel normalization
 * @param str String for normalization
 * @return normalization type
 */
knorm_t knorm_get(const char *str)
{
    if (!strcasecmp(str, "none")) {
        return KN_NONE;
    } else if (!strcasecmp(str, "l2")) {
        return KN_L2;
    }

    warning("Unknown kernel norm '%s'. Using 'none' instead.", str);
    return KN_NONE;
}

/**
 * Normalize a similarity value using a kernel function
 * @param n Normalization type
 * @param k Similarity value
 * @param x String
 * @param y String
 * @param kernel Kernel function
 * @return Normalized similarity value
 */
float knorm(knorm_t n, float k, hstring_t x, hstring_t y,
            float (*kernel) (hstring_t, hstring_t))
{
    uint64_t xk, yk;
    float xv, yv;

    /* Normalization */
    switch (n) {
    case KN_L2:
        xk = hstring_hash1(x);
        if (!vcache_load(xk, &xv, ID_NORM)) {
            xv = kernel(x, x);
            vcache_store(xk, xv, ID_NORM);
        }

        yk = hstring_hash1(y);
        if (!vcache_load(yk, &yv, ID_NORM)) {
            yv = kernel(y, y);
            vcache_store(yk, yv, ID_NORM);
        }
        return k / sqrt(xv * yv);
    case KN_NONE:
    default:
        return k;
    }
}

/** @} */
