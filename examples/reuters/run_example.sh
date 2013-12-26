#!/bin/sh

if ! which harry > /dev/null ; then
    echo "Could not find harry in PATH"
    exit
fi

if ! which svm-train > /dev/null ; then
    echo "Could not find svm-train of libsvm in PATH"
    exit
fi

for kern in kern_spectrum kern_subsequence kern_distance ; do 
    # Compute the kernel
    harry -v -c harry.cfg -m $kern reuters.zip $kern.txt
    # Train SVM (cross-validation)
    svm-train -q -v 5 -t 4 $kern.txt
done
