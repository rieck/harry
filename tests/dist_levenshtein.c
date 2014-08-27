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
    char *delim;        /**< Delimiter string */
    float v;            /**< Expected output */
};

struct hstring_test tests[] = {
    /* Comparison using characters */
    {"", "", "", 0},
    {"a", "", "", 1},
    {"", "a", "", 1},
    {"a", "a", "", 0},
    {"ab", "ba", "", 2},
    {"bab", "ba", "", 1},
    {"abba", "babb", "", 2},
    {"a.b", "a.c", "", 1},
    {".a.b.", "a..c.", "", 3},
    /* Comparison using words */
    {"", "", ".", 0},
    {"a", "", ".", 1},
    {"", "a", ".", 1},
    {"a", "a", ".", 0},
    {"ab", "ba", ".", 1},
    {"bab", "ba", ".", 1},
    {"abba", "babb", ".", 1},
    {"a.b", "a.c", ".", 1},
    {".a.b.", "a..c.", ".", 1},
    /* Further test cases */
    {"abcd", "axcy", "", 2},
    {"abc", "axcy", "", 2},
    {"abcd", "xcy", "", 3},
    {".x.y.", ".x.y.", ".", 0},
    {"x...y..", "...x..y", ".", 0},
    {".x.y", "x.y.", ".", 0},
    /* Tests for new implementation */
    {"a", "b", "", 1},
    {"aa", "aa", "", 0},
    {"ab", "aa", "", 1},
    {"aba", "aaa", "", 1},
    {"a", "bab", "", 2},
    {"bbb", "a", "", 3},
    {"yyybca", "yyycba", "", 2},
    {"bcaxxx", "cbaxxx", "", 2},
    {"yyybcaxxx", "yyycbaxxx", "", 2},
    {NULL}
};

/**
 * Test runs 
 * @return error flag
 */
int test_compare()
{
    int i, err = FALSE;
    hstring_t x, y;

    printf("Testing Levenshtein distance ");
    for (i = 0; tests[i].x && !err; i++) {
        measure_config("dist_levenshtein");

        x = hstring_init(x, tests[i].x);
        y = hstring_init(y, tests[i].y);

        hstring_delim_set(tests[i].delim);
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

    err |= test_compare();

    config_destroy(&cfg);
    return err;
}
