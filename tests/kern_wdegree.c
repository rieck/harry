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


/* Test for degree = 3 and no shift */
struct str_test tests1[] = {
    {"", "", "", 0},
    {"a", "", "", 0},
    {"", "a", "", 0},
    {"a", "b", "", 0},
    {"a", "a", "", 1/2.0},
    {"aa", "ab", "", 1/2.0},
    {"ab", "ab", "", 2/2.0 + 1/3.0},
    {"abc", "abc", "", 3/2.0 + 2/3.0 + 1/6.0},
    {"abab", "abab", "", 4/2.0 + 3/3.0 + 2/6.0},    
    {NULL}
};

/* Test for degree = 3 and shift = 1 */
struct str_test tests2[] = {
    {"", "", "", 0},
    {"a", "", "", 0},
    {"", "a", "", 0},
    {"a", "b", "", 0},
    {"a", "a", "", 1/2.0},
    {"aa", "a", "", 2/2.0},
    {"a", "aa", "", 2/2.0},
    {"aa", "aa", "", 2/2.0 + 2/2.0 + 1/3.0},
    {NULL}
};


int test_compare(struct str_test tests[])
{
    int i, err = FALSE;
    str_t x, y;
    
    for (i = 0; tests[i].x ; i++) {
        x = str_convert(x, tests[i].x);
        y = str_convert(y, tests[i].y);

        str_delim_set(tests[i].delim);
        x = str_symbolize(x);
        y = str_symbolize(y);
        
        float d = measure_compare(x,y);
        double diff = fabs(tests[i].v - d);
        
        if (diff > 1e-6) {
            printf("Error %f != %f\n", d, tests[i].v);
            str_print(x, "x = ");
            str_print(y, "y = ");
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

    measure_config("kern_wdegree");
    err |= test_compare(tests1);

    config_set_int(&cfg, "measures.kern_wdegree.shift", 1);
    measure_config("kern_wdegree");    
    err |= test_compare(tests2);    

    config_destroy(&cfg);
    return err;
}
