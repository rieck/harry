/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2006 Stephen Toub 
 * Copyright (C) 2002-2003 David Necas (Yeti) <yeti@physics.muni.cz>.
 * Copyright (C) 2013-2014  Konrad Rieck (konrad@mlsec.org)
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
#include "dist_levenshtein.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_levenshtein</em>: Levenshtein distance for strings.
 *
 * Levenshtein. Binary codes capable of correcting deletions, insertions,
 * and reversals. Doklady Akademii Nauk SSSR, 163 (4):845-848, 1966.
 * @{
 */

/* Normalizations */
static lnorm_t n = LN_NONE;
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
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_ins",
                        &cost_ins);
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_del",
                        &cost_del);
    config_lookup_float(&cfg, "measures.dist_levenshtein.cost_sub",
                        &cost_sub);

    /* Normalization */
    config_lookup_string(&cfg, "measures.dist_levenshtein.norm", &str);
    n = lnorm_get(str);
}




/**
 * Computes the Levenshtein distance. Code adapted from
 * David Necas (Yeti) <yeti@physics.muni.cz>.
 * @param x first string
 * @param y second string
 * @return Levenshtein distance
 */
static float dist_levenshtein_compare_yeti(hstring_t x, hstring_t y)
{
    int i, *end, half;
    int *row; /* we only need to keep one row of costs */

    /* Catch trivial cases */
    if (x.len == 0)
        return y.len ;
    if (y.len  == 0)
        return x.len;

    /* Make the inner cycle (i.e. string2) the longer one */
    if (x.len > y.len ) {
        hstring_t z = x;
        x = y;
        y = z;
    }

    /* Check x.len == 1 separately */
    if (x.len == 1) {
	int c = 0;
	for (int k = 0; !c && k < y.len; k++) {
	    if (!hstring_compare(x, 0, y, k))
		c = 1;
	}
	return y.len - c;
    }

    x.len++;
    y.len++;
    half = x.len >> 1;

    /* Unitalize first row */
    row = (int *) malloc((y.len ) * sizeof(int));
    if (!row) {
        error("Failed to allocate memory for Levenshtein distance");
        return 0;
    }

    end = row + y.len  - 1;
    for (i = 0; i < y.len  - half; i++)
        row[i] = i;

    /*
     * We don't have to scan two corner triangles (of size x.len/2) in the
     * matrix because no best path can go throught them.  Note this breaks
     * when x.len == y.len == 2 so special case above is necessary
     */
    row[0] = x.len  - half - 1;
    for (i = 1; i < x.len ; i++) {
        int *p;
        int char1p = i - 1;
        int char2p;
        int D, k;
        /* skip the upper triangle */
        if (i >= x.len  - half) {
            int offset = i - (x.len  - half);
            int c3;

            char2p = offset;
            p = row + offset;
            c3 = *(p++) + (hstring_compare(x, char1p, y, char2p++) ? 1 : 0);
            k = *p;
            k++;
            D = k;
            if (k > c3)
                k = c3;
            *(p++) = k;
        } else {
            p = row + 1;
            char2p = 0;
            D = k = i;
        }
        /* skip the lower triangle */
        if (i <= half + 1)
            end = row + y.len  + i - half - 2;
        /* main */
        while (p <= end) {
            int c3 = --D + (hstring_compare(x, char1p, y, char2p++) ? 1 : 0);
            k++;
            if (k > c3)
                k = c3;
            D = *p;
            D++;
            if (k > D)
                k = D;
            *(p++) = k;
        }
        /* lower triangle sentinel */
        if (i <= half) {
            int c3 = --D + (hstring_compare(x, char1p, y, char2p) ? 1 : 0);
            k++;
            if (k > c3)
                k = c3;
            *p = k;
        }
    }

    i = *end;
    free(row);
    return i;
}

/* Ugly macros to access arrays */
#define ROWS(i,j)	rows[(i) * (y.len + 1) + (j)]

/**
 * Computes the Levenshtein distance of two strings. 
 * Adapted from Stephen Toub's C# implementation. 
 * http://blogs.msdn.com/b/toub/archive/2006/05/05/590814.aspx
 * @param x first string
 * @param y second string
 * @return Levenshtein distance
 */
static float dist_levenshtein_compare_toub(hstring_t x, hstring_t y)
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
    int *rows = malloc(sizeof(int) * (y.len + 1) * 2);
    if (!rows) {
        error("Failed to allocate memory for Levenshtein distance");
        return 0;
    }

    for (j = 0; j <= y.len; j++)
         ROWS(curr,j) = j;

    /* For each virtual row (we only have physical storage for two) */
    for (i = 1; i <= x.len; i++) {

        /* Fill in the values in the row */
        ROWS(next,0) = i;
        for (j = 1; j <= y.len; j++) {

            /* Insertion and deletion */
            a = ROWS(curr,j) + cost_ins;
            b = ROWS(next, j - 1) + cost_del;
            if (a > b)
                a = b;

            /* Substitution */
            b = ROWS(curr, j - 1) +
                (hstring_compare(x, i - 1, y, j - 1) ? cost_sub : 0);

            if (a > b)
                a = b;

            /* 
             * Transpositions (Damerau-Levenshtein) are not supported by
             * this implementation, as only two rows of the distance matrix
             * are available. Potential fix: provide three rows.
             */
            ROWS(next, j) = a;
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
    double d = ROWS(curr, y.len);

    /* Free memory */
    free(rows);

    return d;
}


/**
 * Computes the Levenshtein distance. Wrapper function.
 * @param x first string
 * @param y second string
 * @return Levenshtein distance
 */
float dist_levenshtein_compare(hstring_t x, hstring_t y)
{
    float f;

    /*
     * If the costs of all edit operations are equal we use the fast
     * implementation by David Necas, otherwise we switch to the 
     * variant by Stephen Toub.
     */
    if (cost_ins == cost_del && cost_del == cost_sub)
        f = cost_ins * dist_levenshtein_compare_yeti(x, y);
    else
        f = dist_levenshtein_compare_toub(x,y);

    return lnorm(n, f, x, y);
}

/** @} */
