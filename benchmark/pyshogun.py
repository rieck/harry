#!/usr/bin/env python
# Simple code for benchmarking shogun 3.2.0

import sys 

from shogun.Features import *
from shogun.Kernel import *
from modshogun import StringCharFeatures, RAWBYTE
from shogun.Kernel import StringSubsequenceKernel

# Load data in input mode 'line'
data = open(sys.argv[1]).read().splitlines()
data =filter(lambda x: len(x) > 0, data)

# Init kernel
features = StringCharFeatures(data, RAWBYTE)
sk = StringSubsequenceKernel(features, features, 3, 0.1)

# Limit kernel computation to 1 thread
sk.parallel.set_num_threads(1)

# Compute kernel matrix
K = sk.get_kernel_matrix()

