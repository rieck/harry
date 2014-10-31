#!/bin/sh
#
# Simple test comparing precomputed output for different options
# against compiled version of Harry
# 

DATA=$SRCDIR/tests/data1.txt
CHECK=$SRCDIR/tests/check_options.txt
HARRY=$BUILDDIR/src/harry

if test -z "$TMPDIR" ; then
   TMPDIR="/tmp"
fi
OUTPUT=$TMPDIR/harry-$$.txt
rm -f $OUTPUT 

for O in "-m hamming" "-x 2:" "-y 1:-1" "-d%20%0a%0d" "-s 3:1" \
         "-d%20 --soundex" "--reverse_str" "--save_indices" \
         "--save_labels" "--save_sources" "--decode_str" ; do
    $HARRY $O $DATA - | grep -v -E '^#' >> $OUTPUT
done

cp $OUTPUT /tmp/check_options.txt
diff $OUTPUT $CHECK
RET=$?
rm -f $OUTPUT
exit $RET
