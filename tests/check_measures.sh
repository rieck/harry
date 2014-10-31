#!/bin/sh
# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
# --
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.  This program is distributed without any
# warranty. See the GNU General Public License for more details.
# --
# Simple test comparing precomputed output for all similarity measures
# against compiled version of Harry
#

# Check for directories
test -z "$TMPDIR" && TMPDIR="/tmp"
test -z "$BUILDDIR" && BUILDDIR=".."
test -z "$SRCDIR" && SRCDIR=".."

DATA=$SRCDIR/tests/strings.txt
CHECK=$SRCDIR/tests/check_measures.txt
HARRY=$BUILDDIR/src/harry
OUTPUT=$TMPDIR/harry-$$.txt


# Repeat tests five times to check for non-determinism
for I in 1 2 3 4 5 ; do
    rm -f $OUTPUT
    echo "Checking measure; Run $I"

    # Loop over similarity measures
    for MEASURE in dist_bag dist_compression dist_damerau dist_hamming \
             dist_jaro dist_jarowinkler dist_kernel dist_lee \
             dist_levenshtein dist_osa kern_distance kern_spectrum \
             kern_subsequence kern_wdegree sim_braun sim_dice sim_jaccard \
             sim_kulczynski sim_otsuka sim_simpson sim_sokal ; do

	echo $MEASURE >> $OUTPUT
        $HARRY -p 4 -m $MEASURE $DATA - | grep -v '#' >> $OUTPUT
    done

    # Save output
    cp $OUTPUT /tmp/check_measures.txt

    # Diff output and precomputed data
    diff $CHECK $OUTPUT
    RET=$?

    # Exit loop if output doesn't match
    test "$RET" != "0" && exit $RET
done

# Clean up and exit
rm -f $OUTPUT
exit $RET
