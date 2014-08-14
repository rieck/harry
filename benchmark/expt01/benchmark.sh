#!/bin/bash

datasets="enron hids nids reuters sentences sprot arts"
measures="hamming"
nthreads="1 2 3 4 5 6 7 9 10 12 "
runs="1 2 3"

export TIMEFORMAT="%R %U %S" 

for d in $datasets ; do
    for m in $measures ; do
        for n in $nthreads ; do
            for r in $runs ; do
                printf "$d $m $n $r " 1>&2
                time ../../src/harry -n $n -m $m -c ../$d.cfg \
                     ../$d.t*.gz /dev/null 
            done
        done
    done
done

