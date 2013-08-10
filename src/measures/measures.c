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
 * @defgroup measure Module interface
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"
#include "measures.h"

/* Similarity measures */
#include "dist_hamming.h"

/**
 * Structure for measure interface
 */
typedef struct
{
    /** Init function */
    void (*measure_config) ();
    /** Comparison function */
    float (*measure_compare) (string_t *, string_t *);
} func_t;
static func_t func;

/* Global delimiter table */
char delim[256] = { DELIM_NOT_INIT };

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
    if (measure_match(name, "dist_hamming")) {
        func.measure_config = dist_hamming_config;
        func.measure_compare = dist_hamming_compare;
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
double measure_compare(string_t *x, string_t *y)
{
    return func.measure_compare(x, y);
}

/**
 * Decodes a string containing delimiters to a lookup table
 * @param s String containing delimiters
 */
void measure_delim_set(const char *s)
{
    char buf[5] = "0x00";
    unsigned int i, j;

    memset(delim, 0, 256);
    for (i = 0; i < strlen(s); i++) {
        if (s[i] != '%') {
            delim[(unsigned int) s[i]] = 1;
            continue;
        }

        /* Skip truncated sequence */
        if (strlen(s) - i < 2)
            break;

        buf[2] = s[++i];
        buf[3] = s[++i];
        sscanf(buf, "%x", (unsigned int *) &j);
        delim[j] = 1;
    }
}

/**
 * Resets delimiters table. There is a global table of delimiter 
 * symbols which is only initialized once the first sequence is 
 * processed. This functions is used to trigger a re-initialization.
 */
void measure_delim_reset()
{
    delim[0] = DELIM_NOT_INIT;
}


/** @} */
