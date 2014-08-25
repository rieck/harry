/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
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
    m->size = 0;
    m->calcs = 0;

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

    /* Support negative start and end indices */
    if (r.i < 0)
        r.i = n + r.i;
    if (r.n < 0)
        r.n = n + r.n;

    /* Sanity checks */
    if (r.n < 0 || r.i < 0 || r.n > n || r.i > n - 1 || r.i >= r.n) {
        error("Invalid range '%s:%s'. Using default '0:%d'.", str, ptr + 1,
              n);
        r.i = 0, r.n = n;
    }

    return r;
}

/**
 * Infer the matrix' detailed specifications. This can be used
 * to accurately determine the number of \b unique values.
 *
 * @param[in] m Matrix object.
 * @param[out] spec The struct holding the specification.
 */
void hmatrix_inferspec(const hmatrix_t *m, hmatrixspec_t *spec)
{
	assert(m != NULL && spec != NULL);

    const int width = RANGE_LENGTH(m->x);
    const int height = RANGE_LENGTH(m->y);

    const range_t x = m->x;
    const range_t y = m->y;

    spec->b_top = MAX(x.i -y.i, 0);
    spec->b_bottom = MAX(y.n -x.n, 0);
    spec->b_left = MAX(y.i -x.i, 0);
    spec->b_right = MAX(x.n -y.n, 0);
    spec->a = (y.i >= x.n && x.n <= y.i) || (y.n <= x.i && x.i >= y.n) ? 0 : (height -spec->b_top -spec->b_bottom);

    assert(spec->b_left + spec->a + spec->b_right == width);

    spec->n_top = width *spec->b_top;
    spec->n_mid = spec->a*spec->b_left + (pow(spec->a, 2) +spec->a) /2 + spec->a*spec->b_right;
    spec->n_bottom = width *spec->b_bottom;
    spec->n = spec->n_top + spec->n_mid + spec->n_bottom;
}

//#define USE_UNIFORM_SPLITTING

/**
 * Enable splitting matrix
 * @param m Matrix object
 * @param str Split string
 */
void hmatrix_split(hmatrix_t *m, char *str)
{
    /* Empty string */
    if (strlen(str) == 0)
        return;

    /* Parse split string */
    int blocks, index;
    if (sscanf(str, "%d:%d", &blocks, &index) != 2) {
        fatal("Invalid split string '%s'.", str);
        return;
    }

    /* Sanity checks with fatal error */
    if (index < 0 || index >= blocks) {
        fatal("Block index out of range (%d).", index);
        return;
    }

    hmatrix_split_ex(m, blocks, index);
}

#ifdef USE_UNIFORM_SPLITTING
/**
 * Determine the column slice 0:x that approximately holds
 * given the number of \b unique values.
 *
 * @param[in] N The approximate number of \b unique values.
 * @param[in] spec The detailed matrix specification.
 * @param[in] rows The range determining the contained number of rows.
 */
int hmatrix_split_ridx(const unsigned int N, const hmatrixspec_t *spec, const range_t *rows)
{
	assert(spec != NULL);

	unsigned int n = N;
	unsigned long width = (spec->b_left + spec->a + spec->b_right);

	if (n <= 0) {
		return rows->i;

	} else if (n <= spec->n_top) {
		return rows->i +rint(((double) n) /width);

	} else if ((n -= spec->n_top) <= spec->n_mid) {
		const long p = 1 + 2l*width;
		const long q = 2*n;

		const double y = sqrt(pow(p, 2) /4.0 - q);
		//const double x1 = p/2.0 +y;
		const double x2 = p/2.0 -y;
		return rows->i +spec->b_top +rint(x2);

	} else if ((n -= spec->n_mid) <= spec->n_bottom) {
		return rows->i +spec->b_top + spec->a +rint(((double) n) /width);

	}
	return rows->i +spec->b_top + spec->a + spec->b_bottom;
}
#endif

void hmatrix_split_ex(hmatrix_t *m, const int blocks, const int index)
{
    const int width = RANGE_LENGTH(m->x);
    const int height = RANGE_LENGTH(m->y);

    if (blocks <= 0 || blocks > height) {
        fatal("Invalid number of blocks (%d).", blocks);
        return;
    }

#ifdef USE_UNIFORM_SPLITTING
    hmatrixspec_t spec = {0};
    hmatrix_inferspec(m, &spec);

    const unsigned long blocksize = ceil(((double) spec.n) /blocks);

    if (blocksize < width) {
    	const unsigned int max = floor(((double) spec.n) /width);
        fatal("Block size too small. Choose %d blocks at a max for optimal performance.", max);
        return;
    }

    /* Update range */
    m->y.i = hmatrix_split_ridx(blocksize * index, &spec, &m->y);
    m->y.n = hmatrix_split_ridx(blocksize *(index +1), &spec, &m->y);

#else
    UNUSED(width);
    const int block_height = ceil(height / (float) blocks);

    if (block_height <= 0 || block_height > height) {
        fatal("Block height too small (%d).", block_height);
        return;
    }

    /* Update range */
    m->y.i = m->y.i + index * block_height;
    if (m->y.n > m->y.i + block_height)
        m->y.n = m->y.i + block_height;
#endif
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
    int xl, yl, k;

    /* Compute dimensions of matrix */
    xl = m->x.n - m->x.i;
    yl = m->y.n - m->y.i;

    if (m->x.n == m->y.n && m->x.i == m->y.i) {
        /* Symmetric matrix -> allocate triangle */
        m->triangular = TRUE;
        m->size = xl * (xl - 1) / 2 + xl;
    } else {
        /* Patrial matrix -> allocate rectangle */
        m->triangular = FALSE;
        m->size = xl * yl;
    }

    /* Allocate memory */
    m->values = calloc(sizeof(float), m->size);
    if (!m->values) {
        error("Could not allocate matrix for similarity values");
        return NULL;
    }

    hmatrixspec_t spec;
    hmatrix_inferspec(m, &spec);
    m->calcs = spec.n;

    /* Initialize to NaN values and count calculations */
    for (k = 0; k < m->size; k++) m->values[k] = NAN;

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
        if (x - m->x.i > y - m->y.i) {
            i = y - m->y.i, j = x - m->x.i;
        } else {
            i = x - m->x.i, j = y - m->y.i;
        }
        idx = ((j - i) + i * (m->x.n - m->x.i) - i * (i - 1) / 2);
    } else {
        idx = (x - m->x.i) + (y - m->y.i) * (m->x.n - m->x.i);
    }

    assert(idx < m->size);
    m->values[idx] = f;

    /* Set symmetric value on squared matrix */
    if (!m->triangular &&
        y >= m->x.i && y < m->x.n && x >= m->y.i && x < m->y.n) {
        idx = (y - m->x.i) + (x - m->y.i) * (m->x.n - m->x.i);

        assert(idx < m->size);
        m->values[idx] = f;
    }
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
        if (x - m->x.i > y - m->y.i) {
            i = y - m->y.i, j = x - m->x.i;
        } else {
            i = x - m->x.i, j = y - m->y.i;
        }
        idx = ((j - i) + i * (m->x.n - m->y.i) - i * (i - 1) / 2);
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
    assert(m);

    int j = 0, n = (m->x.n - m->x.i) * (m->y.n - m->y.i);
    double ts, ts1 = time_stamp(), ts2 = ts1;
    float f;

#pragma omp parallel for private(ts)
    for (int k = 0; k < n; k++) {
        int xi = k / (m->y.n - m->y.i) + m->x.i;
        int yi = k % (m->y.n - m->y.i) + m->y.i;

        /* Skip values that have been computed earlier */
        f = hmatrix_get(m, xi, yi);
        if (!isnan(f))
            continue;

        /* Set value in matrix */
        f = measure(s[xi], s[yi]);
        hmatrix_set(m, xi, yi, f);

        if (verbose || log_line)
#pragma omp critical
        {
            ts = time_stamp();

            /*
             * Update internal counter. Note that we have slightly more
             * calculations as expected, since we don't lock the matrix and
             * two threads might compute the same value in parallel.  As
             * long as writing to the matrix is atomic this should not be a
             * problem.
             */
            if (j < m->calcs)
                j++;

            /* Update progress bar every 100ms */
            if (verbose && ts - ts1 > 0.1) {
                prog_bar(0, m->calcs, j);
                ts1 = ts;
            }

            /* Print log line every minute if enabled */
            if (log_line && ts - ts2 > 60) {
                log_print(0, m->calcs, j);
                ts2 = ts;
            }
        }
    }

    if (verbose) {
        prog_bar(0, m->calcs, m->calcs);
    }

    if (log_line) {
        log_print(0, m->calcs, m->calcs);
    }
}


/**
 * Benchmark computation of similarity measure
 * @param m Matrix object
 * @param s Array of string objects
 * @param measure Similarity measure
 * @param t Time to run benchmark in seconds
 * @return Number of computations
 */
float hmatrix_benchmark(hmatrix_t *m, hstring_t *s,
                        double (*measure) (hstring_t x, hstring_t y),
                        double t)
{
    assert(m);
    uint64_t j = 0;
    int mt = omp_get_max_threads();
    double ts = time_stamp();

    /*
     * Naive implementation of a while loop. The loop terminates
     * after roughly t seconds by setting k to the maximum value.
     */
#pragma omp parallel for
    for (uint64_t k = 0; k < UINT64_MAX - mt; k++) {

        /* Select random pair of strings */
        int xi = lrand48() % (m->x.n - m->x.i) + m->x.i;
        int yi = lrand48() % (m->y.n - m->y.i) + m->y.i;

        /* Calculate similarity value */
        measure(s[xi], s[yi]);

#pragma omp critical
        {
            j++;
            if (time_stamp() - ts > t)
                k = UINT64_MAX - mt;
        }
    }

    return (float) j;
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
