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

#include "dist_damerau.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_damerau</em>: Damerau-Levenshtein distance for strings.
 *
 * 
 * @{
 */

/* Normalizations */
enum norm_type
{ NORM_NONE, NORM_MIN, NORM_MAX, NORM_AVG };
static enum norm_type norm = NORM_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;
static double cost_tra = 1.0;

/* External variables */
extern config_t cfg;

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
 * Computes the Damerau-Levenshtein distance of two strings. Adapted from 
 * Wikipedia entry and comments from Stackoverflow.com
 * @param x first string 
 * @param y second string
 * @return Levenshtein distance
 */
float dist_damerau_compare(str_t x, str_t y)
{
    int i, j, inf = x.len + y.len;
    int d[x.len + 2][y.len + 2];
    
    
    /* Hack. We will change this into a hash table */
    int alph[65535];

    if (x.len == 0 && y.len == 0)
        return 0;

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

    memset(alph, 0, 65535 * sizeof(int));

    for (i = 1; i <= x.len; i++) {
        int db = 0;
        for (j = 1; j <= y.len; j++) {
            int i1 = alph[y.str.s[j - 1]];
            int j1 = db;
            int dz = ((x.str.s[i - 1] == y.str.s[j - 1]) ? 0 : cost_sub);

            if (dz == 0)
                db = j;

            d[i + 1][j + 1] = min(d[i][j] + dz,
                                  d[i + 1][j] + cost_ins,
                                  d[i][j + 1] + cost_del,
                                  d[i1][j1] + (i - i1 - 1) + cost_tra + 
                                  (j - j1 - 1));
        }
        alph[x.str.s[i - 1]] = i;
    }

    float r = d[x.len + 1][y.len + 1];
    switch (norm) {
    case NORM_MIN:
        return r / fmin(x.len, y.len);
    case NORM_MAX:
        return r / fmax(x.len, y.len);
    case NORM_AVG:
        return r / (0.5 * (x.len + y.len));
    case NORM_NONE:
    default:
        return r;
    }
}

/** @} */
