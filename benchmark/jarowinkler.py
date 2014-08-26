#!/usr/bin/env python
# Simple code for benchmarking python-Levenshtein 0.11.2

import sys
import gzip
import random
import Levenshtein
import time

# Load data in input mode 'line' 
data = gzip.open(sys.argv[1]).read().splitlines()
runtime = int(sys.argv[2])

start = time.time()
counter = 0

while time.time() - start < runtime:
    # Compute Levenshtein distance for random pair
    i = random.randint(0, len(data) - 1)
    j = random.randint(0, len(data) - 1)
    Levenshtein.jaro_winkler(data[i], data[j])
    counter += 1
    
print '%d comparisons; %d seconds; %d threads;' % (counter, runtime, 1)
    
