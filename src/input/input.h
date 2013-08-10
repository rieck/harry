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

#ifndef INPUT_H
#define INPUT_H

#include "str.h"

/* Configuration */
void input_config(const char *);
void input_free(str_t *strs, int len);
void input_preproc(str_t *strs, int len);

/* Generic interface */
int input_open(char *);
int input_read(str_t *, int);
void input_close(void);

/* Additional functions */
void stopwords_load(const char *f);
void stopwords_destroy();

#endif /* INPUT_H */
