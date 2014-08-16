#!/bin/bash

DATASETS="enron hids nids reuters sentences sprot"
MEASURES="levenshtein"
THREADS="4 1 2 3 5 6 7 8"
RUNS="1 2 3"
RANGE=""

export TIMEFORMAT="%R %U %S" 

for D in $DATASETS ; do
    for M in $MEASURES ; do
        for T in $THREADS ; do
            for R in $RUNS ; do
                printf "$D $M $T $R " 1>&2 
                time ../../src/harry -n $T -m $M -o null $range \
                     -c $D.cfg $D.t*.gz /dev/null 1>&2
            done
        done
    done
done

