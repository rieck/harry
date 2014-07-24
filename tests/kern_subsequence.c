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
#include "hconfig.h"
#include "util.h"
#include "measures.h"
#include "tests.h"
#include "vcache.h"

/* Global variables */
int verbose = 0;
config_t cfg;

#define LAM (0.5)
#define LAM2 (LAM * LAM)
#define LAM4 (LAM2 * LAM2)

/*
 * Structure for testing string kernels/distances
 */
struct hstring_test
{
    char *x;            /**< String x */
    char *y;            /**< String y */
    float l;            /**< Weight for gaps */
    int p;              /**< Length of subsequences */
    char *n;            /**< Normalization of kernel */
    float v;            /**< Expected output */
};


struct hstring_test tests[] = {
    /* Corner cases */
    {"", "", LAM, 2, "none", 1},
    {"lala", "", LAM, 2, "none", 0},
    {"", "lala", LAM, 2, "none", 0},

    /* Length 2 */
    {"ab", "ab", LAM, 2, "none", LAM4},
    {"ab", "axb", LAM, 2, "none", LAM4 * LAM},
    {"ab", "abb", LAM, 2, "none", LAM4 * LAM + LAM4},
    {"car", "cat", LAM, 2, "none", LAM4},
    {"axxb", "ayyb", LAM, 2, "none", LAM4 * LAM4},

    /* Length 1, 2, 3 */
    {"cata", "gatta", LAM, 1, "none", 6 * LAM2},
    {"cata", "gatta", LAM, 2, "none",
     LAM4 * LAM2 * LAM + 2 * LAM4 * LAM + 2 * LAM4},
    {"cata", "gatta", LAM, 3, "none", 2 * LAM4 * LAM2 * LAM},

    /* Normalization */
    {"ab", "xy", LAM, 2, "l2", 0},
    {"ab", "ab", LAM, 2, "l2", 1},
    {"abc", "abc", LAM, 2, "l2", 1},

    {NULL}
};

/**
 * Test runs
 * @param error flag
 */
int test_compare()
{
    int i, err = FALSE;
    hstring_t x, y;

    printf("Testing subsequence kernel ");
    for (i = 0; tests[i].x && !err; i++) {
        config_set_float(&cfg, "measures.kern_subsequence.lambda",
                         tests[i].l);
        config_set_int(&cfg, "measures.kern_subsequence.length", tests[i].p);
        config_set_string(&cfg, "measures.kern_subsequence.norm", tests[i].n);
        measure_config("kern_subsequence");

        x = hstring_init(x, tests[i].x);
        y = hstring_init(y, tests[i].y);

        x = hstring_preproc(x);
        y = hstring_preproc(y);

        float d = measure_compare(x, y);
        double diff = fabs(tests[i].v - d);

        printf(".");
        if (diff > 1e-6) {
            printf("Error %f != %f\n", d, tests[i].v);
            hstring_print(x);
            hstring_print(y);
            err = TRUE;
        }

        hstring_destroy(&x);
        hstring_destroy(&y);
    }
    printf(" done.\n");

    return err;
}

/**
 * Main test function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    config_init(&cfg);
    config_check(&cfg);

    vcache_init();

    err |= test_compare();

    vcache_destroy();

    config_destroy(&cfg);
    return err;
}
