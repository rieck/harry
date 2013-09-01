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

#ifndef STR_H
#define STR_H

/** Placeholder for non-initialized delimiters */
#define DELIM_NOT_INIT  42

/* 
 * Basic symbol in a string 
 */
typedef uint16_t sym_t;

#define STR_CHR		0x00
#define STR_SYM		0x01

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
    char flags;               /**< Flags of structure */
    
    char *src;                /**< Optional source of string */
    size_t idx;               /**< Optional index of string */
    float label;              /**< Optional label of string */
} str_t;

void str_free(str_t);
void str_print(str_t, char *);
void str_delim_set(const char *);
void str_delim_reset();
str_t str_symbolize(str_t);
str_t str_convert(str_t, char *);
uint64_t str_hash1(str_t);
uint64_t str_hash2(str_t, str_t);

#endif /* STR_H */
