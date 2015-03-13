# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)

AC_INIT([harry], [0.4.0], [konrad@mlsec.org])
AC_PREFIX_DEFAULT("/usr/local")

echo
echo "  Harry - A Tool for Measuring String Similarity"
echo "  Copyright (c) 2013-2015 Konrad Rieck (konrad@mlsec.org)"
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

# By default remove assert statements
CFLAGS="$CFLAGS -DNDEBUG"

# By default we include the math library
LIBS="$LIBS -lm"

# Set GCC and C99 flags if present
if test "$GCC" = "yes" ; then
   CFLAGS="$CFLAGS -std=c99 -fgnu89-inline -Wall -fPIC"
fi

# Check for Python
AM_PATH_PYTHON(,, [:])
AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != :])

# Optional packages
AC_ARG_WITH([libarchive], [AS_HELP_STRING([--with-libarchive],
            [support for reading archives @<:@default=check@:>@])],
            [], [with_libarchive=check])
AC_ARG_WITH([openmp], [AS_HELP_STRING([--with-openmp],
            [support for multi-processing @<:@default=check@:>@])],
            [], [with_openmp=check])
AC_ARG_WITH([pthreads], [AS_HELP_STRING([--with-pthreads],
            [support POSIX threads and locks @<:@default=check@:>@])],
            [], [with_pthreads=check])


# Check for zlib (required)
AC_CHECK_HEADERS([zlib.h], HEADER_ZLIB="yes")
AC_CHECK_LIB([z], gzopen, LIBRARY_ZLIB="yes")
if test "x$LIBRARY_ZLIB" != "x" && test "x$HEADER_ZLIB" != "x" ; then
    LIBS="-lz $LIBS"
    AC_DEFINE([HAVE_ZLIB], [1], [Define if you have zlib])
    HAVE_ZLIB=yes
else
    HAVE_ZLIB=no
    AC_MSG_FAILURE([libz not found. see README.md])
fi

# Check for libarchive (optional)
AC_CHECK_HEADERS([archive.h], HEADER_LIBARCHIVE="yes")
AC_CHECK_LIB([archive], archive_read_new, LIBRARY_LIBARCHIVE="yes")
if test "x$LIBRARY_LIBARCHIVE" != "x" && \
   test "x$HEADER_LIBARCHIVE" != "x" && \
   test "x$with_libarchive" != "xno" ; then
    AC_DEFINE([HAVE_LIBARCHIVE], [1], [Define if you have libarchive])
    LIBS="-larchive $LIBS"
    HAVE_LIBARCHIVE=yes
else
    HAVE_LIBARCHIVE=no
    if test "x$with_libarchive" == "xyes" ; then
        AC_MSG_FAILURE([libarchive not found. see README.md])
    fi
fi

# Check for libconfig (required)
AC_CHECK_HEADERS([libconfig.h], HEADER_LIBCONFIG="yes")
PKG_CHECK_MODULES([PKGCONFIG], [libconfig >= 1.3.2], LIBRARY_LIBCONFIG="yes")
if test "x$LIBRARY_LIBCONFIG" != "x" && test "x$HEADER_LIBCONFIG" != "x" ; then
    CFLAGS="$CFLAGS $PKGCONFIG_CFLAGS"
    LIBS="$LIBS $PKGCONFIG_LIBS"
    AM_CPPFLAGS="$AM_CPPFLAGS `pkg-config --cflags-only-I libconfig`"
    HAVE_LIBCONFIG=yes
    AC_DEFINE([HAVE_LIBCONFIG], [1], [Define if you have libconfig])
else
    HAVE_LIBCONFIG=no
    AC_MSG_FAILURE([libconfig not found. see README.md])
fi

# Check for OpenMP (optional)
AC_CHECK_HEADERS([omp.h], HEADER_OPENMP="yes")
AX_OPENMP(LIBRARY_OPENMP="yes")
if test "x$LIBRARY_OPENMP" != "x" && \
   test "x$HEADER_OPENMP" != "x" && \
   test "x$with_openmp" != "xno" ; then
    CFLAGS="$CFLAGS $OPENMP_CFLAGS"
    HAVE_OPENMP=yes
    AC_DEFINE([HAVE_OPENMP], [1], [Define if you have OpenMP])
else
    HAVE_OPENMP=no
    if test "x$with_openmp" == "xyes" ; then
        AC_MSG_FAILURE([no openmp support. see README.md])
    fi
fi

# Check for libpthread (optional)
AC_CHECK_HEADERS([pthread.h], HEADER_PTHREAD="yes")
AC_CHECK_LIB([pthread], pthread_rwlock_init, LIBRARY_PTHREAD="yes")
if test "x$LIBRARY_PTHREAD" != "x" && \
   test "x$HEADER_PTHREAD" != "x" && \
   test "x$with_pthreads" != "xno" ; then
    HAVE_PTHREADS=yes
    CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
    LIBS="-lpthread $LIBS"
    AC_DEFINE([HAVE_PTHREADS], [1], [Define if you have posix threads])
else
    HAVE_PTHREADS=no
    if test "x$with_pthreads" == "xyes" ; then
        AC_MSG_FAILURE([libpthread not found. see README.md])
    fi
fi

# Optional features
AC_ARG_ENABLE([prwlock], [AS_HELP_STRING([--enable-prwlock],
    [enable POSIX read-write lock])],
    [
        if test "x$HAVE_PTHREADS" != "xyes" ; then
            AC_MSG_FAILURE([libpthread not found. see README.md])
        fi
        AC_DEFINE([ENABLE_PRWLOCK], [1], [Define if POSIX read-write lock enabled])
        ENABLE_PRWLOCK=yes
    ], [ ENABLE_PRWLOCK=no ])

AC_ARG_ENABLE([md5hash], [AS_HELP_STRING([--enable-md5hash],
    [enable MD5 hash function])],
    [
        AC_DEFINE([ENABLE_MD5HASH], [1], [Define if MD5 hash enabled])
        ENABLE_MD5HASH=yes
    ], [ENABLE_MD5HASH=no])


# Check headers
AC_CHECK_HEADERS([getopt.h string.h strings.h regex.h])
AC_CHECK_HEADERS([uthash.h uthash/uthash.h], HEADER_UTHASH="yes")

# Check functions
AC_CHECK_FUNC(log2, AC_DEFINE(HAVE_FUNC_LOG2, 1,
       [Define to 1 if you have the function log2]))

AC_SUBST([AM_CPPFLAGS])
AC_CONFIG_FILES([
   Makefile \
   src/Makefile \
   src/measures/Makefile \
   src/input/Makefile \
   src/output/Makefile \
   python/Makefile \
   tests/Makefile \
   doc/Makefile \
   examples/Makefile \
   examples/alexa/Makefile \
   examples/reuters/Makefile \
])
AC_OUTPUT

echo
echo " .Oo Optional packages:"
echo "     Support for reading archives (--with-libarchive):       $HAVE_LIBARCHIVE"
echo "     Support for multi-processing (--with-openmp):           $HAVE_OPENMP"
echo "     Support for POSIX threads and locks (--with-pthreads):  $HAVE_PTHREADS"
echo " .Oo Optional features:"
echo "     POSIX read-write lock (--enable-prwlock):               $ENABLE_PRWLOCK"
echo "     MD5 as alternative hash (--enable-md5hash):             $ENABLE_MD5HASH"
echo

