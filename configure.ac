# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)

AC_INIT([harry], [0.2], [konrad@mlsec.org])
AC_PREFIX_DEFAULT("/usr/local")

echo
echo "  Harry - A Tool for Measuring String Similarity"
echo "  Copyright (c) 2013-2014 Konrad Rieck (konrad@mlsec.org)"
echo

# Init automake & config.h
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

# Check for important programs
AC_PROG_CC
AC_PROG_LD
AC_PROG_INSTALL

# Libtool stuff
AC_CONFIG_MACRO_DIR(m4)
AC_PROG_LIBTOOL

# Set GCC and C99 flags if present
if test "$GCC" = "yes" ; then
   CFLAGS="$CFLAGS -std=c99 -Wall -fPIC"
fi

# libm
AC_CHECK_HEADERS([math.h], HEADER_MATH="yes")
AC_CHECK_LIB([m], pow, LIBRARY_MATH="yes")
if test "x$LIBRARY_MATH" != "x" && test "x$HEADER_MATH" != "x" ; then
    LIBS="-lm $LIBS"
else
    AC_MSG_ERROR([The library libm is required for compilation])
fi

# libz
AC_CHECK_HEADERS([zlib.h], HEADER_ZLIB="yes")
AC_CHECK_LIB([z], gzopen, LIBRARY_ZLIB="yes")
if test "x$LIBRARY_ZLIB" != "x" && test "x$HEADER_ZLIB" != "x" ; then
    LIBS="-lz $LIBS"
else
    AC_MSG_ERROR([The library libz is required for compilation.
    Debian/Ubuntu package: libz-dev
    Source code: http://www.zlib.net/
    ])
fi

# libconfig
PKG_CHECK_MODULES([PKGCONFIG], [libconfig >= 1.4], [], 
    AC_MSG_ERROR([The library libconfig is required for compilation.
    Debian/Ubuntu package: libconfig9-dev
    Source code: http://www.hyperrealm.com/libconfig/
    ])
)

CFLAGS="$CFLAGS $PKGCONFIG_CFLAGS"
LIBS="$LIBS $PKGCONFIG_LIBS"
AM_CPPFLAGS="$AM_CPPFLAGS `pkg-config --cflags-only-I libconfig`"
AC_CHECK_HEADERS([libconfig.h], HEADER_LIBCONFIG="yes")

# OpenMP 
AC_CHECK_HEADERS([omp.h], HEADER_OPENMP="yes")
AX_OPENMP([HAVE_OPENMP=yes],[HAVE_OPENMP=no])
if test "x$HAVE_OPENMP" = "xno" ; then
    AC_MSG_ERROR([OpenMP required for computation.
    GCC implements OpenMP as of version >=4.2.
    ])
fi
CFLAGS="$CFLAGS $OPENMP_CFLAGS"

# libarchive
AC_ARG_ENABLE(libarchive,
    [  --enable-libarchive     enable support for loading archives. ],
    [
        if test "x$enableval" = "xyes" ; then
                ENABLE_LIBARCHIVE=yes
                AC_DEFINE(ENABLE_LIBARCHIVE, 1, [Enable support for loading archives])
        else
                ENABLE_LIBARCHIVE=no
        fi
    ], [ ENABLE_LIBARCHIVE=no ]
)


AC_CHECK_HEADERS([archive.h], HEADER_LIBARCHIVE="yes")
AC_CHECK_LIB([archive], archive_read_new, LIBRARY_LIBARCHIVE="yes")

if test "x$ENABLE_LIBARCHIVE" = "xyes" ; then
    if test "x$LIBRARY_LIBARCHIVE" != "x" && test "x$HEADER_LIBARCHIVE" != "x" ; then
        LIBS="-larchive $LIBS"
    else
        AC_MSG_ERROR([libarchive requested but not available
    Debian/Ubuntu package: libarchive-dev
    Source code: http://code.google.com/p/libarchive
        ])
    fi
fi

# POSIX threads
AC_ARG_ENABLE(pthread,
    [  --enable-pthread        enable support for POSIX thread rwlocks. ],
    [
        if test "x$enableval" = "xyes" ; then
                ENABLE_PTHREAD=yes
                AC_DEFINE(ENABLE_PTHREAD, 1, [Enable support for POSIX thread rwlocks])
        else
                ENABLE_PTHREAD=no
        fi
    ], [ ENABLE_PTHREAD=no ]
)

AC_CHECK_HEADERS([pthread.h], HEADER_PTHREAD="yes")
AC_CHECK_LIB([pthread], pthread_rwlock_init, LIBRARY_PTHREAD="yes")

if test "x$ENABLE_PTHREAD" = "xyes" ; then
    if test "x$LIBRARY_PTHREAD" != "x" && test "x$HEADER_PTHREAD" != "x" ; then
        LIBS="-lpthread $LIBS"
    else
        AC_MSG_ERROR([POSIX thread rwlocks requested but not available
        ])
    fi
fi

# Check headers
AC_CHECK_HEADERS([getopt.h string.h strings.h regex.h])
AC_CHECK_HEADERS([uthash.h uthash/uthash.h], HEADER_UTHASH="yes")

# Check functions
AC_CHECK_FUNC(log2, AC_DEFINE(HAVE_FUNC_LOG2, 1,
       [Define to 1 if you have the function log2]))

# MD5 hash flag
AC_ARG_ENABLE(md5hash,
    [  --enable-md5hash        enable MD5 as alternative hash ],
    [
        if test "x$enableval" = "xyes" ; then
                ENABLE_MD5HASH=yes
                AC_DEFINE(ENABLE_MD5HASH, 1, [Enable MD5 as alternative hash])
        else
                ENABLE_MD5HASH=no
        fi
    ], [ ENABLE_MD5HASH=no ]
)

AC_SUBST([AM_CPPFLAGS])
AC_CONFIG_FILES([ 
   Makefile \
   src/Makefile \
   src/measures/Makefile \
   src/input/Makefile \
   src/output/Makefile \
   tests/Makefile \
   doc/Makefile \
])
AC_OUTPUT

echo
echo " .Oo Features:"
echo "     Support for loading archives (--enable-libarchive):    $ENABLE_LIBARCHIVE"
echo "     Support for POSIX thread rwlocks (--enable-pthread):   $ENABLE_PTHREAD"
echo "     MD5 as alternative hash (--enable-md5hash):            $ENABLE_MD5HASH"
echo

