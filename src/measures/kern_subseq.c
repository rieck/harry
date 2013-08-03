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
#include "strutil.h"
#include "util.h"

/**
 * @addtogroup strings
 * <hr>
 * <b>Module kern_subseq</b>: Subsequence kernel for strings 
 *
 * The module supports reading strings and computing the subsequence kernel
 * by Lohdi et al (JMLR 2002).  The strings need to be terminated by a
 * newline character and must not contain the NUL character.  Additionally,
 * strings must not start with 0xff in marker mode.  The kernel is computed
 * over the characters of the strings.
 *  
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

/* Global configuration */
double decay = 1.0;

/**
 * Initializes the module
 * @param c Configuration
 */
void kern_subseq_init(config_t *c)
{
    /* Decay factor */
    config_lookup_float(c, "strings.kern_subseq.decay", &decay);
}

/**
 * Computes the subsequence kernel of two strings. Implementation adapted
 * from the book of Shawe-Taylor and Cristanini (Cambridge 2004).
 * @param x first string 
 * @param y second string
 * @return subsequence kernel
 */
double kern_subseq_cmp(void *x, void *y)
{
    assert(x && y);
    return 0;
}

/**
 * Reads a string from a file stream. Reading stops when a newline character
 * is found, at end-of-file or error.  The function allocates memory that
 * needs to be freed later using levensthein_free.
 * @param f file stream
 * @return string
 */
void *kern_subseq_read(FILE *f)
{
    return str_read(f);
}

/**
 * Frees the memory of a string. 
 * @param x string
 */
void kern_subseq_free(void *x)
{
    str_free(x);
}

/** @} */
