import sys
from numpy import *

# Filename for distance matrix
fname = sys.argv[1]

# Load top alexa websites
N = array(open('alex1000.txt').read().splitlines())

# Load and expand distance matrix
D = genfromtxt(fname, delimiter=',')
D[isnan(D)] = 0
D = D.T + D

# Print 3 most similar names for first 5 sites
print '\n%s' % fname
for i in range(5):
    print '%s:\t' % N[i],
    j = argsort(D[i,:])
    print N[j[1:4]]

