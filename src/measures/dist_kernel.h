/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

#ifndef DIST_KERNEL_H
#define DIST_KERNEL_H

#include "hstring.h"

/* Module interface */
void dist_kernel_config();
float dist_kernel_compare(hstring_t, hstring_t);

#endif /* DIST_KERNEL_H */
