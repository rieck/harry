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

#ifndef OUTPUT_TEXT_H
#define OUTPUT_TEXT_H

/* text output module */
int output_text_open(char *);
int output_text_write(hmatrix_t *);
void output_text_close(void);

#endif /* OUTPUT_TEXT_H */
