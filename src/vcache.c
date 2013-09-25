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

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"
#include "rwlock.h"
#include "vcache.h"

/* External variables */
extern config_t cfg;

/* Cache structure */
static entry_t *hash = NULL;
static list_t *head = NULL;
static list_t *tail = NULL;
static long space = 0;
static long size = 0;

/* Cache statistics */
static double hits = 0;
static double misses = 0;

/* Read-write cache for lock */
static rwlock_t rwlock;

/* 
 * Since we are using a hash table, the consumed memory is higher than the
 * stored data.  For simplicity and in accordance with some simple
 * benchmarks, we use a factor of 2 to model the extra space needed by the
 * hash table.  TODO: Tune uthash.h to decrease this factor.
 */
#define entry_size (2 * (sizeof(list_t) + sizeof(entry_t)))

/**
 * @defgroup vcache Value cache 
 * Cache for similarity values based on uthash. 
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

/**
 * Init value cache
 */
void vcache_init()
{
    int csize;
    config_lookup_int(&cfg, "measures.cache_size", &csize);

    /* Initialize cache stats */
    space = floor((csize * 1024 * 1024) / entry_size);
    size = 0;
    misses = 0;
    hits = 0;

    info_msg(1, "Initializing cache with %dMb (%d entries)", csize, space);

    /* Initialize data structures */
    hash = NULL;
    head = NULL;
    tail = NULL;
    
    /* Initialize lock */
    rwlock_init(&rwlock);
}

/**
 * Trim cache if necessary
 */
void vcache_alloc(entry_t ** entry, list_t ** elem)
{
    if (space > 0) {
        *entry = calloc(1, sizeof(entry_t));
        *elem = calloc(1, sizeof(list_t));

        if (!*elem || !*entry)
            error("Could not allocate memory for cache entry");

        space--;
        size++;
        return;
    }

    assert(head);

    /* Remove first element from hash */
    HASH_FIND(hh, hash, &(head->key), sizeof(uint64_t), (*entry));
    HASH_DEL(hash, *entry);

    /* Remove element from fifo list */
    *elem = head;
    head = head->next;
}

/**
 * Store a similarity value. The value is associated with 64 bit key that
 * can be computed from a string, a sequence of symbols or even a pair
 * of strings. Collisions may occur, but are not likely. 
 * @param key Key for similarity value
 * @param value Value to store
 * @return true on success, false otherwise
 */
int vcache_store(uint64_t key, float value)
{
    entry_t *entry = NULL;
    list_t *elem = NULL;
    int ret;
    
    rwlock_set_wlock(&rwlock);

    /* Check for presence of key */
    HASH_FIND(hh, hash, &key, sizeof(uint64_t), entry);
    if (entry) {
        /* Update value */
        entry->value = value;
        ret = FALSE;
    } else {
        /* Allocate or re-use memory */
        vcache_alloc(&entry, &elem);

        /* Update hash table */
        entry->key = key;
        entry->value = value;
        HASH_ADD(hh, hash, key, sizeof(uint64_t), entry);

        /* Update fifo of keys */
        elem->key = key;
        elem->next = NULL;

        if (!head || !tail) {
            head = elem;
            tail = elem;
        } else {
            tail->next = elem;
            tail = elem;
        }
        ret = TRUE;
    }
    
    rwlock_unset_wlock(&rwlock);
    return ret;
}

/**
 * Load a similarity value. The value is associated with 64 bit key.
 * @param key Key for similarity value
 * @param value Pointer to space for value
 * @return true on success, false otherwise 
 */
int vcache_load(uint64_t key, float *value)
{
    entry_t *entry;
    int ret;

    rwlock_set_rlock(&rwlock);
    
    /* Check for presence of key */
    HASH_FIND(hh, hash, &key, sizeof(uint64_t), entry);
    if (!entry) {
        misses++;
        ret = FALSE;
    } else {
        hits++;
        *value = entry->value;
        ret = TRUE;
    }
    
    rwlock_unset_rlock(&rwlock);
    return ret;
}

/**
 * Display some information about cache usage 
 */
void vcache_info()
{
    float used = (size * entry_size) / (1024.0 * 1024.0);
    float free = (space * entry_size) / (1024.0 * 1024.0);

    info_msg(1,
             "Cache stats: %.1fMb used by %d entries, hits %3.0f%%, %.1fMb free.",
             used, size, 100 * hits / (hits + misses), free);
}

/**
 * Get used memory in megabytes
 * @return used memory
 */
float vcache_get_used()
{
    return (size * entry_size) / (1024.0 * 1024.0);
}

/**
 * Get hit rate
 * @return hit rate
 */
float vcache_get_hitrate()
{
    return 100 * hits / (hits + misses);
}

/**
 * Destroy the value cache 
 */
void vcache_destroy()
{
    info_msg(1, "Clearing cache and freeing memory");

    /* Destroy lock */
    rwlock_destroy(&rwlock);
    
    /* Clear hash table */
    while (hash) {
        entry_t *entry = hash;
        HASH_DEL(hash, entry);
        free(entry);
    }

    /* Clear fifo list */
    while (head) {
        list_t *elem = head;
        head = head->next;
        free(elem);
    }
}

/** @} */
