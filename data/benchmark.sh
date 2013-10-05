#!/bin/bash

datasets="enron hids nids reuters sentences sprot" # arts
measures="hamming"
gcache="0 1"
nthreads="1 2 4 8"
runs="1 2 3"

export TIMEFORMAT="%R %U %S" 

for d in $datasets ; do
    for m in $measures ; do
        for n in $nthreads ; do
            for g in $gcache ; do
                for r in $runs ; do
                    printf "$d $m $n $g $r "
                    /usr/bin/env time \
                        ../src/harry -n $n -g $g -m $m -c $d.cfg \
                        $d.t*.gz /dev/null
                        exit
                done
            done
        done
    done
done

