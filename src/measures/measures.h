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

#ifndef MEASURES_H
#define MEASURES_H

#include "hstring.h"

/* Module functions */
char *measure_config(const char *);
double measure_compare(hstring_t, hstring_t);

#endif /* MEASURES_H */
