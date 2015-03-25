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
    m->col.start = 0;
    m->col.end = n;
    m->row.start = 0;
    m->row.end = n;
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
 * @param s Range string, e.g. 3:14 or 2:-1 or :
 * @param n Maximum size
 * @return Range object
 */
static range_t parse_range(range_t r, char *s, int n)
{
    char *ptr, *end = NULL;
    long l;

    /* Empty string */
    if (strlen(s) == 0)
        return r;

    /*
     * Since "1:1", "1:", ":1"  and ":" are all valid indices, sscanf
     * won't do it and we have to stick to manual parsing :(
     */
    ptr = strchr(s, ':');
    if (!ptr) {
        error("Invalid range string '%s'.", s);
        return r;
    } else {
        /* Create split */
        *ptr = '\0';
    }

    /* Start parsing */
    l = strtol(s, &end, 10);
    if (strlen(s) == 0)
        r.start = 0;
    else if (*end == '\0')
        r.start = (int) l;
    else
        error("Could not parse range '%s:...'.", s);

    l = strtol(ptr + 1, &end, 10);
    if (strlen(ptr + 1) == 0)
        r.end = n;
    else if (*end == '\0')
        r.end = (int) l;
    else
        error("Could not parse range '...:%s'.", ptr + 1);

    /* Support negative start and end indices */
    if (r.start < 0)
        r.start = n + r.start;
    if (r.end < 0)
        r.end = n + r.end;

    /* Sanity checks */
    if (r.end < 0 || r.start < 0 || r.end > n || r.start > n - 1
        || r.start >= r.end) {
        error("Invalid range '%s:%s'. Using default '0:%d'.", s, ptr + 1, n);
        r.start = 0;
        r.end = n;
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
void hmatrix_inferspec(const hmatrix_t *m, hmatrixspec_t * spec)
{
    assert(m != NULL && spec != NULL);

    const int width = RANGE_LENGTH(m->col);
    const int height = RANGE_LENGTH(m->row);

    const range_t c = m->col;
    const range_t r = m->row;

    // determine the overall location of the sub-matrix
    const int is_fully_underneath = (r.start >= c.end && c.end <= r.start);
    const int is_fully_above = (r.end <= c.start && c.start >= r.end);

    // calculate the exact positions
    if (is_fully_above) {
        spec->b_top = height;
        spec->b_bottom = 0;
        spec->b_left = 0;
        spec->b_right = width;
        spec->a = 0;

    } else if (is_fully_underneath) {
        spec->b_top = 0;
        spec->b_bottom = height;
        spec->b_left = width;
        spec->b_right = 0;
        spec->a = 0;

    } else {
        spec->b_top = MAX(c.start - r.start, 0);
        spec->b_bottom = MAX(r.end - c.end, 0);
        spec->b_left = MAX(r.start - c.start, 0);
        spec->b_right = MAX(c.end - r.end, 0);
        spec->a = height - spec->b_top - spec->b_bottom;
    }

    assert(spec->b_left + spec->a + spec->b_right == width);
    assert(spec->b_top + spec->a + spec->b_bottom == height);

    spec->n_top = width * spec->b_top;
    spec->n_mid = spec->a * spec->b_left +
        (pow(spec->a, 2) + spec->a) / 2 + spec->a * spec->b_right;
    spec->n_bottom = width * spec->b_bottom;
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
int hmatrix_split_ridx(const unsigned int N, const hmatrixspec_t * spec,
                       const range_t * rows)
{
    assert(spec != NULL);

    unsigned int n = N;
    unsigned long width = (spec->b_left + spec->a + spec->b_right);

    if (n <= 0) {
        return rows->i;

    } else if (n <= spec->n_top) {
        return rows->i + rint(((double) n) / width);

    } else if ((n -= spec->n_top) <= spec->n_mid) {
        const long p = 1 + 2l * width;
        const long q = 2 * n;

        const double y = sqrt(pow(p, 2) / 4.0 - q);
        //const double x1 = p/2.0 +y;
        const double x2 = p / 2.0 - y;
        return rows->i + spec->b_top + rint(x2);

    } else if ((n -= spec->n_mid) <= spec->n_bottom) {
        return rows->i + spec->b_top + spec->a + rint(((double) n) / width);

    }
    return rows->i + spec->b_top + spec->a + spec->b_bottom;
}
#endif

void hmatrix_split_ex(hmatrix_t *m, const int blocks, const int index)
{
    const int width = RANGE_LENGTH(m->col);
    const int height = RANGE_LENGTH(m->row);

    if (blocks <= 0 || blocks > height) {
        fatal("Invalid number of blocks (%d).", blocks);
        return;
    }
#ifdef USE_UNIFORM_SPLITTING
    hmatrixspec_t spec = { 0 };
    hmatrix_inferspec(m, &spec);

    const unsigned long blocksize = ceil(((double) spec.n) / blocks);

    if (blocksize < width) {
        const unsigned int max = floor(((double) spec.n) / width);
        fatal("Block size too small. The maximum block size is %d.", max);
        return;
    }

    /* Update range */
    m->row.start = hmatrix_split_ridx(blocksize * index, &spec, &m->row);
    m->row.end = hmatrix_split_ridx(blocksize * (index + 1), &spec, &m->row);

#else
    UNUSED(width);
    const int block_height = ceil(height / (float) blocks);

    if (block_height <= 0 || block_height > height) {
        fatal("Block height too small (%d).", block_height);
        return;
    }

    /* Update range */
    m->row.start = m->row.start + index * block_height;
    if (m->row.end > m->row.start + block_height)
        m->row.end = m->row.start + block_height;
#endif
}

/**
 * Set the column range for computation
 * @param m Matrix object
 * @param s String for column range
 */
void hmatrix_col_range(hmatrix_t *m, char *s)
{
    assert(m && s);
    m->col = parse_range(m->col, s, m->num);
}

/**
 * Set the row range for computation
 * @param m Matrix object
 * @param s String for row range
 */
void hmatrix_row_range(hmatrix_t *m, char *s)
{
    assert(m && s);
    m->row = parse_range(m->row, s, m->num);
}

/**
 * Allocate memory for matrix
 * @param m Matrix object
 * @return pointer to floats
 */
float *hmatrix_alloc(hmatrix_t *m)
{
    int cl, rl, k;

    /* Compute dimensions of matrix */
    cl = m->col.end - m->col.start;
    rl = m->row.end - m->row.start;

    if (m->col.end == m->row.end && m->col.start == m->row.start) {
        /* Symmetric matrix -> allocate triangle */
        m->triangular = TRUE;
        m->size = cl * (cl - 1) / 2 + cl;
    } else {
        /* Patrial matrix -> allocate rectangle */
        m->triangular = FALSE;
        m->size = cl * rl;
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
    for (k = 0; k < m->size; k++)
        m->values[k] = NAN;

    return m->values;
}

/**
 * Set a value in the matrix
 * @param m Matrix object
 * @param c Column index
 * @param r Row index
 * @param f Value
 */
void hmatrix_set(hmatrix_t *m, int c, int r, float f)
{
    int idx, i, j;

    if (m->triangular) {
        if (c - m->col.start > r - m->row.start) {
            i = r - m->row.start;
            j = c - m->col.start;
        } else {
            i = c - m->col.start;
            j = r - m->row.start;
        }
        idx = ((j - i) + i * (m->col.end - m->col.start) - i * (i - 1) / 2);
    } else {
        idx = (c - m->col.start) + (r - m->row.start)
            * (m->col.end - m->col.start);
    }

    assert(idx < m->size);
    m->values[idx] = f;

    /* Set symmetric value on squared matrix */
    if (!m->triangular &&
        r >= m->col.start && r < m->col.end && c >= m->row.start
        && c < m->row.end) {
        idx = (r - m->col.start) + (c - m->row.start)
            * (m->col.end - m->col.start);

        assert(idx < m->size);
        m->values[idx] = f;
    }
}


/**
 * Get a value from the matrix
 * @param m Matrix object
 * @param c Column coordinate
 * @param r Row coordinate
 * @return f Value
 */
float hmatrix_get(hmatrix_t *m, int c, int r)
{
    int idx, i, j;

    if (m->triangular) {
        if (c - m->col.start > r - m->row.start) {
            i = r - m->row.start;
            j = c - m->col.start;
        } else {
            i = c - m->col.start;
            j = r - m->row.start;
        }
        idx = ((j - i) + i * (m->col.end - m->row.start) - i * (i - 1) / 2);
    } else {
        idx = (c - m->col.start) + (r - m->row.start)
            * (m->col.end - m->col.start);
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
                     double (*measure) (hstring_t, hstring_t))
{
    assert(m);

    int cnt = 0, n;
    double ts, ts1 = time_stamp(), ts2 = ts1;
    float f;

    n = (m->col.end - m->col.start) * (m->row.end - m->row.start);

#ifdef HAVE_OPENMP
#pragma omp parallel for private(ts)
#endif
    for (int k = 0; k < n; k++) {
        int c = k / (m->row.end - m->row.start) + m->col.start;
        int r = k % (m->row.end - m->row.start) + m->row.start;

        /* Skip values that have been computed earlier */
        f = hmatrix_get(m, c, r);
        if (!isnan(f))
            continue;

        /* Set value in matrix */
        f = measure(s[c], s[r]);
        hmatrix_set(m, c, r, f);

        if (verbose || log_line) {
            /*
             * Update internal counter. Note that we have slightly more
             * calculations as expected, since we don't lock the matrix.
             * Moreover, this update is not thread-safe and the progress
             * bar might not be correct.
             */
            if (cnt < m->calcs)
                cnt++;

            /* Continue if less than 100ms have passed */
            ts = time_stamp();
            if (ts - ts1 < 0.1)
                continue;

            /* Lock only if something is displayed */
#ifdef HAVE_OPENMP
#pragma omp critical
#endif
            {
                /* Update progress bar every 100ms */
                if (verbose) {
                    prog_bar(0, m->calcs, cnt);
                    ts1 = ts;
                }

                /* Print log line every minute if enabled */
                if (log_line && ts - ts2 > 60) {
                    log_print(0, m->calcs, cnt);
                    ts2 = ts;
                }
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
                        double (*measure) (hstring_t, hstring_t), double t)
{
    assert(m);
    uint64_t l = 0;
    int mt = 0;
    double ts = time_stamp();

#ifdef HAVE_OPENMP
    mt = omp_get_max_threads();
#endif

    /*
     * Naive implementation of a while loop. The loop terminates
     * after roughly t seconds by setting k to the maximum value.
     */
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t k = 0; k < UINT64_MAX - mt; k++) {

        /* Select random pair of strings */
        int c = lrand48() % (m->col.end - m->col.start) + m->col.start;
        int r = lrand48() % (m->row.end - m->row.start) + m->row.start;

        /* Calculate similarity value */
        measure(s[c], s[r]);

#ifdef HAVE_OPENMP
#pragma omp critical
#endif
        {
            l++;
            if (time_stamp() - ts > t)
                k = UINT64_MAX - mt;
        }
    }

    return (float) l;
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
