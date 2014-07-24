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
    char *d;            /**< Delimiters */
    int p;              /**< Length of subsequences */
    float v;            /**< Expected output */
};


struct hstring_test tests[] = {
    /* Character strings */
    {"", "", "", 2, 0},
    {"a", "a", "", 2, 0},
    {"ab", "ab", "", 2, 1},
    {"abcd", "", "", 2, 0},
    {"", "abcd", "", 2, 0},
    {"abcd", "abcd", "", 2, 3},
    {"aaaa", "aaaa", "", 2, 9},
    {"aaaabbb", "aaaa", "", 2, 9},
    {"aaaabbb", "aaaabb", "", 2, 9 + 1 + 2},

    /* Words */
    {"a b", "a b", " ", 1, 2},
    {"a b a b", "a b a b", " ", 1, 4 + 4},
    {"a b", "a b", " ", 2, 1},
    {"a b a b", "a b a b", " ", 2, 4 + 1},
    {"a b", "a b", " ", 3, 0},
    {"a b a b", "a b a b", " ", 3, 1 + 1},
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
        config_set_int(&cfg, "measures.kern_spectrum.length", tests[i].p);
        measure_config("kern_spectrum");

        x = hstring_init(x, tests[i].x);
        y = hstring_init(y, tests[i].y);

        hstring_delim_set(tests[i].d);
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
