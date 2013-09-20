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
#include "norm.h"
#include "dist_hamming.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_hamming</em>: Hamming distance for strings.
 * 
 * Hamming. Error-detecting and error-correcting codes. Bell System
 * Technical Journal, 29(2):147-160, 1950.
 * @{
 */

/* Normalizations */
static norm_t norm = NORM_NONE;

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_hamming_config()
{
    const char *str;

    /* Normalization */
    config_lookup_string(&cfg, "measures.dist_hamming.norm", &str);
    norm = norm_get(str);
}

/**
 * Computes the Hamming distance of two strings. If the strings have
 * different lengths, the remaining symbols of the longer string are
 * considered mismatches.
 * @param x first string 
 * @param y second string
 * @return Hamming distance
 */
float dist_hamming_compare(hstring_t x, hstring_t y)
{
    float d = 0;
    int i;

    /* Loop over strings */
    for (i = 0; i < x.len && i < y.len; i++)
        if (hstring_compare(x, i, y, i))
            d += 1;

    /* Add remaining characters as mismatches */
    d += fabs(y.len - x.len);
    
    return norm_length(norm, d, x, y);
}

/** @} */
