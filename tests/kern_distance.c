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
#include "vcache.h"
#include "tests.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/*
 * Structure for testing string kernels/distances
 */
struct hstring_test
{
    char *x;            /**< String x */
    char *y;            /**< String y */
    char *d;            /**< Distance */
    char *t;            /**< Type */
    float v;            /**< Expected output */
};

struct hstring_test tests[] = {
    /* Simple tests */
    {"a", "b", "levenshtein", "linear", 0.5},
    {"", "b", "levenshtein", "linear", 0.0},
    {"a", "", "levenshtein", "linear", 0.0},
    {"", "", "levenshtein", "linear", 0.0},
    {"a", "a", "levenshtein", "linear", 1.0},
    {"ab", "ab", "levenshtein", "linear", 4.0},
    {"ab", "ac", "levenshtein", "linear", 3.5},

    /* Polynomial substituion */
    {"", "", "levenshtein", "poly", 1.0},
    {"a", "a", "levenshtein", "poly", 2.0},
    {"ab", "ab", "levenshtein", "poly", 5.0},
    {"ab", "ac", "levenshtein", "poly", 4.5},

    /* RBF substituion */
    {"", "", "levenshtein", "rbf", 1},
    {"a", "a", "levenshtein", "rbf", 1},
    {"ab", "ab", "levenshtein", "rbf", 1},
    {"ab", "ac", "levenshtein", "rbf", 0.36787944117},

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

    printf("Testing distance substitution kernel ");
    for (i = 0; tests[i].x && !err; i++) {
        config_set_string(&cfg, "measures.kern_distance.dist", tests[i].d);
        config_set_string(&cfg, "measures.kern_distance.type", tests[i].t);
        measure_config("kern_distance");

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
