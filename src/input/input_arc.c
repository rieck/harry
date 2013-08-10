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
 * <em>arc</em>: The strings are stored as files in an archive. The archive
 * is processed recursively and all files are processed by Harry. 
 * @{
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"

#ifdef ENABLE_LIBARCHIVE

#include <archive.h>
#include <archive_entry.h>
#include "input.h"

/* Local variables */
static struct archive *a = NULL;

/**
 * Opens an archive for reading files. 
 * @param name Archive name
 * @return number of regular files or -1 on error
 */
int input_arc_open(char *name)
{
    assert(name);
    struct archive_entry *entry;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    FILE *f = fopen(name, "r");
    if (f == NULL) {
        error("Failed to open '%s", name);
        return -1;
    }

    int r = archive_read_open_FILE(a, f);
    if (r != 0) {
        fclose(f);
        error("%s", archive_error_string(a));
        return -1;
    }

    /* Count regular files in archive */
    int num_files = 0;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        if (archive_entry_filetype(entry) == AE_IFREG) {
            num_files++;
        }
        archive_read_data_skip(a);
    }
    archive_read_close(a);

    /* Open file again */
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    fseek(f, 0, SEEK_SET);
    archive_read_open_FILE(a, f);
    return num_files;
}

/**
 * Reads a block of files into memory.
 * @param strs Array for data
 * @param len Length of block
 * @return number of read files
 */
int input_arc_read(str_t *strs, int len)
{
    assert(strs && len > 0);
    struct archive_entry *entry;
    int i, j = 0;

    /* Load block of files (no OpenMP here) */
    for (i = 0; i < len; i++) {
        /* Perform reading of archive */
        int r = archive_read_next_header(a, &entry);
        if (r != ARCHIVE_OK)
            break;

        if (archive_entry_filetype(entry) != AE_IFREG) {
            archive_read_data_skip(a);
        } else {
            if (!archive_entry_size_is_set(entry)) {
                warning("Archive entry has no size set.");
            }

            /* Add entry */
            strs[j].str.c = malloc(archive_entry_size(entry) * sizeof(char));
            archive_read_data(a, strs[j].str.c, archive_entry_size(entry));
            strs[j].src = strdup(archive_entry_pathname(entry));
            strs[j].len = archive_entry_size(entry);
	    strs[j].idx = j;
            j++;
        }
    }

    return j;
}

/**
 * Closes an open directory.
 */
void input_arc_close()
{
    archive_read_close(a);
}

#endif

/** @} */
