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
 * @defgroup rwlock Read-write lock
 *
 * Wrapper for read-write lock. If the POSIX thread library is available,
 * the corresponding lock is directly used.  Otherwise the implementation
 * resorts to an inefficient OpenMP mutex.  :(
 *
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

/**
 * Init a read-write lock.
 * @param rw pointer to lock structure
 */
void rwlock_init(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_init(&rw->lock, NULL);
#else
    omp_init_lock(&rw->lock);
#endif
}

/**
 * Destroy a read-write lock.
 * @param rw pointer to lock structure
 */
void rwlock_destroy(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_destroy(&rw->lock);
#else
    omp_destroy_lock(&rw->lock);
#endif
}

/**
 * Set lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_set_rlock(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_rdlock(&rw->lock);
#else
    omp_set_lock(&rw->lock);
#endif
}

/**
 * Unset lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_unset_rlock(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_unlock(&rw->lock);
#else
    omp_unset_lock(&rw->lock);
#endif
}

/**
 * Set lock for writing. 
 * @param rw pointer to lock structure
 */
void rwlock_set_wlock(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_wrlock(&rw->lock);
#else
    omp_set_lock(&rw->lock);
#endif
}

/**
 * Unset lock for writing.
 * @param rw pointer to lock structure
 */
void rwlock_unset_wlock(rwlock_t *rw)
{
#ifdef HAVE_PTHREAD_H
    pthread_rwlock_unlock(&rw->lock);
#else
    omp_unset_lock(&rw->lock);
#endif
}

/** @} */
