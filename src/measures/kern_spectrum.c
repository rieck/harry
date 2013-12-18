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
#include "harry.h"
#include "util.h"
#include "vcache.h"
#include "norm.h"
#include "kern_spectrum.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_spectrum</em>: Spectrum kernel
 *
 * The runtime complexity of the kernel is linear in the length of the 
 * strings. However, the implementation is not very efficient, as the 
 * k-mers are repeatedly extracted from the strings. A caching mechanism
 * might provide a considerable acceleration here.
 *
 * C. Leslie, E. Eskin, and W. Noble. The spectrum kernel: a string kernel
 * for SVM protein classifica- tion.  In Proc. of Pacific Symposium on
 * Biocomputing (PSB), pages 564-575, 2002.
 * @{
 */

/* External variables */
extern config_t cfg;

/* Normalizations */
static knorm_t n = KN_NONE;

/* Local variables */
static int len = 3;         /**< Length of k-mers */

/**
 * Initializes the similarity measure
 */
void kern_spectrum_config()
{
    const char *str;

    /* Length parameter */
    config_lookup_int(&cfg, "measures.kern_spectrum.length", &len);

    /* Normalization */
    config_lookup_string(&cfg, "measures.kern_spectrum.norm", &str);
    n = knorm_get(str);
}


/**
 * Compares two unsigned 64 bit integers
 * @param x integer X
 * @param y integer Y
 * @return result as a signed integer
 */
static int cmp_uint64(const void *x, const void *y)
{
    if (*((uint64_t *) x) > *((uint64_t *) y))
        return +1;
    if (*((uint64_t *) x) < *((uint64_t *) y))
        return -1;
    return 0;
}

/**
 * Extract and sort k-mers in a string and return their hashes.
 * @param x string 
 * @return array of sorted k-mer hashes
 */
static uint64_t *extract_kmers(hstring_t x)
{
    assert(x.len - len + 1 >= 0);

    int i;

    uint64_t *xh = malloc(x.len * sizeof(uint64_t));
    if (!xh) {
        error("Could not allocate memory for spectrum kernel");
        return NULL;
    }
    
    for (i = 0; i < x.len - len + 1; i++) 
        xh[i] = hstring_hash_sub(x, i, len);    
    
    qsort(xh, x.len - len + 1, sizeof(uint64_t), cmp_uint64);
    return xh;
}

/**
 * Internal computation of spectrum kernel
 * @param x first string
 * @param y second string
 * @return spectrum kernel
 */
static float kernel(hstring_t x, hstring_t y)
{
    float k = 0;
    int i = 0, j = 0;
    
    /* Check for small strings */
    if (x.len < len || y.len < len)
        return 0;
    
    /* Extract k-mers */
    uint64_t *xh = extract_kmers(x);
    uint64_t *yh = extract_kmers(y);
    
    while(i < x.len - len + 1 && j < y.len - len + 1) {
        if (xh[i] < yh[j]) {
            i++;
        } else if (xh[i] > yh[j]) {
            j++;
        } else {
            float xn = 0;
            while(xh[i] == yh[j]) {
                i++;
                xn++;
            }

            float yn = 0;
            while(xh[i - 1] == yh[j]) {
                j++;
                yn++;
            }

            k += xn * yn;
        }
    } 
    
    /* Free space */
    free(xh);
    free(yh);
    
    return k;
}

/**
 * Compute the spectrum kernel by Leslie et al. (2002). 
 * @param x first string 
 * @param y second string
 * @return spectrum kernel
 */
float kern_spectrum_compare(hstring_t x, hstring_t y)
{
    float k = kernel(x, y);
    return knorm(n, k, x, y, kernel);
}

/** @} */
