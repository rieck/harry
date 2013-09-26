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

/**
 * @defgroup util Utility functions
 * Collection of utility functions for Harry.
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "harry.h"
#include "murmur.h"
#include "md5.h"
#include "vcache.h"


/* Progress bar stuff */
#define PROGBAR_LEN     35
#define PROGBAR_EMPTY   ':'
#define PROGBAR_FULL    '#'
#define PROGBAR_DONE    '#'
#define PROGBAR_FRONT   '#'

/* External variable */
extern int verbose;
/* Global variable */
static double time_start = -1;
/** Progress bar (with NULL) */
static char pb_string[PROGBAR_LEN + 1];
/** Start timestamp measured */
static double pb_start = -1;

/**
 * Print a formated info message with timestamp. 
 * @param v Verbosity level of message
 * @param m Format string
 */
void info_msg(int v, char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    if (time_start == -1)
        time_start = time_stamp();

    if (v > verbose)
        return;

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    fprintf(stderr, "> %s\n", s);
    fflush(stderr);
}

/**
 * Print a formated error/warning message. See the macros error and 
 * warning in util.h
 * @param p Prefix string, e.g. "Error"
 * @param f Function name
 * @param m Format string
 */
void err_msg(char *p, const char *f, char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    fprintf(stderr, "%s: %s (", p, s);
    if (errno)
        fprintf(stderr, "%s, ", strerror(errno));
    fprintf(stderr, "%s)\n", f);
    errno = 0;
}

/**
 * Print a formated a debug message
 * @param m Format string
 */
void debug_msg(char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    if (time_start == -1)
        time_start = time_stamp();

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    fprintf(stderr, "[%d] %s\n", omp_get_thread_num(), s);
    fflush(stderr);
}

/**
 * Return a timestamp of the real time
 * @return time stamp
 */
double time_stamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

/**
 * Print a progress bar in a given range.
 * @param a Minimum value 
 * @param b Maximum value
 * @param c Current value
 */
void prog_bar(long a, long b, long c)
{
    int i, first, last;
    double perc, ptime = 0, min, max, in;
    char *descr = "";

    if (verbose == 0)
        return;

    min = (double) a;
    max = (double) b;
    in = (double) c;

    perc = (in - min) / (max - min);
    first = fabs(in - min) < 1e-10;
    last = fabs(in - max) < 1e-10;

    /* Start of progress */
    if (pb_start < 0 || (first && !last)) {
        pb_start = time_stamp();
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_EMPTY;
        descr = "start";
        perc = 0.0;
    }

    /* End of progress */
    if (last) {
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_FULL;
        ptime = time_stamp() - pb_start;
        descr = "total";
        perc = 1.0;
        pb_start = -1;
    }

    /* Middle of progress */
    if (!first && !last) {
        int len = (int) round(perc * PROGBAR_LEN);
        for (i = 0; i < len; i++)
            if (i < len - 1)
                pb_string[i] = PROGBAR_DONE;
            else
                pb_string[i] = PROGBAR_FRONT;
        ptime = (max - in) * (time_stamp() - pb_start) / (in - min);
        descr = "   in";
    }

    int mins = (int) floor(ptime / 60);
    int secs = (int) floor(ptime - mins * 60);
    pb_string[PROGBAR_LEN] = 0;

    printf("\r[%.2d][%s %3.0f%% %s %.2dm %.2ds][%3.0f%% %5.1fMb]",
           omp_get_num_threads(), pb_string, perc * 100, descr,
           mins, secs, vcache_get_hitrate(), vcache_get_used());

    if (last)
        printf("\n");

    fflush(stdout);
    fflush(stderr);
}

#define BLOCK_SIZE 4096

/**
 * Dirty re-write of the GNU getline() function. I have been
 * searching the Web for a couple of minutes to find a suitable 
 * implementation. Unfortunately, I could not find anything 
 * appropriate. Some people confused fgets() with getline(), 
 * others were arguing on licenses over and over.
 */
size_t gzgetline(char **s, size_t * n, gzFile f)
{
    assert(f);
    int c = 0;
    *n = 0;

    if (gzeof(f))
        return -1;

    while (c != '\n') {
        if (!*s || *n % BLOCK_SIZE == 0) {
            *s = realloc(*s, *n + BLOCK_SIZE + 1);
            if (!*s)
                return -1;
        }

        c = gzgetc(f);
        if (c == -1)
            return -1;

        (*s)[(*n)++] = c;
    }

    (*s)[*n] = 0;
    return *n;
}

/** 
 * Another dirty function to trim strings from leading and trailing 
 * blanks. The original string is modified in place.
 * @param x Input string
 */
void strtrim(char *x)
{
    assert(x);
    int i = 0, j = 0, l = strlen(x);

    if (l == 0)
        return;

    for (i = 0; i < l; i++)
        if (!isspace(x[i]))
            break;

    for (j = l; j > 0; j--)
        if (!isspace(x[j - 1]))
            break;

    if (j > i) {
        memmove(x, x + i, j - i);
        x[j - i] = 0;
    } else {
        x[0] = 0;
    }
}

/**
 * Returns the number of a hexadecimal digit 
 * @param c Byte containing digit
 * @private
 * @return number
 */
static int get_hex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 0xa;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 0xa;

    warning("Invalid URI encoding (%c)", c);
    return 0;
}

/**
 * Decodes a string with URI encoding. The function operates 
 * in-place. A trailing NULL character is appended to the string.
 * @param str String to decode.
 * @return length of decoded sequence
 */
int decode_str(char *str)
{
    int j, k, r;

    /* Loop over string */
    for (j = k = 0; j < strlen(str); j++, k++) {
        if (str[j] != '%') {
            str[k] = str[j];
        } else {
            /* Check for truncated string */
            if (strlen(str) - j < 2)
                break;

            /* Parse hexadecimal number */
            r = get_hex(str[j + 1]) * 16 + get_hex(str[j + 2]);
            j += 2;
            str[k] = (char) r;
        }
    }
    str[k] = 0;

    return k;
}

/**
 * Strip newline characters in place
 * @param str input string
 * @param len length of string
 */
int strip_newline(char *str, int len)
{
    int k;
    static char strip[256] = { 0 };
    strip[(int) '\n'] = 1;
    strip[(int) '\r'] = 1;

    assert(str);

    for (k = len - 1; k >= 0; k--) {
        if (!strip[(int) str[k]]) {
            break;
        }
    }

    str[k + 1] = 0x00;
    return k + 1;
}

/**
 * Hashes a string to a 64 bit hash. Utility function to limit
 * the clatter of code.
 * @param s Byte sequence
 * @param l Length of sequence
 * @return hash value
 */
uint64_t hash_str(char *s, int l)
{
    uint64_t ret = 0;

#ifdef ENABLE_MD5HASH
    unsigned char buf[MD5_DIGEST_LENGTH];
#endif

#ifdef ENABLE_MD5HASH
    MD5((unsigned char *) s, l, buf);
    memcpy(&ret, buf, sizeof(int64_t));
#else
    ret = MurmurHash64B(s, l, 0x12345678);
#endif

    return ret;
}

/** @} */
