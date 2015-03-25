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

#ifndef VCACHE_H
#define VCACHE_H

/** Task identifiers */
#define ID_COMPARE              1       /* Global comparison cache */
#define ID_DIST_COMPRESS	2       /* Compression distance */
#define ID_NORM                 3       /* Normalization */
#define ID_KERN_DISTANCE	4       /* Distance substitution kernel */
#define ID_DIST_KERNEL		5       /* Kernel-based distance */

typedef struct
{
    uint64_t key;       /**< Hash for sequences */
    int id;             /**< ID of task */
    float val;          /**< Cached similarity value */
} entry_t;

void vcache_init();
int vcache_load(uint64_t key, float *value, int);
int vcache_store(uint64_t key, float value, int);
void vcache_info();
void vcache_destroy();
float vcache_get_hitrate();
float vcache_get_used();

#endif
