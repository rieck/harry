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

#ifndef HSTRING_H
#define HSTRING_H

/** Placeholder for non-initialized delimiters */
#define DELIM_NOT_INIT  42

/* 
 * Symbols for words. Note: Some measures enumerate all possible symbols.
 * These need to be patched first to support larger symbol sizes.
 */
typedef uint64_t sym_t;

/* Support types for strings. See union str in struct */
#define TYPE_BYTE		0x00
#define TYPE_WORD		0x01
#define TYPE_BIT		0x02

/**
 * Structure for a string
 */
typedef struct
{
    union
    {
        char *c;              /**< Byte or bit representation */
        sym_t *s;             /**< Word representation */
    } str;

    int len;                  /**< Length of string */
    unsigned int type;        /**< Type of string */

    char *src;                /**< Optional source of string */
    float label;              /**< Optional label of string */
} hstring_t;

void hstring_print(hstring_t);
void hstring_delim_set(const char *);
void hstring_delim_reset();
hstring_t hstring_wordify(hstring_t);
hstring_t hstring_preproc(hstring_t);
hstring_t hstring_empty(hstring_t, int type);
hstring_t hstring_init(hstring_t, char *);
void hstring_destroy(hstring_t *);
uint64_t hstring_hash_sub(hstring_t x, int i, int l);
uint64_t hstring_hash1(hstring_t);
uint64_t hstring_hash2(hstring_t, hstring_t);
int hstring_has_delim();
sym_t hstring_get(hstring_t x, int i);
hstring_t hstring_soundex(hstring_t);

/* Additional functions */
void stopwords_load(const char *f);
void stopwords_destroy();

/* Inline functions */

/** 
 * Compare two symbols/characters
 * @param x string x
 * @param i position in string x
 * @param y string y
 * @param j position in string y
 * @return 0 if equal, < 0 if x smaller, > 0 if y smaller
 */
static inline int hstring_compare(hstring_t x, int i, hstring_t y, int j)
{
    assert(x.type == y.type);
    assert(i < x.len && j < y.len);

    if (x.type == TYPE_WORD)
        return (x.str.s[i] - y.str.s[j]);
    else if (x.type == TYPE_BYTE)
        return (x.str.c[i] - y.str.c[j]);
    else
        error("Unknown string type");
    return 0;
}




#endif /* HSTRING_H */
