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
 * @defgroup matrix Matrix object
 * Functions for processing similarity values in a symmetric matrix
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
extern int log_line;
extern config_t cfg;

/**
 * Initialize a matrix for similarity values
 * @param s Array of string objects
 * @param n Number of string objects 
 * @return Matrix object
 */
hmatrix_t *hmatrix_init(hstring_t *s, int n)
{
    assert(s && n >= 0);

    hmatrix_t *m = malloc(sizeof(hmatrix_t));
    if (!m) {
        error("Could not allocate matrix object");
        return NULL;
    }

    /* Set default ranges */
    m->num = n;
    m->x.i = 0, m->x.n = n;
    m->y.i = 0, m->y.n = n;
    m->triangular = TRUE;
    
    /* Initialized later */
    m->values = NULL;

    /* Allocate some space */
    m->labels = calloc(n, sizeof(float));
    m->srcs = calloc(n, sizeof(char *));
    if (!m->srcs || !m->labels) {
        error("Failed to initialize matrix for similarity values");
        return m;
    }

    /* Copy details from strings */
    for (int i = 0; i < n; i++) {
        m->labels[i] = s[i].label;
        m->srcs[i] = s[i].src ? strdup(s[i].src) : NULL;
    }

    return m;
}

/**
 * Parse a range string 
 * @param r Range object
 * @param str Range string, e.g. 3:14 or 2:-1 or :
 * @param n Maximum size
 * @return Range object
 */
static range_t parse_range(range_t r, char *str, int n)
{
    char *ptr, *end = NULL;
    long l;

    /* Empty string */
    if (strlen(str) == 0)
        return r;

    /* 
     * Since "1:1", "1:", ":1"  and ":" are all valid indices, sscanf 
     * won't do it and we have to stick to manual parsing :(
     */
    ptr = strchr(str, ':');
    if (!ptr) {
        error("Invalid range string '%s'.", str);
        return r;
    } else {
        /* Create split */
        *ptr = '\0';
    }

    /* Start parsing */
    l = strtol(str, &end, 10);
    if (strlen(str) == 0)
        r.i = 0;
    else if (*end == '\0')
        r.i = (int) l;
    else
        error("Could not parse range '%s:...'.", str);

    l = strtol(ptr + 1, &end, 10);
    if (strlen(ptr + 1) == 0)
        r.n = n;
    else if (*end == '\0')
        r.n = (int) l;
    else
        error("Could not parse range '...:%s'.", ptr + 1);

    /* Support negative end index */
    if (r.n < 0) {
        r.n = n + r.n;
    }

    /* Sanity checks */
    if (r.n < 0 || r.i < 0 || r.n > n || r.i > n - 1 || r.i >= r.n) {
        error("Invalid range '%s:%s'. Using default '0:%d'.", str, ptr + 1, n);
        r.i = 0, r.n = n;
    }

    return r;
}

/**
 * Set the x range for computation
 * @param m Matrix object
 * @param x String for x range 
 */
void hmatrix_xrange(hmatrix_t *m, char *x)
{
    assert(m && x);
    m->x = parse_range(m->x, x, m->num);
}

/**
 * Set the y range for computation
 * @param m Matrix object
 * @param y String for y range 
 */
void hmatrix_yrange(hmatrix_t *m, char *y)
{
    assert(m && y);
    m->y = parse_range(m->y, y, m->num);
}

/** 
 * Allocate memory for matrix
 * @param m Matrix object
 * @return pointer to floats
 */
float *hmatrix_alloc(hmatrix_t *m)
{
    if (m->x.n == m->y.n && m->x.i == m->y.i && m->x.i == 0) {
        /* Full matrix -> allocate triangle */
        m->triangular = TRUE;
        m->size = (m->x.n * (m->x.n - 1) / 2 + m->x.n);
    } else {
        /* Patrial matrix -> allocate rectangle */
        m->triangular = FALSE;
        m->size = (m->x.n - m->x.i) * (m->y.n - m->y.i);
    }

    /* Allocate memory */
    m->values = calloc(sizeof(float), m->size);
    if (!m->values) {
        error("Could not allocate matrix for similarity values");
        return NULL;
    }

    return m->values;
}

/**
 * Set a value in the matrix
 * @param m Matrix object
 * @param x Coordinate x
 * @param y Coordinate y
 * @param f Value
 */
void hmatrix_set(hmatrix_t *m, int x, int y, float f)
{
    int idx, i, j;

    if (m->triangular) {
        if (x > y) {
            i = y, j = x;
        } else {
            i = x, j = y;
        }
        idx = ((j - i) + i * m->x.n - i * (i - 1) / 2);
    } else {
        idx = (x - m->x.i) + (y - m->y.i) * (m->x.n - m->x.i);
    }
    
    assert(idx < m->size);
    m->values[idx] = f;
}


/**
 * Get a value from the matrix
 * @param m Matrix object
 * @param x Coordinate x
 * @param y Coordinate y
 * @return f Value
 */
float hmatrix_get(hmatrix_t *m, int x, int y)
{
    int idx, i, j;

    if (m->triangular) {
        if (x > y) {
            i = y, j = x;
        } else {
            i = x, j = y;
        }
        idx = ((j - i) + i * m->x.n - i * (i - 1) / 2);
    } else {
        idx = (x - m->x.i) + (y - m->y.i) * (m->x.n - m->x.i);    
    }
    
    assert(idx < m->size);
    return m->values[idx];
}

/**
 * Compute similarity measure and fill matrix
 * @param m Matrix object
 * @param s Array of string objects
 * @param measure Similarity measure 
 */
void hmatrix_compute(hmatrix_t *m, hstring_t *s,
                     double (*measure) (hstring_t x, hstring_t y))
{
    int i, k = 0;
    int step1 = floor(m->size * 0.01) + 1;
    float ts1 = 0, ts2 = 0;

    /*
     * It seems that the for-loop has to start at index 0 for OpenMP to 
     * collapse both loops. This renders it a little ugly, since hmatrix 
     * requires absolute indices.
     */
#pragma omp parallel for collapse(2) firstprivate(ts1, ts2)
    for (i = 0; i < m->x.n - m->x.i; i++) {
        for (int j = 0; j < m->y.n - m->y.i; j++) {
        
            /* Skip symmetric values */
            if (m->triangular && j < i)
                continue;

            /* First iteration */
            if (k == 0) {
                ts1 = time_stamp();
                ts2 = time_stamp();
            }

            float f = measure(s[i + m->x.i], s[j + m->y.i]);
            
            /* Set value in matrix */
            hmatrix_set(m, i + m->x.i, j + m->y.i, f);

            /* Update progress bar every 100th step and 100ms */
            if (verbose && (k % step1 == 0 || time_stamp() - ts1 > 0.1)) {
                prog_bar(0, m->size, k);
                ts1 = time_stamp();
            }

            /* Print log line every minute if enabled */
            if (log_line && time_stamp() - ts2 > 60) {
                log_print(0, m->size, k);
                ts2 = time_stamp();
            }

            k++;
        }
    }

    if (verbose) {
        prog_bar(0, m->size, m->size);
    }

    if (log_line) {
        log_print(0, m->size, m->size);
    }
}


/**
 * Destroy a matrix of simililarity values and free its memory
 * @param m Matrix object
 */
void hmatrix_destroy(hmatrix_t *m)
{
    if (!m)
        return;

    if (m->values)
        free(m->values);
    for (int i = 0; m->srcs && i < m->num; i++)
        if (m->srcs[i])
            free(m->srcs[i]);

    if (m->srcs)
        free(m->srcs);
    if (m->labels)
        free(m->labels);

    free(m);
}

/** @} */
