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

#include "dist_lee.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_lee</em>: Lee distance for strings.
 * @{
 */

/* Alphabet size */
static int q = 0;

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_lee_config()
{
    config_lookup_int(&cfg, "measures.dist_lee.alphabet", &q);
}

/**
 * Computes the Lee distance of two strings. If the strings have
 * different lengths, the remaining symbols of the longer string are
 * added to the distance.
 * @param x first string 
 * @param y second string
 * @return Lee distance
 */
float dist_lee_compare(str_t x, str_t y)
{
    float d = 0, ad;
    int i;

    /* Loop over strings */
    for (i = 0; i < x.len || i < y.len; i++) {
        if (i < x.len && i < y.len)
            ad = fabs(x.str.s[i] - y.str.s[i]);
        else if (i < x.len)
            ad = x.str.s[i];
        else
            ad = y.str.s[i];

        if (ad >= q) {
            warning("Distance of symbols larger than alphabet. Fixing.");
            ad = q - 1;
        }
        d += fmin(ad, q - ad);
    }

    return d;
}

/** @} */
