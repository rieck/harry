# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Just a collection of test cases which mainly check the interfacing
# with the command-line tool

import numpy as np
import sys
import harry
import random

a = " abcdefghijklmnopqrstuvwxyz.,"
x = [''.join(random.choice(a) for _ in range(100)) for _ in range(100)]

print "Testing options:",
m1 = harry.compare(x)
m2 = harry.compare(x, x)
if np.linalg.norm(m1 - m2) < 1e-9:
    print "Ok"
else:
    print "Failed"
    sys.exit(1)

print "Testing kernels:",
# Check kernel matrices for negative eigenvalues
for name in ["wdegree", "subsequence", "spectrum"]:
    k = harry.compare(x, measure="kern_" + name)
    e = np.min(np.real(np.linalg.eig(k)[0]))
    print ".",
    if np.abs(e) > 1e-9:
        print "Failed"
        sys.exit(1)
print "Ok"

print "Testing distances:", 
# Check distances matrices for metric property
for name in ["hamming", "levenshtein", "bag", "lee"]:
    d = harry.compare(x, measure="dist_" + name)
    
    # This might not be super efficient ;)
    perm = np.random.permutation(len(x))[:30]
    for i in perm:
        for j in perm:
            for k in perm:
                m = d[i,j] + d[j,k] - d[i,k]
                if m < 0:
                    print "Failed"
                    sys.exit(1)
    print ".", 
print "Ok"
