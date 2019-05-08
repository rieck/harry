#!/usr/bin/env python2
# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Just a collection of test cases which mainly check the interfacing
# with the command-line tool

# Loading standard modules
try:
    import sys
    import os
    import random
    import numpy as np
except:
    print "Something is wrong. Skipping test"
    sys.exit(77)

# Add path for Harry module
builddir = os.environ["BUILDDIR"]
sys.path.append(os.path.join(builddir, "python"))

# Load harry module
import harry

# Adapt path of tool (as it is not installed yet)
harry.__tool = os.path.join(builddir, "src", "harry")

a = " abcdefghijklmnopqrstuvwxyz.,"
x = [''.join(random.choice(a) for _ in range(10)) for _ in range(50)]

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

    # Test fails if Eigenvalues are negative
    if e < -1e-6:
        print "Failed"
        sys.exit(1)
print "Ok"

print "Testing distances:",
# Check distances matrices for metric property
for name in ["hamming", "levenshtein", "bag", "lee"]:
    d = harry.compare(x, measure="dist_" + name)

    # This might not be super efficient ;)
    for i in range(len(x)):
        for j in range(i, len(x)):
            for k in range(len(x)):
                m = d[i,j] + d[j,k] - d[i,k]
                if m < 0:
                    print "Failed"
                    sys.exit(1)
    print ".",
print "Ok"
