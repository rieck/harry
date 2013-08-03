
Harry - Similarity Measures for Strings
==

Description
-- 

Harry is a small tool for comparing strings. The tool supports several
common distance and kernel functions for strings as well as some excotic
similarity measures.  The focus of Harry lies on implicit similarity
measures, that is, comparison functions that do not give rise to an explicit
vector space, such as the Levenshtein distance.  Harry complements the tool
Sally that embeds strings in vector spaces, where explicit similarity
measures can be computed, such as bag-of-word kernels.

Dependencies
--

    >= libconfig-1.4

Corresponding packages

+ Debian and Ubuntu Linux: `libconfig9-dev`  
+ MacOS X with Homebrew:   `libconfig`  
+ MacOS X with MacPorts:   `libconfig-hr`

The source code of the libraries is available here:

+ libconfig   <http://www.hyperrealm.com/libconfig/>

Compilation & Installation
--

From GIT repository first run

    ./bootstrap

From tarball run

    ./configure [options]
    make
    make check
    make install

Options for configure

    --prefix=PATH           Set directory prefix for installation

By default Harry is installed into /usr/local. If you prefer a different
location, use this option to select an installation directory.

Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
