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

/** Static variable */
static gzFile in;
static char *old_line = NULL;

/** External variables */
extern config_t cfg;

/**
 * Opens a file for reading text fasta. 
 * @param name File name
 * @return number of fasta or -1 on error
 */
int input_fasta_open(char *name)
{
    assert(name);
    size_t read, size;
    char *line = NULL;

    in = gzopen(name, "r");
    if (!in) {
        error("Could not open '%s' for reading", name);
        return -1;
    }

    int num = 0, cont = FALSE;
    while (!gzeof(in)) {
        line = NULL;
        read = gzgetline(&line, &size, in);
        if (read > 0)
            strtrim(line);
        if (read > 1 && !cont && (line[0] == '>' || line[0] == ';')) {
            num++;
            cont = TRUE;
        } else {
            cont = FALSE;
        }
        free(line);
    }

    /* Prepare reading */
    gzrewind(in);
    return num;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for data
 * @param len Length of block
 * @return number of read files
 */
int input_fasta_read(string_t *strs, int len)
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
            strs[i].str = seq;
            strs[i].len = alloc - 1;
            strs[i].idx = i;
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
    gzclose(in);
}

/** @} */
