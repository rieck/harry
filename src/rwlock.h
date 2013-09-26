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

#ifndef RWLOCK_H
#define RWLOCK_H

#include <omp.h>

typedef struct
{
    int cnt;
    omp_lock_t cnt_lock;
    omp_lock_t sem_lock;
} sem_t;

typedef struct
{
    omp_lock_t up_lock;         /**< Writer lock */
    omp_lock_t down_lock;       /**< Writer lock */
    sem_t semaphore;            /**< Semaphore */
    int readers;                /**< Number of readers */
} rwlock_t;

/* Semaphore */
void sem_init(sem_t *, int);
void sem_destroy(sem_t *);
void sem_up(sem_t *);
void sem_down(sem_t *);
int sem_value(sem_t *);

/* RW lock */
void rwlock_init(rwlock_t *rw, int);
void rwlock_destroy(rwlock_t *rw);
void rwlock_set_rlock(rwlock_t *rw);
void rwlock_unset_rlock(rwlock_t *rw);
void rwlock_set_wlock(rwlock_t *rw);
void rwlock_unset_wlock(rwlock_t *rw);

#endif
