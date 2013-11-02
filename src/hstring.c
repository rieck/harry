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
 * Functions for processing strings and sequences
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "hstring.h"
#include "murmur.h"

/* External variable */
extern int verbose;
extern config_t cfg;

/* Global delimiter table */
char delim[256] = { DELIM_NOT_INIT };

/**
 * Structure for stop words
 */
typedef struct
{
    sym_t sym;                  /* Hash of stop word */
    UT_hash_handle hh;          /* uthash handle */
} stopword_t;
static stopword_t *stopwords = NULL;

/**
 * Free memory of the string structure
 * @param x string structure
 */
void hstring_destroy(hstring_t x)
{
    if (x.type == TYPE_CHAR && x.str.c)
        free(x.str.c);
    if (x.type == TYPE_SYM && x.str.s)
        free(x.str.s);
    if (x.src)
        free(x.src);
}

/** 
 * Check whether delimiters have been set
 * @return true if delimiters have been set
 */
int hstring_has_delim()
{
    return (delim[0] != DELIM_NOT_INIT);
}

/** 
 * Compare two symbols/characters
 * @param x string x
 * @param i position in string x
 * @param y string y
 * @param j position in string y
 * @return 0 if equal, < 0 if x smaller, > 0 if y smaller
 */
int hstring_compare(hstring_t x, int i, hstring_t y, int j)
{
    assert(x.type == y.type);
    assert(i < x.len && j < y.len);

    if (x.type == TYPE_SYM)
        return (x.str.s[i] - y.str.s[j]);
    else if (x.type == TYPE_CHAR)
        return (x.str.c[i] - y.str.c[j]);
    else
        error("Unknown string type");
    return 0;
}


/** 
 * Return symbol/character at given positions
 * @param x string x
 * @param i position in string x
 * @return character/symbol
 */
sym_t hstring_get(hstring_t x, int i)
{
    assert(i < x.len);

    if (x.type == TYPE_SYM)
        return x.str.s[i];
    else if (x.type == TYPE_CHAR)
        return x.str.c[i];
    else
        error("Unknown string type");
    return 0;
}


/** 
 * Print string structure
 * @param x string structure
 */
void hstring_print(hstring_t x)
{
    int i;

    if (x.type == TYPE_CHAR && x.str.c) {
        for (i = 0; i < x.len; i++)
            if (isprint(x.str.c[i]))
                printf("%c", x.str.c[i]);
            else
                printf("%%%.2x", x.str.c[i]);
        printf(" (char)\n");
    }

    if (x.type == TYPE_SYM && x.str.s) {
        for (i = 0; i < x.len; i++)
            printf("%d ", x.str.s[i]);
        printf(" (sym)\n");
    }

    printf("  [type: %d, len: %d; idx:% ld; src: %s, label: %f]\n",
           x.type, x.len, x.idx, x.src, x.label);
}

/**
 * Decodes a string containing delimiters to a lookup table
 * @param s String containing delimiters
 */
void hstring_delim_set(const char *s)
{
    char buf[5] = "0x00";
    unsigned int i, j;

    if (strlen(s) == 0) {
        hstring_delim_reset();
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
void hstring_delim_reset()
{
    delim[0] = DELIM_NOT_INIT;
}



/**
 * Converts a string into a sequence of symbols  (words) using delimiter
 * characters.  The original character string is lost.
 * @param x character string
 * @return symbolized string
 */
hstring_t hstring_symbolize(hstring_t x)
{
    int i = 0, j = 0, k = 0, dlm = 0;
    int wstart = 0;


    /* A string of n chars can have at most n/2 + 1 words */
    sym_t *sym = malloc((x.len / 2 + 1) * sizeof(sym_t));
    if (!sym) {
        error("Failed to allocate memory for symbols");
        return x;
    }

    /* Find first delimiter symbol */
    for (dlm = 0; !delim[(unsigned char) dlm] && dlm < 256; dlm++);

    /* Remove redundant delimiters */
    for (i = 0, j = 0; i < x.len; i++) {
        if (delim[(unsigned char) x.str.c[i]]) {
            if (j == 0 || delim[(unsigned char) x.str.c[j - 1]])
                continue;
            x.str.c[j++] = (char) dlm;
        } else {
            x.str.c[j++] = x.str.c[i];
        }
    }

    /* Extract words */
    for (wstart = i = 0; i < j + 1; i++) {
        /* Check for delimiters and remember start position */
        if ((i == j || x.str.c[i] == dlm) && i - wstart > 0) {
            /* Hash word */
            uint64_t hash = hash_str(x.str.c + wstart, i - wstart);
            sym[k++] = (sym_t) hash;
            wstart = i + 1;
        }
    }
    x.len = k;
    sym = realloc(sym, x.len * sizeof(sym_t));

    /* Change representation */
    free(x.str.c);
    x.str.s = sym;
    x.type = TYPE_SYM;

    return x;
}

/**
 * Convert a c-style string to a string structure. New memory is allocated
 * and the string is copied.
 * @param x string structure
 * @param s c-style string
 */
hstring_t hstring_init(hstring_t x, char *s)
{
    x.str.c = strdup(s);
    x.type = TYPE_CHAR;
    x.len = strlen(s);
    x.idx = 0;
    x.src = NULL;

    return x;
}

/**
 * Create an empty string
 * @param x string structure
 * @param t type of string
 */
hstring_t hstring_empty(hstring_t x, int t)
{
    x.str.c = NULL;
    x.type = t;
    x.len = 0;
    x.idx = 0;
    x.src = NULL;

    return x;
}

/**
 * Compute a 64-bit hash for a string. The hash is used at different locations.
 * Collisions are possible but not very likely (hopefully)  
 * @param x String to hash
 * @return hash value
 */
uint64_t hstring_hash1(hstring_t x)
{
    if (x.type == TYPE_CHAR && x.str.c)
        return MurmurHash64B(x.str.c, sizeof(char) * x.len, 0xc0ffee);
    if (x.type == TYPE_SYM && x.str.s)
        return MurmurHash64B(x.str.s, sizeof(sym_t) * x.len, 0xc0ffee);

    warning("Nothing to hash. String is missing");
    return 0;
}

static uint64_t swap(uint64_t x)
{
    uint64_t r = 0;
    r |= x << 32;
    r |= x >> 32;
    return r;
}

/**
 * Compute a 64-bit hash for two strings. The computation is symmetric, that is,
 * the same strings retrieve the same hash independent of their order. 
 * Collisions are possible but not very likely (hopefully)  
 * @param x String to hash
 * @param y String to hash 
 * @return hash value
 */
uint64_t hstring_hash2(hstring_t x, hstring_t y)
{
    uint64_t a, b;

    if (x.type == TYPE_CHAR && y.type == TYPE_CHAR && x.str.c && y.str.c) {
        a = MurmurHash64B(x.str.c, sizeof(char) * x.len, 0xc0ffee);
        b = MurmurHash64B(y.str.c, sizeof(char) * y.len, 0xc0ffee);
        return swap(a) ^ b;
    }
    if (x.type == TYPE_SYM && y.type == TYPE_SYM && x.str.s && y.str.s) {
        a = MurmurHash64B(x.str.s, sizeof(sym_t) * x.len, 0xc0ffee);
        b = MurmurHash64B(y.str.s, sizeof(sym_t) * y.len, 0xc0ffee);
        return swap(a) ^ b;
    }

    warning("Nothing to hash. Strings are missing or incompatible.");
    return 0;
}


/**
 * Read in and hash stop words 
 * @param file stop word file
 */
void stopwords_load(const char *file)
{
    char buf[1024];
    FILE *f;

    info_msg(1, "Loading stop words from '%s'.", file);
    if (!(f = fopen(file, "r")))
        fatal("Could not read stop word file %s", file);

    /* Read stop words */
    while (fgets(buf, 1024, f)) {
        int len = strip_newline(buf, strlen(buf));
        if (len <= 0)
            continue;

        /* Decode URI-encoding */
        decode_str(buf);

        /* Add stop word to hash table */
        stopword_t *word = malloc(sizeof(stopword_t));
        word->sym = (sym_t) hash_str(buf, len);
        HASH_ADD(hh, stopwords, sym, sizeof(sym_t), word);
    }
    fclose(f);
}

/** 
 * Filter stop words from symbols
 * @param x Symbolized string
 */
hstring_t stopwords_filter(hstring_t x)
{
    assert(x.type = TYPE_SYM);
    stopword_t *stopword;
    int i, j;

    for (i = j = 0; i < x.len; i++) {
        /* Check for stop word */
        sym_t sym = x.str.s[i];
        HASH_FIND(hh, stopwords, &sym, sizeof(sym_t), stopword);

        /* Remove stopword */
        if (stopword)
            continue;

        if (i != j)
            x.str.s[j] = x.str.s[i];
        j++;
    }
    x.len = j;
    return x;
}

/**
 * Preprocess a given string
 * @param x character string
 * @return preprocessed string
 */
hstring_t hstring_preproc(hstring_t x)
{
    assert(x.type == TYPE_CHAR);
    int decode, reverse, c, i, k;

    config_lookup_int(&cfg, "input.decode_str", &decode);
    config_lookup_int(&cfg, "input.reverse_str", &reverse);

    if (decode) {
        x.len = decode_str(x.str.c);
        x.str.c = (char *) realloc(x.str.c, x.len);
    }

    if (reverse) {
        for (i = 0, k = x.len - 1; i < k; i++, k--) {
            c = x.str.c[i];
            x.str.c[i] = x.str.c[k];
            x.str.c[k] = c;
        }
    }

    if (hstring_has_delim())
        x = hstring_symbolize(x);

    if (stopwords)
        x = stopwords_filter(x);

    return x;
}

/**
 * Destroy stop words table
 */
void stopwords_destroy()
{
    stopword_t *s;

    while (stopwords) {
        s = stopwords;
        HASH_DEL(stopwords, s);
        free(s);
    }
}

/** @} */
