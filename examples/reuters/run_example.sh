#!/bin/sh

if ! which harry > /dev/null ; then
    echo "Could not find harry in PATH"
    exit
fi

if ! which svm-train > /dev/null ; then
    echo "Could not find svm-train of libsvm in PATH"
    exit
fi

harry -c harry.cfg -v -m kern_spectrum reuters.zip kern1.txt
harry -c harry.cfg -v -m kern_subsequence reuters.zip kern2.txt
harry -c harry.cfg -v -m kern_distance reuters.zip kern3.txt

svm-train -q -v 5 -t 4 kern1.txt
svm-train -q -v 5 -t 4 kern2.txt
svm-train -q -v 5 -t 4 kern3.txt
