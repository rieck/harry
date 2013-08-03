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
 * <b>Module dist_edit</b>: Edit distance for strings.
 *
 * The module supports reading strings and computing their edit distance
 * (Levenshtein distance).  The strings need to be terminated by a newline
 * character and must not contain the NUL character.  Additionally, strings
 * must not start with 0xff in marker mode.  The distance is computed over
 * the characters of the strings.
 *  
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

/* Global configuration */
enum norm_type { NORM_NONE, NORM_MIN, NORM_MAX, NORM_AVG };
static enum norm_type norm = NORM_NONE;
static double cost_ins = 1.0;
static double cost_del = 1.0;
static double cost_sub = 1.0;

/**
 * Initializes the module
 * @param c Configuration
 */
void dist_edit_init(config_t *c)
{
    const char *str;

    /* Costs */
    config_lookup_float(c, "strings.dist_edit.cost_ins", &cost_ins);
    config_lookup_float(c, "strings.dist_edit.cost_del", &cost_del);    
    config_lookup_float(c, "strings.dist_edit.cost_sub", &cost_sub);
    
    /* Normalization */
    config_lookup_string(c, "strings.dist_edit.norm", &str);    
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
 * Computes the edit distance of two strings. Implementation adapted
 * from C# code by Stephen Toub.
 * @param x first string 
 * @param y second string
 * @return edit distance
 */
double dist_edit_cmp(void *x, void *y)
{
    assert(x && y);
    int i, j, a, b, lx = strlen(x), ly = strlen(y);
    char *sx = (char *) x, *sy = (char *) y;

    if (lx == 0 && ly == 0)
        return 0;
    
    /* 
     * Rather than maintain an entire matrix (which would require O(n*m)
     * space), just store the current row and the next row, each of which
     * has a length m+1, so just O(m) space.  Initialize the curr row.
     */
    int curr = 0, next = 1;
    int rows[2][ly + 1];

    for (j = 0; j <= ly; j++)
        rows[curr][j] = j;

    /* For each virtual row (we only have physical storage for two) */
    for (i = 1; i <= lx; i++) {

        /* Fill in the values in the row */
        rows[next][0] = i;
        for (j = 1; j <= ly; j++) {
        
            /* Insertion and deletion */
            a = rows[curr][j] + cost_ins;
            b = rows[next][j - 1] + cost_del;
            if (a > b)
                a = b;

            /* Substituion */
            b = rows[curr][j - 1] + (sx[i - 1] == sy[j - 1] ? 0 : cost_sub);
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

    switch(norm) {
    case NORM_MIN:
	return (double) rows[curr][ly] / fmin(lx, ly);
    case NORM_MAX:
	return (double) rows[curr][ly] / fmax(lx, ly);
    case NORM_AVG:
	return (double) rows[curr][ly] / ( 0.5 * (lx + ly));
    case NORM_NONE:
    default:
    	return (double) rows[curr][ly];
    }
}

/**
 * Reads a string from a file stream. Reading stops when a newline character
 * is found, at end-of-file or error.  The function allocates memory that
 * needs to be freed later using levensthein_free.
 * @param f file stream
 * @return string
 */
void *dist_edit_read(FILE *f)
{
    return str_read(f);
}

/**
 * Frees the memory of a string. 
 * @param x string
 */
void dist_edit_free(void *x)
{
    str_free(x);
}

/** @} */
