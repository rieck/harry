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

/**
 * @defgroup string String functions
 * The module contains string functions.
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "str.h"

/* External variable */
extern int verbose;

/* Global delimiter table */
char delim[256] = { DELIM_NOT_INIT };

/**
 * Free memory of the string structure
 * @param x string structure
 */
void str_free(str_t x)
{
    if (x.str)
        free(x.str);
    if (x.sym)
        free(x.sym);
    if (x.src)
        free(x.src);
}

/** 
 * Print string structure
 * @param x string structure
 */
void str_print(str_t x)
{
    int i;

    printf("str_t\nlen:%d; idx:%ld; src:%s\n", x.len, x.idx, x.src);
    if (x.str) {
        printf("str:");
        for (i = 0; i < x.len; i++)
            if (isprint(x.str[i]))
                printf("%c", x.str[i]);
            else
                printf("%%%.2x", x.str[i]);
        printf("\n");
    }

    if (x.sym) {
        printf("sym:");
        for (i = 0; i < x.len; i++)
            printf("%d ", x.sym[i]);
        printf("\n");
    }
}

/**
 * Decodes a string containing delimiters to a lookup table
 * @param s String containing delimiters
 */
void str_delim_set(const char *s)
{
    char buf[5] = "0x00";
    unsigned int i, j;

    if (strlen(s) == 0) {
        str_delim_reset();
        return;
    }

    memset(delim, 0, 256);
    for (i = 0; i < strlen(s); i++) {
        if (s[i] != '%') {
            delim[(unsigned int) s[i]] = 1;
            continue;
        }

        /* Skip truncated sequence */
        if (strlen(s) - i < 2)
            break;

        buf[2] = s[++i];
        buf[3] = s[++i];
        sscanf(buf, "%x", (unsigned int *) &j);
        delim[j] = 1;
    }
}

/**
 * Resets delimiters table. There is a global table of delimiter 
 * symbols which is only initialized once the first sequence is 
 * processed. This functions is used to trigger a re-initialization.
 */
void str_delim_reset()
{
    delim[0] = DELIM_NOT_INIT;
}

/**
 * Converts a string into a string of symbols. Depending on the configuration
 * either the words or the characters of the string are mapped to symbols.
 * The current version frees the original character string. This might be
 * changed later.
 * @param x strin
 * @return symbolized string
 */
str_t str_symbolize(str_t x)
{
    int i = 0, j = 0, k = 0, dlm = 0;
    int wstart = 0;

    x.sym = malloc(x.len * sizeof(sym_t));
    if (!x.sym) {
        error("Failed to allocate memory for symbols");
        return x;
    }

    if (delim[0] == DELIM_NOT_INIT) {
        for (i = 0; i < x.len; i++)
            x.sym[i] = (sym_t) x.str[i];
    } else {
        /* Find first delimiter symbol */
        for (dlm = 0; !delim[(unsigned char) dlm] && dlm < 256; dlm++);

        /* Remove redundant delimiters */
        for (i = 0, j = 0; i < x.len; i++) {
            if (delim[(unsigned char) x.str[i]]) {
                if (j == 0 || delim[(unsigned char) x.str[j - 1]])
                    continue;
                x.str[j++] = (char) dlm;
            } else {
                x.str[j++] = x.str[i];
            }
        }

        /* Extract words */
        for (wstart = i = 0; i < j + 1; i++) {
            /* Check for delimiters and remember start position */
            if ((i == j || x.str[i] == dlm) && i - wstart > 0) {
                uint64_t hash = hash_str(x.str + wstart, i - wstart);
                x.sym[k++] = (sym_t) hash;
                wstart = i + 1;
            }
        }
        x.len = k;
        
        /* Condense memory */
        sym_t *sym = malloc(x.len * sizeof(sym_t));
        memcpy(sym, x.sym, x.len * sizeof(sym_t));
        free(x.sym);
        x.sym = sym;
    }

    /* Free original string to save space. */
    free(x.str);
    x.str = NULL;
    
    return x;
}

/**
 * Convert a c-style string to a string structure. New memory is allocated
 * and the string is copied.
 * @param x string structure
 * @param s c-style string
 */
str_t str_convert(str_t x, char *s)
{
    x.str = strdup(s);
    x.len = strlen(s);
    x.sym = NULL;
    x.idx = 0;
    x.src = NULL;
    
    return x;
}

/** @} */
