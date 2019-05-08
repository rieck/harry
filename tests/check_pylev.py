#!/usr/bin/env python2
# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Comparison with the Python-levensthein module (if available)

# Loading standard modules
try:
    import sys
    import os
    import random
    import Levenshtein as ls
    import numpy as np
except:
    print "Something is wrong. Skipping test"
    sys.exit(77)


def pylev_compare(x1, x2):
    """ Simple wrapper for Python levensthein module """
    m = np.zeros((len(x1), len(x2)))
    for i in range(len(x1)):
        for j in range(len(x2)):
            m[i,j] = ls.distance(x1[i], x2[j])
    return m

# Add path for Harry module
builddir = os.environ["BUILDDIR"]
sys.path.append(os.path.join(builddir, "python"))

# Load harry module
import harry

# Adapt path of tool (as it is not installed yet)
harry.__tool = os.path.join(builddir, "src", "harry")

x1 = ['this', 'is', 'a', 'test']
x2 = ['guess', 'what', 'this', 'is', 'not', 'a', 'test']

print "Testing Levenshtein implementation:",
m1 = harry.compare(x1, x2)
m2 = pylev_compare(x1, x2)

print
print m1
print m2

if np.linalg.norm(m1 - m2) < 1e-9:
    print "Ok"
else:
    print "Failed"
    sys.exit(1)
