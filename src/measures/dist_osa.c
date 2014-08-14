/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2014 Konrad Rieck (konrad@mlsec.org)
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
#include "dist_osa.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_osa</em>: Optimal sequence alignment (OSA) distance 
 *
 * Doolittle. Of Urfs and Orfs: A Primer on How to Analyze Derived Amino
 * Acid Sequences. University Science Books, 1986 
 @{
 */

/* Normalizations */
static lnorm_t n = LN_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;
static double cost_tra = 1.0;

/* External variables */
extern config_t cfg;

/**
 * Initializes the similarity measure
 */
void dist_osa_config()
{
    const char *str;

    /* Costs */
    config_lookup_float(&cfg, "measures.dist_osa.cost_ins", &cost_ins);
    config_lookup_float(&cfg, "measures.dist_osa.cost_del", &cost_del);
    config_lookup_float(&cfg, "measures.dist_osa.cost_sub", &cost_sub);
    config_lookup_float(&cfg, "measures.dist_osa.cost_tra", &cost_tra);

    /* Normalization */
    config_lookup_string(&cfg, "measures.dist_osa.norm", &str);
    n = lnorm_get(str);
}

/* Ugly macros to access arrays */
#define D(i,j) 		d[(i) * (y.len + 1) + (j)]

/**
 * Computes the OSA distance of two strings. 
 * @param x first string 
 * @param y second string
 * @return OSA distance
 */
float dist_osa_compare(hstring_t x, hstring_t y)
{
    int i, j, a, b, c;

    if (x.len == 0 && y.len == 0)
        return 0;

    /* Allocate matrix. We might reduce this to some rows only */
    int *d = (int *) calloc((x.len + 1) * (y.len + 1), sizeof(int));

    /* Init margin of matrix */
    for (i = 0; i <= x.len; i++)
        D(i, 0) = i * cost_ins;
    for (j = 0; j <= y.len; j++)
        D(0, j) = j * cost_ins;

    for (i = 1; i <= x.len; i++) {
        for (j = 1; j <= y.len; j++) {

            /* Comparison */
            c = hstring_compare(x, i - 1, y, j - 1);

            /* Insertion an deletion */
            a = D(i - 1, j) + cost_ins;
            b = D(i, j - 1) + cost_del;
            if (a > b)
                a = b;

            /* Substitution */
            b = D(i - 1, j - 1) + (c ? cost_sub : 0);
            if (a > b)
                a = b;

            /* Transposition */
            if (i > 1 && j > 1 && 
                !hstring_compare(x, i - 1, y, j - 2) &&
                !hstring_compare(x, i - 2, y, j - 1)) {
                b = D(i - 2, j - 2) + (c ? cost_tra : 0);
                if (a > b)
                    a = b;
            }

            /* Update matrix */
            D(i, j) = a;
        }
    }

    double m = D(x.len, y.len);
    free(d);

    return lnorm(n, m, x, y);
}

/** @} */
