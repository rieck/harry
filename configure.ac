# Harry - Similarity Measures for Strings
# Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)

AC_INIT([harry], [0.1], [konrad@mlsec.org])
AC_PREFIX_DEFAULT("/usr/local")

echo
echo "  Harry - Similarity Measures for Strings"
echo "  Copyright (c) 2013 Konrad Rieck (konrad@mlsec.org)"
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
AX_OPENMP([HAVE_OPENMP=yes],[HAVE_OPENMP=no])
if test "x$HAVE_OPENMP" = "xno" ; then
    AC_MSG_ERROR([The OpenMP support required for compulation.
    GCC implements OpenMP as of version >=4.2.
    ])
fi
CFLAGS="$CFLAGS $OPENMP_CFLAGS"

# Check headers
AC_CHECK_HEADERS([getopt.h string.h strings.h])

AC_SUBST([AM_CPPFLAGS])
AC_CONFIG_FILES([ 
   Makefile \
   src/Makefile \
   src/modules/Makefile \
   tests/Makefile \
   doc/Makefile \
])
AC_OUTPUT
