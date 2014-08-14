#!/bin/bash

datasets="enron hids nids reuters sentences sprot" # arts
measures="hamming levenshtein"
nthreads="1 2 3 4 5 6 7 8"
runs="1 2 3"

export TIMEFORMAT="%R %U %S" 

for r in $runs ; do
    for d in $datasets ; do
        for m in $measures ; do
            for n in $nthreads ; do
                printf "$d $m $n $r " 1>&2
                time ../../src/harry -n $n -m $m -c ../$d.cfg \
                     ../$d.t*.gz /dev/null 
            done
        done
    done
done

