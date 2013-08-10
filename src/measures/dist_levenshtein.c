/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2006 Stephen Toub 
 *               2013  Konrad Rieck (konrad@mlsec.org)
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

#include "dist_levenshtein.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_levenshtein</em>: Levenshtein distance for strings.
 *
 * Adapted from Stephen Toub's C# implementation. 
 * http://blogs.msdn.com/b/toub/archive/2006/05/05/590814.aspx
 * @{
 */

/* Normalizations */
enum norm_type
{ NORM_NONE, NORM_MIN, NORM_MAX, NORM_AVG };
static enum norm_type norm = NORM_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_levenshtein_config()
{
    const char *str;

    /* Costs */
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_ins", &cost_ins);
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_del", &cost_del);
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_sub", &cost_sub);

    /* Normalization */
    config_lookup_string(&cfg, "measures.dist_levenshtein.norm", &str);

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
 * Computes the Levenshtein distance of two strings. Adapted from 
 * Stephen Toub's C# implementation.
 * @param x first string 
 * @param y second string
 * @return Levenshtein distance
 */
float dist_levenshtein_compare(str_t x, str_t y)
{
    int i, j, a, b;

    if (x.len == 0 && y.len == 0)
        return 0;

    /* 
     * Rather than maintain an entire matrix (which would require O(n*m)
     * space), just store the current row and the next row, each of which
     * has a length m+1, so just O(m) space.  Initialize the curr row.
     */
    int curr = 0, next = 1;
    int rows[2][y.len + 1];

    for (j = 0; j <= y.len; j++)
        rows[curr][j] = j;

    /* For each virtual row (we only have physical storage for two) */
    for (i = 1; i <= x.len; i++) {

        /* Fill in the values in the row */
        rows[next][0] = i;
        for (j = 1; j <= y.len; j++) {

            /* Insertion and deletion */
            a = rows[curr][j] + cost_ins;
            b = rows[next][j - 1] + cost_del;
            if (a > b)
                a = b;

            /* Substituion */
            b = rows[curr][j - 1] + (x.str.s[i - 1] ==
                                     y.str.s[j - 1] ? 0 : cost_sub);
            if (a > b)
                a = b;

            /* 
             * Transpositions (Damerau-Levenshtein) are not supported by
             * this implementation, as only two rows of the distance matrix
             * are available. Potential fix: provide three rows.
             */
            rows[next][j] = a;
        }

        /* Swap the current and next rows */
        if (curr == 0) {
            curr = 1;
            next = 0;
        } else {
            curr = 0;
            next = 1;
        }
    }

    switch (norm) {
    case NORM_MIN:
        return rows[curr][y.len] / fmin(x.len, y.len);
    case NORM_MAX:
        return rows[curr][y.len] / fmax(x.len, y.len);
    case NORM_AVG:
        return rows[curr][y.len] / (0.5 * (x.len + y.len));
    case NORM_NONE:
    default:
        return rows[curr][y.len];
    }
}

/** @} */
