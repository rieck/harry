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

#ifndef SIM_COEFFICIENTS_H
#define SIM_COEFFICIENTS_H

#include "hstring.h"

typedef struct
{
    float a;    /**< Number of matching symbols */
    float b;    /**< Number of left mismatches */
    float c;    /**< Number of right mismatches */
} match_t;

void sim_coefficient_config();

#define sim_jaccard_config sim_coefficient_config
float sim_jaccard_compare(hstring_t x, hstring_t y);

#define sim_simpson_config sim_coefficient_config
float sim_simpson_compare(hstring_t x, hstring_t y);

#define sim_braunblanquet_config sim_coefficient_config
float sim_braunblanquet_compare(hstring_t x, hstring_t y);

#define sim_czekanowski_config sim_coefficient_config
float sim_czekanowski_compare(hstring_t x, hstring_t y);

#define sim_sokalsneath_config sim_coefficient_config
float sim_sokalsneath_compare(hstring_t x, hstring_t y);

#define sim_kulczynski1_config sim_coefficient_config
float sim_kulczynski1_compare(hstring_t x, hstring_t y);

#define sim_kulczynski2_config sim_coefficient_config
float sim_kulczynski2_compare(hstring_t x, hstring_t y);

#define sim_otsuka_config sim_coefficient_config
float sim_otsuka_compare(hstring_t x, hstring_t y);

#endif /* SIM_COEFFICIENTS_H */
