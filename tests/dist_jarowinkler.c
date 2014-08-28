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
    float v;            /**< Expected output */
};


struct hstring_test tests[] = {
    /* Comparison using characters */
    {"", "", 0},
    {"a", "", 1.0},
    {"", "a", 1.0},
    {"MARTHA", "MARHTA", 1 - 0.961},
    {"DWAYNE", "DUANE", 1 - 0.84},
    {"DIXON", "DICKSONX", 1 - 0.813},
    /* New examples */
    {"b", "b", 0},
    {"b", "bac", 1 - 0.8},
    {"b", "baba", 1 - 0.775},
    {"bac", "baba", 1 - 0.777778},
    {"baba", "baba", 1 - 1},
    {"john", "baba", 1 - 0},
    {NULL}
};

/**
 * Test runs
 */
int test_compare()
{
    int i, err = FALSE;
    hstring_t x, y;

    printf("Testing Jaro-Winkler distance ");
    for (i = 0; tests[i].x && !err; i++) {
        measure_config("dist_jarowinkler");

        x = hstring_init(x, tests[i].x);
        y = hstring_init(y, tests[i].y);

        x = hstring_preproc(x);
        y = hstring_preproc(y);

        float d = measure_compare(x, y);
        double diff = fabs(tests[i].v - d);

        printf(".");
        if (diff > 1e-3) {
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

    err |= test_compare();

    config_destroy(&cfg);
    return err;
}
