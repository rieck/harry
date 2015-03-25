/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
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
#include "uthash.h"

#include "sim_coefficient.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>sim_simpson</em>: Simpson coefficient <br/>
 * <em>sim_jaccard</em>: Jaccard coefficient <br/> 
 * <em>sim_braun</em>: Braun-Blanquet coefficient <br/> 
 * <em>sim_dice</em>: Dice coefficient (Czekanowsi, Soerensen-Dice) <br/>
 * <em>sim_sokal</em>: Sokal-Sneath coefficient (Anderberg) <br/> 
 * <em>sim_kulczynski</em>: second Kulczynski coefficient <br/> 
 * <em>sim_otsuka</em>: Otsuka coefficient (Ochiai) <br/>
 * @{
 */

typedef struct
{
    sym_t sym;          /**< Symbol or character */
    float cnt;          /**< Count of symbol */
    UT_hash_handle hh;  /**< uthash handle */
} bag_t;

/* Local variables */
static int binary = FALSE;

/* External variables */
extern config_t cfg;

void sim_coefficient_config()
{
    const char *str;

    /* Matching */
    config_lookup_string(&cfg, "measures.sim_coefficient.matching", &str);

    if (!strcasecmp(str, "cnt")) {
        binary = FALSE;
    } else if (!strcasecmp(str, "bin")) {
        binary = TRUE;
    } else {
        warning("Unknown matching '%s'. Using 'cnt' instead.", str);
        binary = FALSE;
    }
}

/**
 * Computes a histogram of symbols or characters
 * @param x string
 * @return histogram
 */
static bag_t *bag_create(hstring_t x)
{
    bag_t *xh = NULL, *bag = NULL;

    for (int i = 0; i < x.len; i++) {
        sym_t s = hstring_get(x, i);
        HASH_FIND(hh, xh, &s, sizeof(sym_t), bag);

        if (!bag) {
            bag = malloc(sizeof(bag_t));
            bag->sym = s;
            bag->cnt = 0;
            HASH_ADD(hh, xh, sym, sizeof(sym_t), bag);
        }

        bag->cnt++;
    }

    return xh;
}

/** 
 * Free the memory of histogram
 * @param xh Histogram
 */
static void bag_destroy(bag_t * xh)
{
    /* Clear hash table */
    while (xh) {
        bag_t *bag = xh;
        HASH_DEL(xh, bag);
        free(bag);
    }
}

/**
 * Computes the matches and mismatches
 * @param x first string 
 * @param y second string
 * @return matches
 */
static match_t match(hstring_t x, hstring_t y)
{
    bag_t *xh, *yh, *xb, *yb;
    match_t m;
    int missing;

    m.a = 0;
    m.b = 0;
    m.c = 0;

    xh = bag_create(x);
    yh = bag_create(y);

    if (!binary) {
        /* Count matching */
        missing = y.len;
        for (xb = xh; xb != NULL; xb = xb->hh.next) {
            HASH_FIND(hh, yh, &(xb->sym), sizeof(sym_t), yb);
            if (!yb) {
                m.b += xb->cnt;
            } else {
                m.a += fmin(xb->cnt, yb->cnt);
                missing -= fmin(xb->cnt, yb->cnt);
                if (yb->cnt < xb->cnt)
                    m.b += xb->cnt - yb->cnt;
            }
        }
        m.c += missing;
    } else {
        /* Binary matching */
        missing = HASH_COUNT(yh);
        for (xb = xh; xb != NULL; xb = xb->hh.next) {
            HASH_FIND(hh, yh, &(xb->sym), sizeof(sym_t), yb);
            if (!yb) {
                m.b += 1;
            } else {
                m.a += 1;
                missing -= 1;
            }
        }
        m.c += missing;
    }

    bag_destroy(xh);
    bag_destroy(yh);
    return m;
}

/**
 * Computes the Jaccard coefficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_jaccard_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return m.a / (m.a + m.b + m.c);
}

/**
 * Computes the Simpson coefficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_simpson_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return m.a / fmin(m.a + m.b, m.a + m.c);
}

/**
 * Computes the Braun-Blanquet coefficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_braun_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return m.a / fmax(m.a + m.b, m.a + m.c);
}

/**
 * Computes the Dice efficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_dice_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return 2 * m.a / (2 * m.a + m.b + m.c);
}

/**
 * Computes the Sokal-Sneath efficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_sokal_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return m.a / (m.a + 2 * (m.b + m.c));
}

/**
 * Computes the Kulczynski (2nd) efficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_kulczynski_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return 0.5 * (m.a / (m.a + m.b) + m.a / (m.a + m.c));
}

/**
 * Computes the Otsuka efficient 
 * @param x String x
 * @param y String y
 * @return coefficient
 */
float sim_otsuka_compare(hstring_t x, hstring_t y)
{
    match_t m = match(x, y);
    if (m.b == 0 && m.c == 0)
        return 1;

    return m.a / sqrt((m.a + m.b) * (m.a + m.c));
}


/** @} */
