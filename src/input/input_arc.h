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

#ifndef INPUT_ARC_H
#define INPUT_ARC_H

#include "hstring.h"

#ifdef HAVE_LIBARCHIVE
/* Archive module */
int input_arc_open(char *);
int input_arc_read(hstring_t *, int);
void input_arc_close(void);
#endif

#endif /* INPUT_ARC_H */
