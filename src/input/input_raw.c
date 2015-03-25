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
 * <em>raw</em>: The strings are provided on standard input in raw format.
 *
 * This input module is designed for efficiently interfacing with other
 * analysis environments.  The raw format of a string is [len][array], where
 * [len] is a 32-bit unsigned integer in host-byte order indicating the
 * length of the following byte [array] containing the string data.
 @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"
#include "murmur.h"

/** Static variable */
static int str_num = 0;

/** External variables */
extern config_t cfg;

/**
 * Opens stdin for reading raw strings
 * @param name File name (bogus)
 * @return 1 on success, 0 otherwise
 */
int input_raw_open(char *name)
{
    if (stdin == NULL) {
        error("Could not open <stdin> for reading");
        return FALSE;
    }

    str_num = 0;
    return TRUE;
}

/**
 * Reads a block of strings into memory.
 * @param strs Array for data
 * @param num Length of block
 * @return number of strings read into memory
 */
int input_raw_read(hstring_t *strs, int num)
{
    assert(strs && num > 0);
    uint32_t len, ret, i = 0, j = 0;
    char *str = NULL;

    for (i = 0; i < num; i++) {
        ret = fread(&len, sizeof(len), 1, stdin);
        if (ret != 1) {
            error("Incomplete length field in raw format");
            break;
        }

        str = malloc(len * sizeof(char));
        if (!str) {
            error("Failed to allocate memory for string");
            break;
        }

        ret = fread(str, sizeof(char), len, stdin);
        if (ret != len) {
            error("Incomplete byte array in raw format");
            break;
        }

        strs[j].str.c = str;
        strs[j].type = TYPE_BYTE;
        strs[j].len = len;
        strs[j].src = NULL;
        j++;
    }

    return j;
}

/**
 * Closes an open directory.
 */
void input_raw_close()
{
    /* Do nothing */
}

/** @} */
