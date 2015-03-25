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

#ifndef RWLOCK_H
#define RWLOCK_H

#ifdef ENABLE_PRWLOCK
#include <pthread.h>
#endif

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

typedef struct
{
#ifdef ENABLE_PRWLOCK
    pthread_rwlock_t lock;
#else
#ifdef HAVE_OPENMP
    omp_lock_t lock;            /* Workaround: Global mutex :( */
#else
    int empty;                  /* ;) */
#endif
#endif
} rwlock_t;

/* RW lock */
void rwlock_init(rwlock_t *rw);
void rwlock_destroy(rwlock_t *rw);
void rwlock_set_rlock(rwlock_t *rw);
void rwlock_unset_rlock(rwlock_t *rw);
void rwlock_set_wlock(rwlock_t *rw);
void rwlock_unset_wlock(rwlock_t *rw);

#endif
