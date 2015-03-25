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

#ifndef COMMON_H
#define COMMON_H

#define _BSD_SOURCE             /* Linux: strdup() */
#define __USE_POSIX             /* Linux: readdir_r() */
#define _SVID_SOURCE            /* Linux: lrand48() */

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__          /* Required for pthread_rwlock */
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Standard C headers */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <regex.h>

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#ifdef HAVE_LIBCONFIG_H
#include <libconfig.h>
/* Patch for changed ABI in libconfig versions */
#if LIBCONFIG_VER_MAJOR > 1 || LIBCONFIG_VER_MINOR > 3
typedef int cfg_int;
#else
typedef long cfg_int;
#endif
#endif

#ifdef HAVE_UTHASH_UTHASH_H
#include <uthash/uthash.h>
#else
#ifdef HAVE_UTHASH_H
#include <uthash.h>
#else
#include "uthash.h"
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#endif /* COMMON_H */
