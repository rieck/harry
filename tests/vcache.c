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
#include "vcache.h"
#include "tests.h"

/* Global variables */
int verbose = 1;
config_t cfg;

/** 
 * Test storage
 * @return error flag
 */
int test_storage()
{
    int i, err = FALSE;
    uint64_t key;
    float v1, v2;

    vcache_init();

    for (i = 0; i < 50000 && !err; i++) {
        key = lrand48();
        v1 = drand48();

        vcache_store(key, v1, ID_COMPARE);
        vcache_load(key, &v2, ID_COMPARE);

        if (v1 != v2) {
            printf("Error: %f != %f\n", v1, v2);
            err = TRUE;
        }

        if (i % 10000 == 0)
            vcache_info();
    }

    vcache_destroy();

    return err;
}

/** 
 * Stress test
 * @return error flag
 */
int test_stress()
{
    size_t i, err = FALSE;
    uint64_t key;
    float v1, v2;

    vcache_init();

    for (i = 0; i < 2000000 && !err; i++) {
        key = lrand48();
        v1 = drand48();
        vcache_store(key, v1, ID_COMPARE);
    }

    for (i = 0; i < 50000 && !err; i++) {
        key = lrand48();
        v1 = drand48();

        vcache_store(key, v1, ID_COMPARE);
        vcache_load(key, &v2, ID_COMPARE);

        if (v1 != v2) {
            printf("Error: %f != %f\n", v1, v2);
            err = TRUE;
        }

        if (i % 10000 == 0)
            vcache_info();
    }

    vcache_destroy();
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

    err |= test_storage();
    err |= test_stress();

    config_destroy(&cfg);
    return err;
}
