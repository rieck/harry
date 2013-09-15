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

/**
 * @defgroup measures Measure interface
 *
 * Interface and functions for computing similarity measures for strings
 *
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"
#include "measures.h"
#include "vcache.h"

/* Similarity measures */
#include "dist_hamming.h"
#include "dist_levenshtein.h"
#include "dist_jarowinkler.h"
#include "dist_lee.h"
#include "dist_damerau.h"
#include "dist_compression.h"
#include "kern_wdegree.h"
#include "kern_subsequence.h"

/* External variables */
extern config_t cfg;

/* Global variables */
static int global_cache;

/**
 * Structure for measure interface
 */
typedef struct
{
    /** Init function */
    void (*measure_config) ();
    /** Comparison function */
    float (*measure_compare) (hstring_t, hstring_t);
} func_t;
static func_t func;

/**
 * Match a string against a measure name
 * @param str string
 * @param mod measure name 
 * @return true if string matches (fuzzily)
 */
static int measure_match(const char *str, const char *mod)
{
    assert(str && mod);
    const char *name = mod;

    /* Match entire measure */
    if (!strcasecmp(str, name))
        return TRUE;

    /* Add fuzzy matching */
    return FALSE;
}

/**
 * Configures the measure for a given similarity measure.
 * @param name Name of similarity measure
 */
void measure_config(const char *name)
{
    const char *cfg_str;

    /* Set delimiters */
    config_lookup_string(&cfg, "measures.delim", &cfg_str);
    if (strlen(cfg_str) > 0)
        hstring_delim_set(cfg_str);
    else
        hstring_delim_reset();

    /* Enable global cache */
    config_lookup_int(&cfg, "measures.global_cache", &global_cache);

    if (measure_match(name, "dist_hamming")) {
        func.measure_config = dist_hamming_config;
        func.measure_compare = dist_hamming_compare;
    } else if (measure_match(name, "dist_levenshtein")) {
        func.measure_config = dist_levenshtein_config;
        func.measure_compare = dist_levenshtein_compare;
    } else if (measure_match(name, "dist_jarowinkler")) {
        func.measure_config = dist_jarowinkler_config;
        func.measure_compare = dist_jarowinkler_compare;
    } else if (measure_match(name, "dist_lee")) {
        func.measure_config = dist_lee_config;
        func.measure_compare = dist_lee_compare;
    } else if (measure_match(name, "dist_damerau")) {
        func.measure_config = dist_damerau_config;
        func.measure_compare = dist_damerau_compare;
    } else if (measure_match(name, "dist_compression")) {
        func.measure_config = dist_compression_config;
        func.measure_compare = dist_compression_compare;
    } else if (measure_match(name, "kern_wdegree")) {
        func.measure_config = kern_wdegree_config;
        func.measure_compare = kern_wdegree_compare;
    } else if (measure_match(name, "kern_subsequence")) {
        func.measure_config = kern_subsequence_config;
        func.measure_compare = kern_subsequence_compare;
    } else {
        error("Unknown measure '%s', using 'dist_hamming' instead.", name);
        measure_config("dist_hamming");
    }

    func.measure_config();
}

/**
 * Compares two strings with the given similarity measure.
 * @param x first string
 * @param y second second
 * @return similarity/dissimilarity value
 */
double measure_compare(hstring_t x, hstring_t y)
{
    int ret;

    if (!global_cache)
        return func.measure_compare(x,y);

    uint64_t xyk = hstring_hash2(x, y);
    float m = 0;

    #pragma omp critical (vcache)
    ret = vcache_load(xyk, &m);

    if (!ret) {
        m = func.measure_compare(x, y);
        #pragma omp critical (vcache)
        vcache_store(xyk, m);
    }
    return m;
}

/** @} */
