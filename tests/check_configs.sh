#!/bin/sh
#
# Simple test comparing precomputed output for different configurations
# against compiled version of Harry
# 

DATA=$SRCDIR/tests/data1.txt
CHECK=$SRCDIR/tests/check_configs.txt
HARRY=$BUILDDIR/src/harry

if test -z "$TMPDIR" ; then
   TMPDIR="/tmp"
fi
OUTPUT=$TMPDIR/harry-$$.txt
rm -f $OUTPUT 

for C in config1.cfg config2.cfg config3.cfg ; do
    $HARRY -c $SRCDIR/tests/$C $DATA - | grep -v -E '^#' >> $OUTPUT
done

cp $OUTPUT /tmp/check_configs.txt
diff $OUTPUT $CHECK
RET=$?
rm -f $OUTPUT
exit $RET
