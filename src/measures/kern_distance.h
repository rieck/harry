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

#ifndef KERN_DISTANCE_H
#define KERN_DISTANCE_H

#include "hstring.h"

typedef enum
{
    DS_LINEAR, DS_POLY, DS_NEG, DS_RBF
} subst_t;

/* Module interface */
void kern_distance_config();
float kern_distance_compare(hstring_t, hstring_t);

#endif /* KERN_DISTANCE_H */
