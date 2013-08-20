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

#include "kern_wdegree.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_wdegree</em>: Weighted-degree kernel
 * @{
 */

/* External variables */
extern config_t cfg;

/* Local variables */
int degree = 3;		/**< Degree of kernel */
int shift = 0;		/**< Shift of kernel */

/**
 * Initializes the similarity measure
 */
void kern_wdegree_config()
{
    config_lookup_int(&cfg, "measures.kern_wdegree.degree", &degree);
    config_lookup_int(&cfg, "measures.kern_wdegree.shift", &shift);
}

static float kern_wdegree(sym_t *x, sym_t *y, int len)
{
    int i, start;
    float n = degree * (degree + 1.0);    
    float k = 0;

    for (i = 0, start = -1; i < len ; i++) {
        /* Identify matching region */
        if (x[i] == y[i]) {
            if (start == -1) 
                start = i;
            continue;
        }

        /* No match found continue */
        if (start == -1)
            continue;
        
        /* FIXME! There is a closed form by Sonnenburg. Too lazy now */
        len = i - start;    
        for (int j = degree; j > 0; j--) { 
            if (j > len)
                continue;
            k += 2 * (len - j + 1) * (degree - j + 1.0) / n;
        }
        start = -1;
    }
    
    if (start != -1) {
        len = i - start;           
        for (int j = degree; j > 0; j--) { 
            if (j > len)
                continue;
            k += 2 * (len - j + 1) * (degree - j + 1.0) / n;
        }       
    }
    
    return k;
}

/**
 * Compute the weighted degree kernel for strings. If the strings 
 * have unequal size, the remaining symbols of the longer string are
 * ignored (in accordance with the kernel definition)
 * @param x first string 
 * @param y second string
 * @return weighted degree kernel
 */
float kern_wdegree_compare(str_t x, str_t y)
{
    float k = 0;
    int s, len;

    /* Loop over strings */
    for (s = -shift; s <= shift; s++) {
        if (s <= 0) {
            len = fmax(fmin(x.len, y.len + s), 0);
            k += kern_wdegree(x.str.s, y.str.s - s, len);
        } else {
            len = fmax(fmin(x.len - s, y.len), 0);        
            k += kern_wdegree(x.str.s + s, y.str.s, len);
        }
    }

    return k;
}

/** @} */
