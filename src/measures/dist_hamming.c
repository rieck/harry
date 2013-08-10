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

#include "dist_hamming.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_hamming</em>: Hamming distance for strings.
 * @{
 */

/* Normalizations */
enum norm_type
{ NORM_NONE, NORM_MIN, NORM_MAX, NORM_AVG };
static enum norm_type norm = NORM_NONE;

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
    if (!strcasecmp(str, "none")) {
        norm = NORM_NONE;
    } else if (!strcasecmp(str, "min")) {
        norm = NORM_MIN;
    } else if (!strcasecmp(str, "max")) {
        norm = NORM_MAX;
    } else if (!strcasecmp(str, "avg")) {
        norm = NORM_AVG;
    } else {
        warning("Unknown norm '%s'. Using 'none' instead.", str);
    }
}

/**
 * Computes the Hamming distance of two strings. If the strings have
 * different lengths, the remaining symbols of the longer string are
 * considered mismatches.
 * @param x first string 
 * @param y second string
 * @return Hamming distance
 */
float dist_hamming_compare(str_t x, str_t y)
{
    float d = 0;
    int i;

    /* Loop over strings */
    for (i = 0; i < x.len && i < y.len; i++)
        if (x.str.s[i] != y.str.s[i])
            d += 1;

    /* Add remaining characters as mismatches */
    d += fabs(y.len - x.len);

    switch (norm) {
    case NORM_MIN:
        return d / fmin(x.len, y.len);
    case NORM_MAX:
        return d / fmax(x.len, y.len);
    case NORM_AVG:
        return d / (0.5 * (x.len + y.len));
    case NORM_NONE:
    default:
        return d;
    }
}

/** @} */
