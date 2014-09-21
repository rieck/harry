/*
 * Implementation of Jaro-Winkler Distance
 * Copyright (C) 2011 Miguel Serrano
 * Copyright (C) 2002-2003 David Necas (Yeti) <yeti@physics.muni.cz>.
 * Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
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
 *
 * Jaro. Advances in record linkage methodology as applied to the 1985
 * census of Tampa Florida. Journal of the American Statistical
 * Association 84 (406): 414-420, 1989.
 *
 * Winkler.  String Comparator Metrics and Enhanced Decision Rules in the
 * Fellegi-Sunter Model of Record Linkage. Proceedings of the Section on
 * Survey Research Methods. 354-359, 1990.
 * @{
 */

/* External variables */
extern config_t cfg;

/* Global variables */
static double scaling = 0.1;

/* Some help functions */
inline int max(int x, int y)
{
    return x > y ? x : y;
}

inline int min(int x, int y)
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


#ifdef JARO_COMPARE_SERRANO
/**
 * Computes the Jaro distance of two strings. Code adapted
 * from implementation by Miguel Serrano
 * @param x first string
 * @param y second string
 * @return Jaro distance
 */
static float dist_jaro_compare_serrano(hstring_t x, hstring_t y)
{
    int i, j, l;
    int m = 0, t = 0;
    int range = max(0, max(x.len, y.len) / 2 - 1);

    if (x.len == 0 && y.len == 0)
        return 0.0;

    char *xflags = calloc(sizeof(char), x.len);
    if (!xflags) {
        error("Could not allocate memory for Jaro distance");
        return 0;
    }

    char *yflags = calloc(sizeof(char), y.len);
    if (!yflags) {
        error("Could not allocate memory for Jaro distance");
        return 0;
    }

    /* Calculate matching characters */
    for (i = 0; i < y.len; i++) {
        for (j = max(i - range, 0), l = min(i + range + 1, x.len); j < l; j++) {
            if (!hstring_compare(y, i, x, j) && !xflags[j]) {
                xflags[j] = 1;
                yflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (m == 0)
        return 1.0;

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
            if (hstring_compare(y, i, x, j))
                t++;
        }
    }
    t /= 2;

    free(xflags);
    free(yflags);

    return 1 - ((((float) m / x.len) + ((float) m / y.len) +
                 ((float) (m - t) / m)) / 3.0);
}
#else
/**
 * Computes the Jaro distance of two strings. Code adapted from
 * from implementation by David Necas (Yeti).
 * @param x first string
 * @param y second string
 * @return Jaro distance
 */
static float dist_jaro_compare_yeti(hstring_t x, hstring_t y)
{
    int i, j, halflen, trans, match, to;
    int *idx;
    float md;

    if (x.len == 0 || y.len == 0) {
        if (x.len == 0 && y.len == 0)
            return 0.0;
        return 1.0;
    }
    /* make x.len always shorter (or equally long) */
    if (x.len > y.len) {
        hstring_t z;
        z = x;
        x = y;
        y = z;
    }

    halflen = (x.len + 1) / 2;
    idx = (int *) calloc(x.len, sizeof(int));
    if (!idx) {
        error("Failed to allocate memory for Jaro distance");
        return 0;
    }

    /* The literature about Jaro metric is confusing as the method of assigment
     * of common characters is nowhere specified.  There are several possible
     * deterministic mutual assignments of common characters of two strings.
     * We use earliest-position method, which is however suboptimal (e.g., it
     * gives two transpositions in jaro("Jaro", "Joaro") because of assigment
     * of the first `o').  No reasonable algorithm for the optimal one is
     * currently known to me. */
    match = 0;
    /* the part with allowed range overlapping left */
    for (i = 0; i < halflen; i++) {
        for (j = 0; j <= i + halflen; j++) {
            if (!hstring_compare(x, j, y, i) && !idx[j]) {
                match++;
                idx[j] = match;
                break;
            }
        }
    }

    /* the part with allowed range overlapping right */
    to = x.len + halflen < y.len ? x.len + halflen : y.len;
    for (i = halflen; i < to; i++) {
        for (j = i - halflen; j < x.len; j++) {
            if (!hstring_compare(x, j, y, i) && !idx[j]) {
                match++;
                idx[j] = match;
                break;
            }
        }
    }
    if (!match) {
        free(idx);
        return 1.0;
    }
    /* count transpositions */
    i = 0;
    trans = 0;
    for (j = 0; j < x.len; j++) {
        if (idx[j]) {
            i++;
            if (idx[j] != i)
                trans++;
        }
    }
    free(idx);

    md = (float) match;
    return 1.0 - (md / x.len + md / y.len + 1.0 - trans / md / 2.0) / 3.0;
}
#endif

/**
 * Computes the Jaro-Winkler distance of two strings.
 * @param x first string
 * @param y second string
 * @return Jaro-Winkler distance
 */
float dist_jaro_compare(hstring_t x, hstring_t y)
{
#ifdef JARO_COMPARE_SERRANO
    return dist_jaro_compare_serrano(x, y);
#else
    return dist_jaro_compare_yeti(x, y);
#endif
}

/**
 * Computes the Jaro-Winkler distance of two strings.
 * @param x first string
 * @param y second string
 * @return Jaro-Winkler distance
 */
float dist_jarowinkler_compare(hstring_t x, hstring_t y)
{
    int l;
    float d = dist_jaro_compare(x, y);

    /* Calculate common string prefix up to 4 chars */
    int m = min(min(x.len, y.len), 4);
    for (l = 0; l < m; l++)
        if (hstring_compare(x, l, y, l))
            break;

    /* Jaro-Winkler distance */
    return d - l * scaling * d;
}

/** @} */
