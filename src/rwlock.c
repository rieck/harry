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
#include "rwlock.h"
#include "util.h"

/**
 * @defgroup rwlock Read-write lock for OpenMP. 
 * Code adapted from http://www.linux.org.ru/forum/development/4510260
 * This implementation does not provide fair scheduling. The writer thread
 * starves, if too many readers repeatingly acquire the lock.  Technically,
 * this can be fixed with a non-binary semaphore or a condition variable;
 * which, however, are both not available in OpenMP.  I am unsure how this
 * can be fixed.
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

/**
 * Init a read-write lock.
 * @param rw pointer to lock structure
 */
void rwlock_init(rwlock_t *rw)
{
    omp_init_lock(&rw->read);
    omp_init_lock(&rw->write);
    rw->readers = 0;
}

/**
 * Destroy a read-write lock.
 * @param rw pointer to lock structure
 */
void rwlock_destroy(rwlock_t *rw)
{
    omp_destroy_lock(&rw->read);
    omp_destroy_lock(&rw->write);
}

/**
 * Set lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_set_rlock(rwlock_t *rw)
{
    omp_set_lock(&rw->read);

    if (rw->readers == 0)
        omp_set_lock(&rw->write);
    rw->readers++;

    omp_unset_lock(&rw->read);
}

/**
 * Unset lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_unset_rlock(rwlock_t *rw)
{
    omp_set_lock(&rw->read);

    assert(rw->readers > 0);
    rw->readers--;
    if (rw->readers == 0)
        omp_unset_lock(&rw->write);

    omp_unset_lock(&rw->read);
}

/**
 * Set lock for writing. 
 * @param rw pointer to lock structure
 */
void rwlock_set_wlock(rwlock_t *rw)
{
    omp_set_lock(&rw->write);
    assert(rw->readers == 0);
}

/**
 * Unset lock for writing.
 * @param rw pointer to lock structure
 */
void rwlock_unset_wlock(rwlock_t *rw)
{
    assert(rw->readers == 0);
    omp_unset_lock(&rw->write);
}

/** @} */
