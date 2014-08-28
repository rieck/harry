#!/usr/bin/env python
# Simple code for benchmarking python-Levenshtein 0.11.2

import sys
import random
import Levenshtein
import time

# Load data in input mode 'line' 
data = open(sys.argv[1]).read().splitlines()
measure = sys.argv[2]

start = time.time()
counter = 0

for i in range(len(data)):
    for j in range(i, len(data)):
        if measure == 'levenshtein':
            Levenshtein.distance(data[i], data[j])
        elif measure == 'jarowinkler':
            Levenshtein.jaro_winkler(data[i], data[j])
        elif measure == 'hamming':
            Levenshtein.hamming(data[i], data[j])
        else:
            raise "Unknown measure"
    
