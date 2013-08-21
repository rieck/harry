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

#ifndef VCACHE_H
#define VCACHE_H

typedef struct {
    uint64_t hash;	/**< Hash for sequences */
    float value;	/**< Cached similarity value */
    UT_hash_handle hh;  /**< uthash handle */
} entry_t;

#endif
