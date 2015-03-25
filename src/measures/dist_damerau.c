/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2006 Stephen Toub 
 *               2013-2015  Konrad Rieck (konrad@mlsec.org)
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
static lnorm_t n = LN_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;
static double cost_tra = 1.0;

/* External variables */
extern config_t cfg;

/* Symbols hash table */
typedef struct
{
    sym_t sym;          /**< Symbol (key) */
    int val;            /**< Value associated with symbol */
    UT_hash_handle hh;  /**< Makes struct hashable */
} sym_hash_t;

/*
 * Four-way minimum
 */
static float min(float a, float b, float c, float d)
{
    return fmin(fmin(a, b), fmin(c, d));
}

/**
 * Get value associated with a symbol from the hash
 * @param hash hash table
 * @param s symbol
 * @return value
 */
static int hash_get(sym_hash_t **hash, sym_t s)
{
    sym_hash_t *entry = NULL;

    HASH_FIND(hh, *hash, &s, sizeof(sym_t), entry);
    if (!entry) {
        entry = malloc(sizeof(sym_hash_t));
        entry->sym = s;
        entry->val = 0;
        HASH_ADD(hh, *hash, sym, sizeof(sym_t), entry);
    }

    return entry->val;
}

/**
 * Set the value of a symbol in the hash
 * @param hash hash table
 * @param s symbol
 * @param val value
 */
static void hash_set(sym_hash_t **hash, sym_t s, int val)
{
    sym_hash_t *entry = NULL;

    HASH_FIND(hh, *hash, &s, sizeof(sym_t), entry);
    if (!entry) {
        entry = malloc(sizeof(sym_hash_t));
        entry->sym = s;
        entry->val = val;
        HASH_ADD(hh, *hash, sym, sizeof(sym_t), entry);
    }

    entry->val = val;
}

/**
 * Destroy hash table
 * @param hash hash table
 */
static void hash_destroy(sym_hash_t **hash)
{
    sym_hash_t *entry;

    while (*hash) {
        entry = *hash;
        HASH_DEL(*hash, entry);
        free(entry);
    }
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
    n = lnorm_get(str);
}

/* Ugly macros to access arrays */
#define D(i,j)       d[(i) * (y.len + 2) + (j)]

/**
 * Computes the Damerau-Levenshtein distance of two strings. Adapted from 
 * Wikipedia entry and comments from Stackoverflow.com
 * @param x first string 
 * @param y second string
 * @return Levenshtein distance
 */
float dist_damerau_compare(hstring_t x, hstring_t y)
{
    sym_hash_t *shash = NULL;
    int i, j, inf = x.len + y.len;

    if (x.len == 0 && y.len == 0)
        return 0;

    /* Allocate table for dynamic programming */
    int *d = malloc((x.len + 2) * (y.len + 2) * sizeof(int));
    if (!d) {
        error("Could not allocate memory for Damerau-Levenshtein distance");
        return 0;
    }

    /* Initialize distance matrix */
    D(0, 0) = inf;
    for (i = 0; i <= x.len; i++) {
        D(i + 1, 1) = i;
        D(i + 1, 0) = inf;
    }
    for (j = 0; j <= y.len; j++) {
        D(1, j + 1) = j;
        D(0, j + 1) = inf;
    }

    for (i = 1; i <= x.len; i++) {
        int db = 0;
        for (j = 1; j <= y.len; j++) {
            int i1 = hash_get(&shash, hstring_get(y, j - 1));
            int j1 = db;
            int dz = hstring_compare(x, i - 1, y, j - 1) ? cost_sub : 0;
            if (dz == 0)
                db = j;

            D(i + 1, j + 1) = min(D(i, j) + dz,
                                  D(i + 1, j) + cost_ins,
                                  D(i, j + 1) + cost_del,
                                  D(i1, j1) + (i - i1 - 1) + cost_tra +
                                  (j - j1 - 1));
        }

        hash_set(&shash, hstring_get(x, i - 1), i);
    }

    float r = D(x.len + 1, y.len + 1);

    /* Free memory */
    free(d);
    hash_destroy(&shash);

    return lnorm(n, r, x, y);
}

/** @} */
