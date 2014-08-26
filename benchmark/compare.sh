#!/bin/sh

DATASETS="arts.txt.gz"
DATADIR="/Users/rieck/Work/data/harry/data"
MEASURES="levenshtein jarowinkler hamming"
RUNTIME=5

for DATA in $DATASETS ; do
    for MEASURE in $MEASURES ; do
    	printf "pylevenshtein/$MEASURE "
    	python pylevenshtein.py $DATADIR/$DATA $MEASURE $RUNTIME
    	printf "harry "
    	../src/harry -m $MEASURE -d '' -n 1 \
        	     --benchmark $RUNTIME $DATADIR/$DATA /dev/null
    done
done
