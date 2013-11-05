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

#ifndef HSTRING_H
#define HSTRING_H

/** Placeholder for non-initialized delimiters */
#define DELIM_NOT_INIT  42

/* 
 * Symbols for words. We keep this signed.
 */
typedef int16_t sym_t;

/* Support types for strings. See union str in struct */
#define TYPE_CHAR		0x00
#define TYPE_SYM		0x01

/**
 * Structure for a string
 */
typedef struct
{
    union
    {
        char *c;              /**< String data (not necessary c-style) */
        sym_t *s;             /**< Symbol representation */
    } str;

    int len;                  /**< Length of string */
    unsigned int type:1;      /**< Type of string */

    char *src;                /**< Optional source of string */
    size_t idx;               /**< Optional index of string */
    float label;              /**< Optional label of string */
} hstring_t;

void hstring_print(hstring_t);
void hstring_delim_set(const char *);
void hstring_delim_reset();
hstring_t hstring_symbolize(hstring_t);
hstring_t hstring_preproc(hstring_t);
hstring_t hstring_empty(hstring_t, int type);
hstring_t hstring_init(hstring_t, char *);
void hstring_destroy(hstring_t);
uint64_t hstring_hash_sub(hstring_t x, int i, int l);
uint64_t hstring_hash1(hstring_t);
uint64_t hstring_hash2(hstring_t, hstring_t);
int hstring_has_delim();
int hstring_compare(hstring_t x, int i, hstring_t y, int j);
sym_t hstring_get(hstring_t x, int i);

/* Additional functions */
void stopwords_load(const char *f);
void stopwords_destroy();

#endif /* HSTRING_H */
