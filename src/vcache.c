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
#include "vcache.h"

/* External variables */
extern config_t cfg;

/* Cache structure */
static entry_t *hash = NULL;
static list_t *head = NULL;
static list_t *tail = NULL;
static int space = 0;
static int size = 0;

/* Cache statistics */
static double hits = 0;
static double misses = 0;

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
    int size;
    config_lookup_int(&cfg, "measures.cache", &size);
    space = (size * 1024 * 1024) / (sizeof(entry_t) + sizeof(list_t));

    info_msg(1, "Initializing cache with %d megabytes (%d entries)", size,
             space);

    hash = NULL;
    head = NULL;
    tail = NULL;
}

/**
 * Trim cache if necessary
 */
void vcache_trim()
{
    entry_t *entry;
    list_t *elem;

    if (space > 0 || !head)
        return;

    /* Remove element from hash */
    HASH_FIND(hh, hash, &(head->key), sizeof(uint64_t), entry);
    HASH_DEL(hash, entry);
    free(entry);

    /* Remove element from fifo list */
    elem = head;
    head = head->next;
    free(elem);

    /* Update free space */
    space++;
    size--;
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
    entry_t *entry;
    list_t *elem;

    /* Check for presence of key */
    HASH_FIND(hh, hash, &key, sizeof(uint64_t), entry);
    if (entry) {
        /* Update value */
        entry->value = value;
        return TRUE;
    }

    /* Update hash table */
    entry = malloc(sizeof(entry_t));
    if (!entry) {
        error("Could not allocate memory for hash table");
        return FALSE;
    }
    entry->key = key;
    entry->value = value;
    HASH_ADD(hh, hash, key, sizeof(uint64_t), entry);

    /* Update fifo of keys */
    elem = malloc(sizeof(list_t));
    if (!elem) {
        error("Could not allocate memory for fifo list");
        return FALSE;
    }
    elem->key = key;

    if (!head || !tail) {
        head = elem;
        tail = elem;
    } else {
        tail->next = elem;
        tail = elem;
    }

    /* Update free space */
    space--;
    size++;
    vcache_trim();
    
    return TRUE;
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

    /* Check for presence of key */
    HASH_FIND(hh, hash, &key, sizeof(uint64_t), entry);
    if (!entry) {
        misses++;
        return FALSE;
    } else {
        hits++;
        *value = entry->value;
        return TRUE;
    }
}

/**
 * Display some information about cache usage 
 */
void vcache_info()
{
    info_msg(1, "Cache stats: %dMb used by %d entries, hit rate %f%%, %dMb free.",
             size * (sizeof(entry_t) + sizeof(list_t)), size, hits / (hits+misses),
             space * (sizeof(entry_t) + sizeof(list_t)));
}

/**
 * Destroy the value cache 
 */
void vcache_destroy()
{
    info_msg(1, "Clearing cache and freeing memory");

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
