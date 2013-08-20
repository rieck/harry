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

struct str_test tests[] = {
    /* comparison using characters */
    {"", "", "", 0},
    {"a", "", "", 1.0},
    {"", "a", "", 1.0},
    {"MARTHA", "MARHTA", "", 0.961},
    {"DWAYNE", "DUANE", "", 0.84},
    {"DIXON", "DICKSONX", "", 0.813},
    {NULL}
};

int test_compare()
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
        
        if (diff > 1e-3) {
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

    measure_config("dist_jarowinkler");

    err |= test_compare();

    config_destroy(&cfg);
    return err;
}
