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
# Simple test for one input, two inputs and ranges.
#

# Check for directories
test -z "$TMPDIR" && TMPDIR="/tmp"
test -z "$BUILDDIR" && BUILDDIR=".."
test -z "$SRCDIR" && SRCDIR=".."

DATA=$SRCDIR/tests/strings.txt
HARRY=$BUILDDIR/src/harry
OUTPUT1=$TMPDIR/harry1-$$.txt
OUTPUT2=$TMPDIR/harry2-$$.txt
TMPFILE=$TMPDIR/harry3-$$.txt
rm -f $OUTPUT1 $OUTPUT2 $TMPFILE

for i in 1 2 ; do
   case $i in
   1) 
      # Check one and two inputs 
      harry $DATA $OUTPUT1      
      harry $DATA $DATA $OUTPUT2
      ;;
   2) 
      # Check one input and ranges
      harry $DATA $OUTPUT1        
      cat $DATA $DATA $TMPFILE
      harry -x :8 -y 8: $TMPFILE $OUTPUT2
      ;;
   3)
      # Check "invalid ranges"
      harry $DATA $OUTPUT1      
      harry -x 3:5 -y 2:4 $DATA $DATA $OUTPUT2
      ;;
   esac

   # Check for identical output
   diff $OUTPUT1 $OUTPUT2
   if [ $? != 0 ] ; then
       exit 1
   fi
done

# Clean up and exit
rm -f $OUTPUT1 $OUTPUT2 $TMPFILE
exit 0
