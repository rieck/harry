/*
 * Harry - Similarity Measures for Strings
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

#include "input.h"

/* Module functions */
void measure_config(const char *);
void measure_init();
double measure_compare(string_t *, string_t *);
void measure_free(void *);

#endif /* MEASURES_H */
