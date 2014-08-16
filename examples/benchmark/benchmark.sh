#!/bin/bash

DATASETS="enron hids nids reuters sentences sprot"
MEASURES="levenshtein"
THREADS="1 2 3 4 5 6 7 8"
RUNS="1 2 3"
RANGE="-x :100"
LOGFILE=`hostname -s`-`date "+%Y%m%d%H%M"`.log

export TIMEFORMAT="%R %U %S" 

for D in $DATASETS ; do
    for M in $MEASURES ; do
        for T in $THREADS ; do
            for R in $RUNS ; do
                printf "$D $M $T $R " 1>&2 >> $LOGFILE
                time ../../src/harry -n $T -m $M -o null $range \
                     -c $D.cfg $D.t*.gz /dev/null 1>&2 >> $LOGFILE
            done
        done
    done
done

