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
#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"

#include "dist_lee.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_lee</em>: Lee distance for strings.
 *
 * Lee. Some properties of nonbinary error-correcting codes. IRE
 * Transactions on Information Theory 4 (2): 77-82, 1958.
 * @{
 */

/* Alphabet size */
static cfg_int min_sym = 0;
static cfg_int max_sym = 255;

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_lee_config()
{
    config_lookup_int(&cfg, "measures.dist_lee.min_sym", &min_sym);
    config_lookup_int(&cfg, "measures.dist_lee.max_sym", &max_sym);
}

/**
 * Computes the Lee distance of two strings. If the strings have
 * different lengths, the remaining symbols of the longer string are
 * added to the distance.
 * @param x first string 
 * @param y second string
 * @return Lee distance
 */
float dist_lee_compare(hstring_t x, hstring_t y)
{
    float d = 0, ad;
    int i, q = max_sym - min_sym;

    /* Loop over strings */
    for (i = 0; i < x.len || i < y.len; i++) {
        if (i < x.len && i < y.len)
            ad = abs(hstring_compare(x, i, y, i) - min_sym);
        else if (i < x.len)
            ad = abs(((int) hstring_get(x, i)) - min_sym);
        else
            ad = abs(((int) hstring_get(y, i)) - min_sym);

        if (ad > q) {
            warning("Distance of symbols larger than alphabet. Fixing.");
            ad = q - 1;
        }
        d += fmin(ad, q - ad);
    }

    return d;
}

/** @} */
