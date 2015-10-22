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
 * <em>dir</em>: The strings are stored as files in a directory. The
 * directory is not processed recursively.  The suffixes of the files are
 * used as la If the suffixes are numbers, they are directly intepreted as
 * labels, otherwise they are hashed.
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"
#include "murmur.h"

/* Local functions */
static char *load_file(char *path, char *name, int *size);
static float get_label(char *desc);
static void fix_dtype(char *path, struct dirent *dp);

/* Local variables */
static DIR *dir = NULL;
static char *path = NULL;

/**
 * Opens a directory for reading files. 
 * @param p Directory name
 * @return 1 on success, 0 otherwise
 */
int input_dir_open(char *p)
{
    assert(p);
    path = p;

    /* Open directory */
    dir = opendir(path);
    if (!dir) {
        error("Could not open directory '%s'", path);
        return FALSE;
    }

    return TRUE;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for file data
 * @param len Length of block
 * @return number of read files 
 */
int input_dir_read(hstring_t *strs, int len)
{
    assert(strs && len > 0);
    int j = 0, l = 0;
    struct dirent *dp;

    /* Load block of files */
    while (dir && j < len && (dp = readdir(dir)) != NULL) {
        /* Skip all entries except for regular files and symlinks */
        fix_dtype(path, dp);
        if (dp->d_type != DT_REG && dp->d_type != DT_LNK)
            continue;

        strs[j].str.c = load_file(path, dp->d_name, &l);
        strs[j].src = strdup(dp->d_name);
        strs[j].type = TYPE_BYTE;
        strs[j].len = l;
        strs[j].label = get_label(strs[j].src);
        j++;
    }

    return j;
}

/**
 * Closes an open directory.
 */
void input_dir_close()
{
    closedir(dir);
}

/**
 * Loads a file into a byte array. The array is allocated 
 * and need to be free'd later by the caller.
 * @param path Path to file
 * @param name File name or NULL
 * @param size Pointer to file size
 * @return file data
 */
static char *load_file(char *path, char *name, int *size)
{
    assert(path);
    long read;
    char *x = NULL, file[512];
    struct stat st;

    /* snprintf is not necessary thread-safe. good to know. */
    if (name)
        snprintf(file, 512, "%s/%s", path, name);
    else
        snprintf(file, 512, "%s", path);

    /* Open file */
    FILE *fptr = fopen(file, "r");
    if (!fptr) {
        warning("Could not open file '%s'", file);
        return NULL;
    }

    /* Allocate memory */
    stat(file, &st);
    *size = st.st_size;
    if (!(x = malloc((*size + 1) * sizeof(char)))) {
        warning("Could not allocate memory for file data");
        return NULL;
    }

    /* Read data */
    read = fread(x, sizeof(char), *size, fptr);
    fclose(fptr);
    if (*size != read)
        warning("Could not read all data from file '%s'", file);

    return x;
}

static void fix_dtype(char *path, struct dirent *dp)
{
    struct stat st;
    char buffer[512];

    if (dp->d_type == DT_UNKNOWN) {
        snprintf(buffer, 512, "%s/%s", path, dp->d_name);
        stat(buffer, &st);
        if (S_ISREG(st.st_mode))
            dp->d_type = DT_REG;
        if (S_ISLNK(st.st_mode))
            dp->d_type = DT_LNK;
    }
}

/** 
 * Converts a file name to a label. The label is computed from the 
 * file's suffix; either directly if the suffix is a number or 
 * indirectly by hashing.
 * @param desc Description (file name) 
 * @return label value.
 */
static float get_label(char *desc)
{
    char *endptr;
    char *name = desc + strlen(desc) - 1;

    /* Determine dot in file name */
    while (name != desc && *name != '.')
        name--;

    /* Place pointer before '.' */
    if (name != desc)
        name++;

    /* Test direct conversion */
    float f = strtof(name, &endptr);

    /* Compute hash value */
    if (!endptr || strlen(endptr) > 0)
        f = MurmurHash64B(name, strlen(name), 0xc0d3bab3) % 0xffff;

    return f;
}

/** @} */
