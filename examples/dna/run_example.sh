#!/bin/sh

if ! which harry > /dev/null ; then
    echo "Could not find harry in PATH"
    exit
fi

if ! which svm-train > /dev/null ; then
    echo "Could not find svm-train in PATH"
    exit
fi

# Compute the weighted-degree kernel
harry -v -c harry.cfg -m wdegree dna_hg16.txt kern_wdegree.txt

# Compute the Spectrum kernel
harry -v -c harry.cfg -m spectrum dna_hg16.txt kern_spectrum.txt

# Train SVM (cross-validation)
svm-train -v 5 -t 4 kern_wdegree.txt
svm-train -v 5 -t 4 kern_spectrum.txt
