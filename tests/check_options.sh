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
# Simple test comparing precomputed output for different options
# against compiled version of Harry
#

# Check for directories
test -z "$TMPDIR" && TMPDIR="/tmp"
test -z "$BUILDDIR" && BUILDDIR=".."
test -z "$SRCDIR" && SRCDIR=".."

DATA=$SRCDIR/tests/strings.txt
CHECK=$SRCDIR/tests/check_options.txt
HARRY=$BUILDDIR/src/harry
OUTPUT=$TMPDIR/harry-$$.txt
rm -f $OUTPUT

# Loop over some random options
for OPTION in "-m dist_hamming" "-m kern_spectrum" "-x 2:" "-y 1:-1" \
              "-y :" "-x -1:" "-s 3:1" "-d%20%0a%0d" "-d%20abcd" \
              "-d%20 --soundex" "--reverse_str" "--decode_str" \
              "--save_indices" "--save_labels" "--save_sources" ; do

    echo "$OPTION" >> $OUTPUT
    $HARRY -p 4 $OPTION $DATA - | grep -v -E '^#' >> $OUTPUT
done

# Save output
cp $OUTPUT /tmp/check_options.txt

# Diff output and precomputed data
diff $CHECK $OUTPUT
RET=$?

# Clean up and exit
rm -f $OUTPUT
exit $RET
