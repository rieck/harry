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
#include "strutil.h"
#include "util.h"

/**
 * @addtogroup strings
 * <hr>
 * <b>Module dist_hamming</b>: Hamming distance for strings.
 *
 * The module contains supports reading strings and computing their Hamming
 * distance.  The strings need to be terminated by a newline character and
 * must not contain the NUL character.  Additionally, strings must not start
 * with 0xff in marker mode.  The distance is computed over the characters
 * of the strings.
 * 
 * @author Konrad Rieck (konrad@mlsec.org) 
 * @{
 */

/* Global configuration */
enum norm_type { NORM_NONE, NORM_MIN, NORM_MAX, NORM_AVG };
static enum norm_type norm = NORM_NONE;

/**
 * Initializes the module
 * @param c Configuration
 */
void dist_hamming_init(config_t *c)
{
    const char *str;

    /* Normalization */
    config_lookup_string(c, "strings.dist_hamming.norm", &str);    
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
 * different lengths, the remaining characters of the longer string are
 * considered mismatches.
 * @param x first string 
 * @param y second string
 * @return Hamming distance
 */
double dist_hamming_cmp(void *x, void *y)
{
    assert(x && y);
    double d = 0;
    int i, lx = strlen(x), ly = strlen(y);

    /* Loop over strings */
    for (i = 0; i < lx && i < ly; i++)
        if (((char *) x)[i] != ((char *) y)[i])
            d += 1;

    /* Add remaining characters as mismatches */
    if (lx != ly)
        d += fabs(lx - ly);

    switch(norm) {
    case NORM_MIN:
	return d / fmin(lx, ly);
    case NORM_MAX:
	return d / fmax(lx, ly);
    case NORM_AVG:
	return d / ( 0.5 * (lx + ly));
    case NORM_NONE:
    default:
    	return d;
    }
}

/**
 * Reads a string from a file stream. Reading stops when a newline character
 * is found, at end-of-file or error.  The function allocates memory that
 * needs to be freed later using dist_hamming_free.  
 * @param f file stream 
 * @return string
 */
void *dist_hamming_read(FILE *f)
{
    return str_read(f);
}

/**
 * Frees the memory of a string. 
 * @param x string
 */
void dist_hamming_free(void *x)
{
    str_free(x);
}

/** @} */
