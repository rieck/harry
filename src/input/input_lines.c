/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org);
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
 * <em>lines</em>: The strings are stored as text lines in a file. 
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
static int line_num = 0;

/** External variables */
extern config_t cfg;

/** 
 * Converts the beginning of a text line to a label. The label is computed
 * by matching a regular expression, either directly if the match is a
 * number or indirectly by hashing.
 * @param line Text line
 * @return label value.
 */
static float get_label(char *line)
{
    char *endptr, *name = line, old;
    regmatch_t pmatch[1];

    /* No match found */
    if (regexec(&re, line, 1, pmatch, 0))
        return 1.0;

    name = line + pmatch[0].rm_so;
    old = line[pmatch[0].rm_eo];
    line[pmatch[0].rm_eo] = 0;

    /* Test direct conversion */
    float f = strtof(name, &endptr);

    /* Compute hash value */
    if (!endptr || strlen(endptr) > 0)
        f = MurmurHash64B(name, strlen(name), 0xc0d3bab3) % 0xffff;

    line[pmatch[0].rm_eo] = old;

    /* Shift string. This is very inefficient. I know */
    memmove(line, line + pmatch[0].rm_eo, strlen(line) - pmatch[0].rm_eo + 1);
    return f;
}


/**
 * Opens a file for reading text lines. 
 * @param name File name
 * @return 1 on success, 0 otherwise
 */
int input_lines_open(char *name)
{
    assert(name);
    const char *pattern;

    in = gzopen(name, "r");
    if (!in) {
        error("Could not open '%s' for reading", name);
        return FALSE;
    }

    /* Compile regular expression for label */
    config_lookup_string(&cfg, "input.lines_regex", &pattern);
    if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
        error("Could not compile regex for label");
        return FALSE;
    }

    line_num = 0;
    return TRUE;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for data
 * @param len Length of block
 * @return number of lines read into memory
 */
int input_lines_read(hstring_t *strs, int len)
{
    assert(strs && len > 0);
    int read, i = 0, j = 0;
    size_t size;
    char buf[32], *line = NULL;

    for (i = 0; i < len; i++) {
        line = NULL;
        read = gzgetline(&line, &size, in);
        if (read == -1) {
            free(line);
            break;
        }

        /* Strip newline characters */
        strip_newline(line, read);

        /* Caution: May modify the line */
        strs[j].label = get_label(line);

        strs[j].str.c = line;
        strs[j].type = TYPE_BYTE;
        strs[j].len = strlen(line);
        snprintf(buf, 32, "line%d", line_num++);
        strs[j].src = strdup(buf);
        j++;
    }

    return j;
}

/**
 * Closes an open directory.
 */
void input_lines_close()
{
    regfree(&re);
    gzclose(in);
}

/** @} */
