#!/bin/sh
#
# Simple test comparing precomputed output for all similarity measures
# against compiled version of Harry
# 

DATA=$SRCDIR/tests/data1.txt
CHECK=$SRCDIR/tests/check_measures.txt
HARRY=$BUILDDIR/src/harry

if test -z "$TMPDIR" ; then
   TMPDIR="/tmp"
fi
OUTPUT=$TMPDIR/harry-$$.txt
rm -f $OUTPUT

for M in dist_bag dist_compression dist_damerau dist_hamming dist_jaro \
         dist_jarowinkler dist_kernel dist_lee dist_levenshtein dist_osa \
         kern_distance kern_spectrum kern_subsequence kern_wdegree \
         sim_braun sim_dice sim_jaccard sim_kulczynski sim_otsuka \
         sim_simpson sim_sokal ; do

    $HARRY -m $M $DATA - | grep -v '#' >> $OUTPUT
done

cp $OUTPUT /tmp/check_measures.txt
diff $OUTPUT $CHECK
RET=$?
rm -f $OUTPUT
exit $RET
