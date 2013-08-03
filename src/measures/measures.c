/*
 * Harry - Similarity Measures for Strings
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
#include "measures.h"
#include "util.h"
#include "input.h"

/**
 * Structure for measure interface
 */
typedef struct
{
    /** Init function */
    void (*measure_init) ();
    /** Comparison function */
    float (*measure_compare) (string_t *, string_t *);
    /** Free function */
    void (*measure_free) (void *);
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
    char *c, *s;
    const char *name = mod;

    /* Match entire measure */
    if(!strcasecmp(str, name))
        return TRUE;
        
    /* Match last component of measure path */
    c = strrchr(name, '.');
    if (c && strlen(c) > 1) {
        name = c + 1;
        if (!strcasecmp(str, name))
            return TRUE;
    }
    
    /* Swap elements around first '_' */
    c = strchr(name, '_');
    if (c && strlen(c) > 1) {
        s = strdup(name);
        snprintf(s, strlen(name) + 1, "%s_%s", c + 1, name);
        int match = !strcasecmp(str, s);
        free(s);
        if (match)
            return TRUE;
    }
    
    return FALSE;
}

/**
 * Configures the measure for a given similarity measure.
 * @param name Name of similarity measure
 */
void measure_config(const char *name)
{
    if (measure_match(name, "xxx")) {
        func.measure_init = NULL;
        func.measure_compare = NULL;
        func.measure_free = NULL;
    } else {
        error("Unknown measure '%s', using 'xxx' instead.", name);
        measure_config("xxx");
    }
}

/**
 * Init comparison measure
 */
void measure_init()
{
    func.measure_init();
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
 * Frees the memory of a similarity measure
 * @param x object
 */
void measure_free(void *x)
{
    func.measure_free(x);
}

/** @} */
