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


#ifndef UTIL_H
#define UTIL_H

#include <zlib.h>
#include "config.h"

/* Progress bar stuff */
#define PROGBAR_LEN     52
#define PROGBAR_EMPTY   ':'
#define PROGBAR_FULL    '#'
#define PROGBAR_DONE    '#'
#define PROGBAR_FRONT   '|'

/* Fatal message */
#ifndef fatal
#define fatal(...)     {err_msg("Error", __func__, __VA_ARGS__); exit(-1);}
#endif
/* Error message */
#ifndef error
#define error(...)     {err_msg("Error", __func__, __VA_ARGS__);}
#endif
/* Warning message */
#ifndef warning
#define warning(...)   {err_msg("Warning", __func__, __VA_ARGS__);}
#endif

/* Utility functions */
void err_msg(char *, const char *, char *, ...);
void info_msg(int, char *, ...);
double time_stamp();
void prog_bar(long, long, long);
size_t gzgetline(char **s, size_t * n, gzFile f);
void strtrim(char *x);
int decode_str(char *str);
uint64_t hash_str(char *s, int l); 
int strip_newline(char *s, int l);

#endif /* UTIL_H */
