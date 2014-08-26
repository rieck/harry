#!/usr/bin/env python
# Simple code for benchmarking python-Levenshtein 0.11.2

import sys
import gzip
import random
import Levenshtein
import time

# Load data in input mode 'line' 
data = gzip.open(sys.argv[1]).read().splitlines()
runtime = int(sys.argv[3])
measure = sys.argv[2]

start = time.time()
counter = 0

while time.time() - start < runtime:
    # Compute Levenshtein distance for random pair
    i = random.randint(0, len(data) - 1)
    j = random.randint(0, len(data) - 1)
    
    if measure == 'levenshtein':
        Levenshtein.distance(data[i], data[j])
    elif measure == 'jarowinkler':
        Levenshtein.jaro_winkler(data[i], data[j])
    elif measure == 'hamming':
        Levenshtein.hamming(data[i], data[j])
    else:
        raise "Unknown measure"
    counter += 1
    
print '%d comparisons; %d seconds; %d threads;' % (counter, runtime, 1)
    
