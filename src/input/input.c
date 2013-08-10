/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org);
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

/** 
 * @defgroup input Input interface
 *
 * Interface and functions for reading strings.
 *
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"

/* Modules */
#include "input_arc.h"
#include "input_dir.h"
#include "input_lines.h"
#include "input_fasta.h"

/* Other stuff */
#include "uthash.h"

/**
 * Structure for input interface
 */
typedef struct
{
    int (*input_open) (char *);
    int (*input_read) (string_t *, int);
    void (*input_close) (void);
} func_t;
static func_t func;

/**
 * Structure for stop words
 */
typedef struct
{
    uint64_t hash;              /* Hash of stop word */
    UT_hash_handle hh;          /* uthash handle */
} stopword_t;
static stopword_t *stopwords = NULL;

/**< Delimiter table */
extern char delim[256];
/** External variables */
extern config_t cfg;

/** 
 * Configure the input of Harry
 * @param format Name of input format
 */
void input_config(const char *format)
{
    if (!strcasecmp(format, "dir")) {
        func.input_open = input_dir_open;
        func.input_read = input_dir_read;
        func.input_close = input_dir_close;
    } else if (!strcasecmp(format, "lines")) {
        func.input_open = input_lines_open;
        func.input_read = input_lines_read;
        func.input_close = input_lines_close;
    } else if (!strcasecmp(format, "fasta")) {
        func.input_open = input_fasta_open;
        func.input_read = input_fasta_read;
        func.input_close = input_lines_close;
#ifdef ENABLE_LIBARCHIVE
    } else if (!strcasecmp(format, "arc")) {
        func.input_open = input_arc_open;
        func.input_read = input_arc_read;
        func.input_close = input_arc_close;
#endif
    } else {
        error("Unknown input format '%s', using 'lines' instead.", format);
        input_config("lines");
    }
}

/**
 * Wrapper for opening the input source.
 * @param name Name of input source, e.g., directory or file name
 * @return Number of available entries or -1 on error
 */
int input_open(char *name)
{
    return func.input_open(name);
}

/**
 * Wrapper for reading a block from the input source.
 * @param strs Allocated array for string data
 * @param len Length of allocated arrays
 * @return Number of read strings
 */
int input_read(string_t *strs, int len)
{
    return func.input_read(strs, len);
}

/**
 * Wrapper for closing the input source. 
 */
void input_close(void)
{
    func.input_close();
}

/**
 * Free a chunk of input strings
 */
void input_free(string_t *strs, int len)
{
    assert(strs);

    int j;
    for (j = 0; j < len; j++) {
        if (strs[j].src)
            free(strs[j].src);
        if (strs[j].str)
            free(strs[j].str);
        if (strs[j].sym)
            free(strs[j].sym);
    }
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
        word->hash = hash_str(buf, len);
        HASH_ADD(hh, stopwords, hash, sizeof(uint64_t), word);
    }
    fclose(f);
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

/** 
 * Filter stopwords in place
 * @param str input string
 * @param len length of string
 * @return len of new string
 */
int stopwords_filter(char *str, int len)
{
    int i, k, start = -1;
    stopword_t *found;

    for (i = 0, k = 0; i < len; i++) {

        int dlm = delim[(int) str[i]];
        int end = (i == len - 1);

        /* Start of word */
        if (start == -1 && !dlm)
            start = i;

        /* End of word */
        if (start != -1 && (dlm || end)) {
            int len = (i - start) + (end ? 1 : 0);
            uint64_t hash = hash_str(str + start, len);

            /* Check for stop word and copy if not */
            HASH_FIND(hh, stopwords, &hash, sizeof(uint64_t), found);
            if (!found) {
                memcpy(str + k, str + start, len);
                k += len;
            }

            start = -1;
        }

        /* Always copy delimiter. Keep consecutive delimiters. */
        if (dlm)
            str[k++] = str[i];
    }

    return k;
}

/** 
 * In-place pre-processing of strings
 */
void input_preproc(string_t *strs, int len)
{
    assert(strs);
    int decode, reverse, c, i, j, k;

    config_lookup_int(&cfg, "input.decode_str", &decode);
    config_lookup_int(&cfg, "input.reverse_str", &reverse);

    for (j = 0; j < len; j++) {
        if (decode) {
            strs[j].len = decode_str(strs[j].str);
            strs[j].str = (char *) realloc(strs[j].str, strs[j].len);
        }

        if (reverse) {
            for (i = 0, k = strs[j].len - 1; i < k; i++, k--) {
                c = strs[j].str[i];
                strs[j].str[i] = strs[j].str[k];
                strs[j].str[k] = c;
            }
        }

        if (stopwords) {
            strs[j].len = stopwords_filter(strs[j].str, strs[j].len);
        }
    }
}

/** @} */
