/*
 * SIMONE - Similarity Measures for Structured Data
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
#include "tests.h"

/* Global variables */
int verbose = 0;
config_t cfg;

/* Set of test strings */
static char *test_str[] = {
    "a test", "äöüÄÖÜß€", "", " ", "nocrlf", NULL
};

/*
 * Test read function for edit distance
 */
int test_read()
{
    char *x = NULL, *n = NULL;
    FILE *f;
    int i = -1;

    printf("testing reading... ");
    n = getenv("TEST_FILE1");
    f = fopen(n, "r");

    /* Read test strings */
    for (i = 0; test_str[i]; i++) {
        x = dist_edit_read(f);
        if (strcmp(x, test_str[i])) {
            printf("error (%d)\n", i);
            return TRUE;
        }
    }

    printf("ok\n");
    return FALSE;
}

/* Test strings */
struct str_cmp test_dist[] = {
    {"abc", "abc", 0},
    {"tier", "tor", 2},
    {"sommer", "sonne", 3},
    {"regen", "riegel", 2},
    {"abc", "abcxyz", 3},
    {"xyz", "abcxyz", 3},
    {"", "", 0},
    {"", "abc", 3},
    {"abc", "", 3},
    {NULL, NULL, 0}
};

/* 
 * Test comparison function of edit distance 
 */
int test_cmp()
{
    int i = -1;
    double d;

    printf("testing comparison... ");
    for (i = 0; test_dist[i].x; i++) {
        d = dist_edit_cmp(test_dist[i].x, test_dist[i].y);
        if (fabs(d - test_dist[i].v) > 1e-9) {
            printf("error (%d)\n", i);
            return TRUE;
        }
    }

    printf("ok\n");
    return FALSE;
}

/* 
 * Test runtime performance
 */
int test_runtime()
{
    FILE *f;
    char *x[10], *n;
    int i = 0, j = 0, error = FALSE;
    double d[10][10], t;

    n = getenv("TEST_FILE2");
    f = fopen(n, "r");

    /* Read test strings */
    for (i = 0; i < 10; i++) {
        x[i] = dist_edit_read(f);
    }

    printf("testing runtime... "); 
    t = timestamp();
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            d[i][j] = dist_edit_cmp(x[i], x[j]);
        }
    }
    printf("ok (%.2fs)\n", timestamp() - t);
    
    printf("testing runtime (openmp)... ");
    t = timestamp();
#pragma omp parallel private(i)
    for (i = 0; i < 10; i++) {
#pragma omp for
        for (j = 0; j < 10; j++) {
            double s = dist_edit_cmp(x[i], x[j]);
            if (fabs(s - d[i][j]) > 1e-9) {
                error = TRUE;
            }
        }
    }

    if (error)
        printf("error\n");
    else
        printf("ok (%.2fs)\n", timestamp() - t);
    return error;
}

/**
 * Main test function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    config_init(&cfg);
    config_check(&cfg);

    err |= test_read();
    err |= test_cmp();
    err |= test_runtime();

    config_destroy(&cfg);

    return err;
}
