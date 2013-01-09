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
#include "harry.h"
#include "util.h"

#include "kern_subsequence.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_subsequence</em>: Subsequence kernel
 * @{
 */

/* External variables */
extern config_t cfg;

/* Local variables */
int length = 3;         /**< Maximum length */
double lambda = 0.1;    /**< Weight for gaps */

/**
 * Initializes the similarity measure
 */
void kern_subsequence_config()
{
    config_lookup_int(&cfg, "measures.kern_subsequence.length", &length);
    config_lookup_float(&cfg, "measures.kern_subsequence.lambda", &lambda);
}

/**
 * Compute the subsequence kernel by Lodhi et al. (2002). The implementation
 * has been taken from the book by Cristianini & Shawe-Taylor.
 * @param x first string 
 * @param y second string
 * @return subsequence kernel
 */
float kern_subsequence_compare(str_t x, str_t y)
{
    float dps[x.len][y.len];
    float dp[x.len + 1][y.len + 1];
    float kern[length];
    int i, j, l;

    /* Initalize dps */
    for (i = 0; i < x.len; i++)
        for (j = 0; j < y.len; j++)
            if (!str_compare(x, i, y, j))
                dps[i][j] = lambda * lambda;
            else
                dps[i][j] = 0;

    /* Initialize dp */
    for (i = 0; i < x.len + 1; i++)
        dp[i][0] = 0;
    for (j = 0; j < y.len + 1; j++)
        dp[0][j] = 0;


    for (l = 1; l < length; l++) {
        kern[l] = 0;
        for (i = 0; i < x.len - 1; i++) {
            for (j = 0; j < y.len - 1; j++) {
                dp[i + 1][j + 1] = dps[i][j] + lambda * dp[i][j + 1] +
                    lambda * dp[i + 1][j] - lambda * lambda * dp[i][j];
                if (!str_compare(x, i, y, j)) {
                    dps[i][j] = lambda * lambda * dp[i][j];
                    kern[l] = kern[l] + dps[i][j];
                }
            }
        }
        return kern[length - 1];
    }

    return 0;
}

/** @} */
