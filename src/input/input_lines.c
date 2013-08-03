/*
 * Harry - Similarity Measures for Strings
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org);
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
#include "util.h"
#include "input.h"

/** Static variable */
static gzFile in;
static int line_num = 0;

/** External variables */
extern config_t cfg;

/**
 * Opens a file for reading text lines. 
 * @param name File name
 * @return number of lines or -1 on error
 */
int input_lines_open(char *name)
{
    assert(name);

    in = gzopen(name, "r");
    if (!in) {
        error("Could not open '%s' for reading", name);
        return -1;
    }

    /* Count lines in file (I hope this is buffered)*/
    int c = -1, prev, num_lines = 0;
    do {
        prev = c;
        c = gzgetc(in);
        if (c == '\n')
            num_lines++;
    } while(c != -1);

    if (prev >= 0 && prev != '\n') num_lines++;

    /* Prepare reading */
    gzrewind(in);
    line_num = 0;

    return num_lines;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for data
 * @param len Length of block
 * @return number of lines read into memory
 */
int input_lines_read(string_t *strs, int len)
{
    assert(strs && len > 0);
    int read, i = 0, j = 0;
    size_t size;
    char buf[32], *line = NULL;

    for (i = 0; i < len; i++) {
#ifdef ENABLE_EVALTIME 
        double t1 = time_stamp();   
#endif    
    
        line = NULL;
        read = gzgetline(&line, &size, in);
        if (read == -1) {
            free(line);
            break;
        }
        
        /* Strip newline characters */
        strip_newline(line, read);

        strs[j].str = line;
        strs[j].len = strlen(line);

        snprintf(buf, 32, "line%d", line_num++);
        strs[j].src = strdup(buf);
        strs[j].idx = j;
        j++;

#ifdef ENABLE_EVALTIME 
        printf("strlen %d read %f\n", strs[j-1].len, time_stamp() - t1);
#endif    
    }

    return j;
}

/**
 * Closes an open directory.
 */
void input_lines_close()
{
    gzclose(in);
}

/** @} */

