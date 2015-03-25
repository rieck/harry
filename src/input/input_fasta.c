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

/** 
 * @addtogroup input
 * <hr>
 * <em>fasta</em>: The strings are stored in FASTA format. A detailed 
 * description is available here http://en.wikipedia.org/wiki/FASTA_format. 
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"
#include "murmur.h"

/** Static variable */
static gzFile in;
static regex_t re;
static char *old_line = NULL;

/** External variables */
extern config_t cfg;

/** 
 * Converts a description to a label. The label is computed by matching 
 * a regular expression, either directly if the match is a number or 
 * indirectly by hashing.
 * @param desc Description 
 * @return label value.
 */
static float get_label(char *desc)
{
    char *endptr, *name = desc;
    regmatch_t pmatch[1];

    /* No match found */
    if (regexec(&re, desc, 1, pmatch, 0))
        return 0;

    name = desc + pmatch[0].rm_so;
    desc[pmatch[0].rm_eo] = 0;

    /* Test direct conversion */
    float f = strtof(name, &endptr);

    /* Compute hash value */
    if (!endptr || strlen(endptr) > 0)
        f = MurmurHash64B(name, strlen(name), 0xc0d3bab3) % 0xffff;

    return f;
}


/**
 * Opens a file for reading text fasta. 
 * @param name File name
 * @return 1 on success, 0 otherwise
 */
int input_fasta_open(char *name)
{
    assert(name);
    const char *pattern;

    /* Compile regular expression for label */
    config_lookup_string(&cfg, "input.fasta_regex", &pattern);
    if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
        error("Could not compile regex for label");
        return FALSE;
    }

    in = gzopen(name, "r");
    if (!in) {
        error("Could not open '%s' for reading", name);
        return FALSE;
    }

    return TRUE;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for data
 * @param len Length of block
 * @return number of read files
 */
int input_fasta_read(hstring_t *strs, int len)
{
    assert(strs && len > 0);
    int read, i = 0, alloc = -1;
    size_t size;
    char *line = NULL, *seq = NULL;

    while (i < len) {

        /* Read line */
        if (old_line) {
            line = old_line;
            read = strlen(line) + 1;
        } else {
            line = NULL;
            read = gzgetline(&line, &size, in);
        }
        old_line = NULL;

        /* Trim line */
        if (read >= 0)
            strtrim(line);

        /* End of sequence */
        if (alloc > 1 && (read == -1 || line[0] == ';' || line[0] == '>')) {
            strs[i].str.c = seq;
            strs[i].type = TYPE_BYTE;
            strs[i].len = alloc - 1;
            i++;
        }

        /* Stop on read error */
        if (read == -1) {
            free(line);
            break;
        }

        /* Reset pointer for next chunk */
        if (i == len) {
            /* Save old line */
            old_line = line;
#if 0
            /* Alternative code with slow gzseek */
            gzseek(in, -read, SEEK_CUR);
            free(line);
#endif
            break;
        }

        /* Check for comment char */
        if (line[0] == ';' || line[0] == '>') {
            /* Start of sequence */
            if (alloc == -1 || alloc > 1) {
                strs[i].src = strdup(line);
                strs[i].label = get_label(line);
                seq = calloc(sizeof(char), 1);
                alloc = 1;
            }
            goto skip;
        }

        /* Skip text before first comment */
        if (alloc == -1)
            goto skip;

        /* Append line to sequence */
        alloc += strlen(line);
        seq = realloc(seq, alloc * sizeof(char));
        strncat(seq, line, strlen(line));

      skip:
        free(line);
    }

    return i;
}

/**
 * Closes an open directory.
 */
void input_fasta_close()
{
    regfree(&re);
    gzclose(in);
}

/** @} */
