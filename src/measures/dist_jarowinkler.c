/*
 * Implementation of Jaro-Winkler Distance
 * Copyright (C) 2011 Miguel Serrano
 *           (C) 2013 Konrad Rieck
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "util.h"

#include "dist_jarowinkler.h"

/**
 * @addtogroup measures
 * <hr>
 * <em>dist_jarowinkler</em>: Jaro-Winkler distance for strings.
 * @{
 */

/* External variables */
extern config_t cfg;

/* Global variables */
static double scaling = 0.1;

/* Some help functions */
static int max(int x, int y)
{
    return x > y ? x : y;
}

static int min(int x, int y)
{
    return x < y ? x : y;
}

/**
 * Initializes the similarity measure
 */
void dist_jarowinkler_config()
{
    config_lookup_float(&cfg, "measures.dist_jarowinkler.scaling", &scaling);
}

/**
 * Computes the Jaro-Winkler distance of two strings. 
 * @param x first string 
 * @param y second string
 * @return Jaro-Winkler distance
 */
float dist_jarowinkler_compare(str_t x, str_t y)
{
    int i, j, l;
    int m = 0, t = 0;
    int xflags[x.len], yflags[y.len];
    int range = max(0, max(x.len, y.len) / 2 - 1);
    float dw;

    if (x.len == 0 || y.len == 0)
        return 0.0;

    for (i = 0; i < y.len; i++)
        yflags[i] = 0;

    for (i = 0; i < x.len; i++)
        xflags[i] = 0;

    /* Calculate matching characters */
    for (i = 0; i < y.len; i++) {
        for (j = max(i - range, 0), l = min(i + range + 1, x.len); j < l; j++) {
            if (y.str.s[i] == x.str.s[j] && !xflags[j]) {
                xflags[j] = 1;
                yflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (!m)
        return 0.0;

    /* Calculate character transpositions */
    l = 0;
    for (i = 0; i < y.len; i++) {
        if (yflags[i] == 1) {
            for (j = l; j < x.len; j++) {
                if (xflags[j] == 1) {
                    l = j + 1;
                    break;
                }
            }
            if (y.str.s[i] != x.str.s[j])
                t++;
        }
    }
    t /= 2;

    /* Jaro distance */
    dw = (((float) m / x.len) + ((float) m / y.len) +
          ((float) (m - t) / m)) / 3.0;

    /* Calculate common string prefix up to 4 chars */
    for (l = 0; l < min(min(x.len, y.len), 4); l++)
        if (x.str.s[l] != y.str.s[l])
            break;

    /* Jaro-Winkler distance */
    dw = dw + (l * scaling * (1 - dw));
    return dw;
}

/** @} */
