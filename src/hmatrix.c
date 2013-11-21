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
 * @defgroup matrix Matrix functions
 * Functions for processing similarity values in a matrix
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "hstring.h"
#include "murmur.h"
#include "hmatrix.h"

/* External variable */
extern int verbose;
extern config_t cfg;

/**
 * Initialize a matrix for similarity values
 * @param m Matrix structure
 * @param s Array of string objects
 * @param n Number of string objects 
 * @return Matrix structure
 */
hmatrix_t hmatrix_init(hmatrix_t m, hstring_t *s, int n)
{
    assert(s && n >= 0);

    /* Set default ranges */
    m.num = n;
    m.x.i = 0;
    m.x.n = n;
    m.y.i = 0;
    m.y.n = n;

    /* Initialized later */
    m.values = NULL;

    /* Allocate some space */
    m.labels = calloc(n, sizeof(float));
    m.srcs = calloc(n, sizeof(char *));
    if (!m.srcs || !m.labels) {
        error("Failed to initialize matrix for similarity values");
        return m;
    }

    /* Copy details from strings */
    for (int i = 0; i < n; i++) {
        m.labels[i] = s[i].label;
        m.srcs[i] = s[i].src ? strdup(s[i].src) : NULL;
    }

    return m;
}

/**
 * Parse a range string 
 * @param r Range structure
 * @param str Range string, e.g. 3:14 or 2:-1 or :
 * @param n Maximum size
 * @return Range structure
 */
static range_t parse_range(range_t r, char *str, int n)
{
    char *end = NULL;
    char *ptr = strchr(str, ':');
    long l;

    if (!ptr) {
        error("Invalid range string '%s'.", str);
        return r;
    }

    /* Create split */
    *ptr = '\0';

    /* Start parsing */
    l = strtol(str, &end, 10);
    if (strlen(str) == 0) 
        r.i = 0;
    else if (end == '\0') 
        r.i = (int) l;
    else
        error("Could not parse range '%s'.", str);

    l = strtol(ptr+1, &end, 10);
    if (strlen(ptr+1) == 0)
        r.n = n;
    else if (end == '\0') 
        r.n = (int) l;
    else
        error("Could not parse range '%s'.", str);

    /* Support negative end index */
    if (r.n < 0) {
        r.n = n - r.n;
    }

    /* Sanity checks */
    if (r.n < 0 || r.i < 0 || r.n > n || r.i > n - 1 || r.i >= r.n) {
        error("Invalid range '%s'. Using default '0:%d'.", str, n);
        r.i = 0, r.n = n;
    }

    return r;
}

/**
 * Set the x and y range for computation
 * @param m Matrix structure
 * @param x String for x range 
 * @param y String for y range
 * @return Matrix structure
 */
hstring_t hmatrix_range(hmatrix_t m, char *x, char *y)
{
    m.x = parse_range(m.x, x, m.num);
    m.y = parse_range(m.y, y, m.num);
    return m;
}

/** 
 * Allocate memory for matrix
 * @param m Matrix structure
 * @return Matrix structure
 */
hmatrix_t hmatrix_alloc(hmatrix_t m)
{
    int size = (m.x.n - m.x.i) * (m.y.n - m.y.i);

    m.values = calloc(sizeof(float), size);
    if (!m.values) 
        error("Could not allocate matrix for similarity values");
    return m;
}

/**
 * Compute similarity measure and fill matrix
 * @param m Matrix structure
 * @param s Array of string objects
 * @param f Similarity measure 
 * @return Matrix structure
 */
hmatrix_t hmatrix_compute(hmatrix_t m, hstring_t *s, 
                          float (*measure)(hstring_t x, hstring_t y))
{
    for (int i = x.i; i < x.n; i++) {
        for (int j = y.i; j < y.n; j++) {
            float f = measure(s[i], s[j]);
            m.values[i - m.x.i][j - m.y.i] = f;
        }
    }
}
 

/**
 * Destroy a matrix of simililarity values and free its memory
 * @param m Matrix structure
 */
void hmatrix_destroy(hmatrix_t m)
{
    if (m.values)
        free(m.values);
    for (int i = 0; m.srcs && i < m.num; i++) 
        if (m.srcs[i])
            free(m.srcs[i]);
    
    if (m.srcs)
        free(m.srcs);
    if (m.labels)
        free(m.labels);
}

/** @} */
