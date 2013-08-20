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

#ifndef KERN_WDEGREE_H
#define KERN_WDEGREE_H

#include "str.h"

/* Module interface */
void kern_wdegree_config();
float kern_wdegree_compare(str_t, str_t);

#endif /* KERN_WDEGREE_H */
