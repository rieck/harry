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
#include "norm.h"
#include "dist_damerau.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_damerau</em>: Damerau-Levenshtein distance for strings.
 *
 * Damerau. A technique for computer detection and correction of spelling
 * errors, Communications of the ACM, 7(3):171-176, 1964  
 * @{
 */

/* Normalizations */
static lnorm_t norm = LNORM_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;
static double cost_tra = 1.0;

/* External variables */
extern config_t cfg;

/*
 * Four-way minimum
 */
static float min(float a, float b, float c, float d)
{
    return fmin(fmin(a, b), fmin(c, d));
}

/**
 * Initializes the similarity measure
 */
void dist_damerau_config()
{
    const char *str;

    /* Costs */
    config_lookup_float(&cfg, "measures.dist_damerau.cost_ins", &cost_ins);
    config_lookup_float(&cfg, "measures.dist_damerau.cost_del", &cost_del);
    config_lookup_float(&cfg, "measures.dist_damerau.cost_sub", &cost_sub);
    config_lookup_float(&cfg, "measures.dist_damerau.cost_tra", &cost_tra);

    /* Normalization */
    config_lookup_string(&cfg, "measures.dist_damerau.norm", &str);
    norm = lnorm_get(str);
}

/**
 * Computes the Damerau-Levenshtein distance of two strings. Adapted from 
 * Wikipedia entry and comments from Stackoverflow.com
 * @param x first string 
 * @param y second string
 * @return Levenshtein distance
 */
float dist_damerau_compare(hstring_t x, hstring_t y)
{
    int i, j, inf = x.len + y.len;
    int d[x.len + 2][y.len + 2];

    if (x.len == 0 && y.len == 0)
        return 0;

    /* FIXME. This should be replaced with a hash table */
    int max_alph = 1 << (8 * sizeof(sym_t));
    int alph[max_alph];
    memset(alph, 0, max_alph * sizeof(int));

    /* Initialize distance matrix */
    d[0][0] = inf;
    for (i = 0; i <= x.len; i++) {
        d[i + 1][1] = i;
        d[i + 1][0] = inf;
    }
    for (j = 0; j <= y.len; j++) {
        d[1][j + 1] = j;
        d[0][j + 1] = inf;
    }

    for (i = 1; i <= x.len; i++) {
        int db = 0;
        for (j = 1; j <= y.len; j++) {
            int i1 = alph[hstring_get(y, j - 1)];
            int j1 = db;
            int dz = hstring_compare(x, i - 1, y, j - 1) ? cost_sub : 0;
            if (dz == 0)
                db = j;

            d[i + 1][j + 1] = min(d[i][j] + dz,
                                  d[i + 1][j] + cost_ins,
                                  d[i][j + 1] + cost_del,
                                  d[i1][j1] + (i - i1 - 1) + cost_tra +
                                  (j - j1 - 1));
        }

        alph[hstring_get(x, i - 1)] = i;
    }

    float r = d[x.len + 1][y.len + 1];
    return lnorm(norm, r, x, y);
}

/** @} */
