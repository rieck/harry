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
 * <em>dir</em>: The strings are stored as files in a directory. The directory
 * is not processed recursively. 
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "input.h"

/* Local functions */
static char *load_file(char *path, char *name, int *size);
static void fix_dtype(char *path, struct dirent *dp);

/* Local variables */
static DIR *dir = NULL;
static char *path = NULL;

/**
 * Opens a directory for reading files. 
 * @param p Directory name
 * @return number of regular files or -1 on error
 */
int input_dir_open(char *p)
{
    assert(p);
    struct dirent *dp;
    path = p;

    /* Open directory */
    dir = opendir(path);
    if (!dir) {
        error("Could not open directory '%s'", path);
        return -1;
    }

    /* Count files */
    int num_files = 0;
    while (dir && (dp = readdir(dir)) != NULL) {
        fix_dtype(path, dp);
        if (dp->d_type == DT_REG || dp->d_type == DT_LNK)
            num_files++;
    }
    rewinddir(dir);
    return num_files;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for file data
 * @param len Length of block
 * @return number of read files 
 */
int input_dir_read(string_t *strs, int len)
{
    assert(strs && len > 0);
    int i, j = 0, l;

    /* Determine maximum path length and allocate buffer */
    int maxlen = fpathconf(dirfd(dir), _PC_NAME_MAX);

    /* Load block of files */
    for (i = 0; i < len; i++) {
        struct dirent *buf, *dp;
        buf = malloc(offsetof(struct dirent, d_name) + maxlen + 1);

        /* Read directory entry to local buffer */
        int r = readdir_r(dir, (struct dirent *) buf, &dp);
        if (r != 0 || !dp) {
            free(buf);
            return j;
        }

        /* Skip all entries except for regular files and symlinks */
        fix_dtype(path, dp);
        if (dp->d_type != DT_REG && dp->d_type != DT_LNK)
            goto skip;

        strs[j].str = load_file(path, dp->d_name, &l);
        strs[j].src = strdup(dp->d_name);
        strs[j].len = l;
        strs[j].idx = j;
        j++;
      skip:
        free(buf);
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

#ifdef ENABLE_OPENMP
#pragma omp critical (snprintf)
#endif
    {
        /* snprintf is not necessary thread-safe. good to know. */
        if (name)
            snprintf(file, 512, "%s/%s", path, name);
        else
            snprintf(file, 512, "%s", path);
    }

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

/** @} */
