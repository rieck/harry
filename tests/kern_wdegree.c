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
#include "default.h"
#include "util.h"
#include "measures.h"
#include "tests.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/*
 * Structure for testing string kernels/distances
 */
struct str_test
{
    char *x;		/**< String x */
    char *y;		/**< String y */
    int d;		/**< Degree of kernel */
    int s;		/**< Shift of kernel */
    float v;		/**< Expected output */
};


struct str_test tests[] = {
    /* No shift */
    {"", "", 3, 0, 0},
    {"a", "", 3, 0, 0},
    {"", "a", 3, 0, 0},
    {"a", "b", 3, 0, 0},
    {"a", "a", 3, 0, 1 / 2.0},
    {"aa", "ab", 3, 0, 1 / 2.0},
    {"ab", "ab", 3, 0, 2 / 2.0 + 1 / 3.0},
    {"abc", "abc", 3, 0, 3 / 2.0 + 2 / 3.0 + 1 / 6.0},
    {"abab", "abab", 3, 0, 4 / 2.0 + 3 / 3.0 + 2 / 6.0},
    /* Shift */
    {"", "", 3, 1, 0},
    {"a", "", 3, 1, 0},
    {"", "a", 3, 1, 0},
    {"a", "b", 3, 1, 0},
    {"a", "a", 3, 1, 1 / 2.0},
    {"aa", "a", 3, 1, 2 / 2.0},
    {"a", "aa", 3, 1, 2 / 2.0},
    {"aa", "aa", 3, 1, 2 / 2.0 + 2 / 2.0 + 1 / 3.0},
    {NULL}
};

/**
 * Test runs
 * @param error flag
 */
int test_compare()
{
    int i, err = FALSE;
    str_t x, y;

    for (i = 0; tests[i].x && !err; i++) {
        config_set_int(&cfg, "measures.kern_wdegree.shift", tests[i].s);
        config_set_int(&cfg, "measures.kern_wdegree.degree", tests[i].d);
        measure_config("kern_wdegree");
    
        x = str_convert(x, tests[i].x);
        y = str_convert(y, tests[i].y);

        x = str_symbolize(x);
        y = str_symbolize(y);

        float d = measure_compare(x, y);
        double diff = fabs(tests[i].v - d);

        if (diff > 1e-6) {
            printf("Error %f != %f\n", d, tests[i].v);
            str_print(x);
            str_print(y);
            err = TRUE;
        }

        str_free(x);
        str_free(y);
    }

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
