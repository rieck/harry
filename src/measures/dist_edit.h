/*
 * SIMONE - Similarity Measures for Structured Data
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

#ifndef DIST_EDIT_H
#define DIST_EDIT_H

#ifdef HAVE_LIBCONFIG_H
#include <libconfig.h>
#endif

/* Module interface */
void dist_edit_init(config_t *);
double dist_edit_cmp(void *, void *);
void *dist_edit_read(FILE *);
void dist_edit_free(void *);

#endif /* DIST_EDIT_H */
