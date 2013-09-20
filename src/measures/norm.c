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
#include "norm.h"

/**
 * @addtogroup normalization
 * Functions for normalization of similarity values
 */

/**
 * Parse normalization name. All normalization types are represented
 * by a single enumeration. This is suboptimal but yields a simple 
 * implementation.  
 * @param str String for normalization
 * @return normalization type
 */
norm_t norm_get(const char *str)
{
    if (!strcasecmp(str, "none")) {
        return NORM_NONE;
        
    /* Length normalizations */
    } else if (!strcasecmp(str, "min")) {
        return NORM_MIN;
    } else if (!strcasecmp(str, "max")) {
        return NORM_MAX;
    } else if (!strcasecmp(str, "avg")) {
        return NORM_AVG;
    
    /* Kernel normalizations */
    } else if (!strcasecmp(str, "l2")) {
        return NORM_L2;
    }

    warning("Unknown norm '%s'. Using 'none' instead.", str);
    return NORM_NONE;
}

/**
 * Normalize a similarity value by string length
 * @param n Normalization type
 * @param d Similarity value
 * @param x String 
 * @param y String
 * @return Normalized similarity value
 */
float norm_length(norm_t n, float d, hstring_t x, hstring_t y)
{
    switch (n) {
    case NORM_MIN:
        return d / fmin(x.len, y.len);
    case NORM_MAX:
        return d / fmax(x.len, y.len);
    case NORM_AVG:
        return d / (0.5 * (x.len + y.len));
    case NORM_NONE:
        return d;
    default:
        warning("Unknown norm selected. Skipping normalization.");
        return d;
    }
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
float norm_kernel(norm_t n, float k, hstring_t x, hstring_t y,  
       float (*kernel) (hstring_t, hstring_t))
{
    uint64_t xk, yk;
    float xv, yv;
    int ret;

    /* Normalization */
    switch (n) {
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
        return k;
    default:
        warning("Unknown norm selected. Skipping normalization.");
        return k;
    }
}

/** @} */
