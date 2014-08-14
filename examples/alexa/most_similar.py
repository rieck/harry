import sys
from numpy import *

# Filename for distance matrix
fname = sys.argv[1]

# Load top alexa websites
N = array(open('alexa1000.txt').read().splitlines())

# Load and expand distance matrix
D = genfromtxt(fname, delimiter=',')
D[isnan(D)] = 0
D = D.T + D

# Sort rows of distance matrix
j = argsort(D)

# Print 3 most similar names for first 5 sites
for i in range(5):
    print '%s:\t%s' % (N[i], N[j[i, 1:4]])

