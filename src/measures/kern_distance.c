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
#include "vcache.h"
#include "norm.h"
#include "measures.h"
#include "kern_distance.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>kern_distance</em>: Distance substitution kernel
 *
 * XXX
 * @{
 */

/* External variables */
extern config_t cfg;
extern func_t func[];

/* Normalizations */
static knorm_t norm = KN_NONE;
static subst_t subst = DS_LINEAR;
static int dist = 0;
static double fgamma = 1.0;
static double degree = 1.0;

/**
 * Initializes the similarity measure
 */
void kern_distance_config()
{
    const char *str;

    /* Distance measure */
    config_lookup_string(&cfg, "measures.kern_distance.distance", &str);
    dist = measure_match(str);
    func[dist].measure_config();
    
    /* Substitution type */
    config_lookup_string(&cfg, "measures.kern_distance.distance", &str);
    if (!strcasecmp(str, "linear")) {
        subst = DS_LINEAR;
    } else if (!strcasecmp(str, "poly")) {
        subst = DS_POLY;
    } else if (!strcasecmp(str, "neg")) {
        subst = DS_NEG;
    } else if (!strcasecmp(str, "rbf")) {
        subst = DS_RBF;
    } else {
        warning("Unknown substitution type '%s'. Using 'linear'.", str);
        subst = DS_LINEAR;
    }
    
    /* Parameters */
    config_lookup_float(&cfg, "measures.kern_distance.fgamma", &fgamma);
    config_lookup_float(&cfg, "measures.kern_distance.degree", &degree);    

    /* Normalization */    
    config_lookup_string(&cfg, "measures.kern_distance.norm", &str);
    norm = knorm_get(str);
}

static float dot(hstring_t x, hstring_t y)
{
    /* TODO */    
}

/**
 * Internal computation of distance-degree kernel 
 * @param x first string
 * @param y second string
 * @return distance substitution kernel
 */
static float kernel(hstring_t x, hstring_t y)
{
    float k = 0;
    float d = func[dist].measure_compare(x, y);

    switch (subst) {
    default:
    case DS_LINEAR:
        /* TODO */
        break;
    case DS_POLY:
        /* TODO */
        break;
    case DS_NEG:
        k = -pow(d, degree); 
        break;
    case DS_RBF:
        k = exp(-fgamma * d * d);
        break;
    }

    return k;
}

/**
 * Compute a distance substitution kernel
 * @param x first string 
 * @param y second string
 * @return distance substitution kernel
 */
float kern_distance_compare(hstring_t x, hstring_t y)
{
    float k = kernel(x, y);	
    return knorm(norm, k, x, y, kernel);
}

/** @} */
