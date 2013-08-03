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

#ifndef KERN_SUBSEQ_H
#define KERN_SUBSEQ_H

#ifdef HAVE_LIBCONFIG_H
#include <libconfig.h>
#endif

/* Module interface */
void kern_subseq_init(config_t *);
double kern_subseq_cmp(void *, void *);
void *kern_subseq_read(FILE *);
void kern_subseq_free(void *);

#endif /* KERN_SUBSEQ_H */
