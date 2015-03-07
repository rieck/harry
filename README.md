![Harry](harry.png) 

Harry - A Tool for Measuring String Similarity
==

Description
--

Harry is a small tool for comparing strings. The tool supports several
common distance and kernel functions for strings as well as some excotic
similarity measures.  The focus of Harry lies on implicit similarity
measures, that is, comparison functions that do not give rise to an explicit
vector space.  Examples of such similarity measures are the Levenshtein
distance, the Jaro-Winkler distance or the spectrum kernel.

During operation Harry loads a set of strings from input, computes the
specified similarity measure and writes a matrix of similarity values to
output.  The similarity measure can be computed based on the granulartiy of
characters as well as words contained in the strings.  The configuration of
this process, such as the input format, the similarity measure and the
output format, are specified in a configuration file and can be additionally
refined using command-line options.

Harry is implemented using OpenMP, such that the computation time for a
set of strings scales linear with the number of available CPU cores. 
Moreover, efficient implementations of several similarity measures,
effective caching of similarity values and low-overhead locking further
speedup the computation.

Harry complements the tool sally(1) that embeds strings in a vector space
and allows computing vectorial similarity measures, such as the cosine
distance and the bag-of-words kernel.

Similarity Measures
--

The following similarity measures are supported so by Harry

    dist_bag             Bag distance
    dist_compression     Normalized compression distance (NCD)
    dist_damerau         Damerau-Levenshtein distance
    dist_hamming         Hamming distance
    dist_jaro            Jaro distance
    dist_jarowinkler     Jaro-Winkler distance
    dist_kernel          Kernel-based distance
    dist_lee             Lee distance
    dist_levenshtein     Levenshtein distance
    dist_osa             Optimal string alignment (OSA) distance
    kern_distance        Distance substitution kernel (DSK)
    kern_spectrum        Spectrum kernel
    kern_subsequence     Subsequence kernel (SSK)
    kern_wdegree         Weighted-degree kernel (WDK)
    sim_braun            Braun-Blanquet coefficient
    sim_dice             Soerensen-Dice coefficient
    sim_jaccard          Jaccard coefficient
    sim_kulczynski       second Kulczynski coefficient
    sim_otsuka           Otsuka coefficient
    sim_simpson          Simpson coefficient
    sim_sokal            Sokal-Sneath coefficient

Dependencies
--

+   OpenMP >= 2.5 (need to be supported by the C compiler)
+   zlib >= 1.2.1, <http://www.zlib.net/>
+   libconfig >= 1.3.2, <http://www.hyperrealm.com/libconfig/>
+   libarchive >= 2.70, <http://libarchive.github.com/>

#### Debian & Ubuntu Linux

The following packages need to be installed for compiling Harry on Debian
and Ubuntu Linux

    gcc
    libz-dev
    libconfig8-dev
    libarchive-dev

For bootstrapping Harry from the GIT repository or manipulating the
automake/autoconf configuration, the following additional packages are
necessary.

    automake
    autoconf
    libtool

#### Mac OS X

For compiling Harry on Mac OS X a working installation of Xcode is needed.
Moreover, a C compiler supporting OpenMP is required (`clang` from Xcode
currently does not support OpenMP).  The following packages need to be
installed from Homebrew.

    gcc43 (or download from <http://hpc.sourceforge.net>)
    libconfig
    libarchive (from homebrew-alt)

#### OpenBSD

Due to the vague state of OpenBSD multi-threading, neither the default `gcc`
nor the packaged `gcc` seem to correctly support OpenMP.  To get Harry to
run you can only try to build gcc from scratch

    :(

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

    --enable-prwlock        Enable support for POSIX read-write locks

This feature enables read-write locks (rwlocks) from the POSIX thread
library.  The locks can accelerate the run-time performance on multi-core
systems.  However, these POSIX locks are not guaranteed to interplay with
OpenMP and thus may not work on all platforms.

    --enable-md5hash        Enable MD5 as alternative hash

Harry uses a hash function for mapping words to symbols. By default the very
efficient Murmur hash is used for this task.  In certain critical cases it
may be useful to use a cryptographic hash as MD5.

Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
