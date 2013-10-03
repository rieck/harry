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
 * Read-write lock implemented using OpenMP and a semaphore.
 *
 * @author Konrad Rieck (konrad@mlsec.org)
 * @author Henrik Brosenne (brosenne@cs.uni-goettingen.de)
 * @{
 */

/** 
 * Init a semaphore
 * @param s Pointer to semaphore
 * @param c Start value 
 */
void sem_init(sem_t * s, int c)
{
    omp_init_lock(&s->cnt_lock);
    omp_init_lock(&s->sem_lock);

    s->cnt = c;
    omp_set_lock(&s->sem_lock);
}

/**
 * Destroy a semaphore
 * @param s pointer to semaphore
 */
void sem_destroy(sem_t * s)
{
    omp_destroy_lock(&s->cnt_lock);
    omp_destroy_lock(&s->sem_lock);
}

/**
 * Down the semaphore
 * @param s pointer to semaphore
 */
void sem_down(sem_t * s)
{
    omp_set_lock(&s->cnt_lock);
    s->cnt--;
    if (s->cnt < 0) {
        omp_unset_lock(&s->cnt_lock);
        omp_set_lock(&s->sem_lock);
    } else {
        omp_unset_lock(&s->cnt_lock);
    }
}

/**
 * Up the semaphore
 * @param s pointer to semaphore
 */
void sem_up(sem_t * s)
{
    omp_set_lock(&s->cnt_lock);
    s->cnt++;
    if (s->cnt <= 0) {
        omp_unset_lock(&s->cnt_lock);
        omp_unset_lock(&s->sem_lock);
    } else {
        omp_unset_lock(&s->cnt_lock);
    }
}

/**
 * Return the semaphore counter
 * @param s pointer to semaphore
 * @return current counter
 */
int sem_value(sem_t * s)
{
    int r;
    omp_set_lock(&s->cnt_lock);
    r = s->cnt;
    omp_unset_lock(&s->cnt_lock);
    return r;
}

/**
 * Init a read-write lock.
 * @param rw pointer to lock structure
 * @param r number of readers
 */
void rwlock_init(rwlock_t *rw, int r)
{
    omp_init_lock(&rw->wrt_lock);
    sem_init(&rw->semaphore, r);
    rw->readers = r;
}

/**
 * Destroy a read-write lock.
 * @param rw pointer to lock structure
 */
void rwlock_destroy(rwlock_t *rw)
{
    sem_destroy(&rw->semaphore);
    omp_destroy_lock(&rw->wrt_lock);
}

/**
 * Set lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_set_rlock(rwlock_t *rw)
{
    sem_down(&rw->semaphore);
}

/**
 * Unset lock for reading. 
 * @param rw pointer to lock structure
 */
void rwlock_unset_rlock(rwlock_t *rw)
{
    sem_up(&rw->semaphore);
}

/**
 * Set lock for writing. 
 * @param rw pointer to lock structure
 */
void rwlock_set_wlock(rwlock_t *rw)
{
    /* No two writer should acquire the lock concurrently */
    omp_set_lock(&rw->wrt_lock);
    for (int i = 0; i < rw->readers; i++) 
        sem_down(&rw->semaphore);
    omp_unset_lock(&rw->wrt_lock);
}

/**
 * Unset lock for writing.
 * @param rw pointer to lock structure
 */
void rwlock_unset_wlock(rwlock_t *rw)
{
    for (int i = 0; i < rw->readers; i++) 
        sem_up(&rw->semaphore);
}

/** @} */
