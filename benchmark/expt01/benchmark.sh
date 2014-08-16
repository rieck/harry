#!/bin/bash

datasets="enron hids nids reuters sentences sprot"
measures="levenshtein compression hamming"
nthreads="1 2 3 4 5 6 7 8"
runs="1 2 3"

export TIMEFORMAT="%R %U %S" 

for r in $runs ; do
    for d in $datasets ; do
        for m in $measures ; do
            for n in $nthreads ; do
                printf "$d $m $n $r " 1>&2
                time ../../src/harry -x 000:100 -y 100:200 -n $n -m $m -c ../$d.cfg \
                     ../$d.t*.gz /dev/null 
            done
        done
    done
done

