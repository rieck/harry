# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.  This program is distributed without any
# warranty. See the GNU General Public License for more details.
# --
# Very simple benchmark to compare the module "python-Levenshtein" against
# the module "harry". As the python module of Harry invokes the tool on each
# call, there is only performance benefit if the number of strings
# to compare reaches a certain limit.

import time
import random
import Levenshtein
import numpy as np
import matplotlib.pyplot as plt
import harry


def rand_str(len):
    """ Generate random string """
    alph = "abcdefghijklmnopqrstuwxvy"
    s = ""
    for i in range(len):
        s += random.choice(alph)
    return s


def gen_strs(num):
    """ Generate a set of random strings, each of length 50 """
    s = []
    for i in np.arange(num):
        s.append(rand_str(50))
    return s


def test_pylev(strs):
    """ Compute Levenshtein distance; return time (python-Levensthein) """
    t = time.time()
    for i in range(len(strs)):
        for j in range(i, len(strs)):
            Levenshtein.distance(strs[i], strs[j])
    return time.time() - t


def test_harry(strs):
    """ Compute Levenshtein distance; return time (Harry) """
    t = time.time()
    m = harry.compare(strs, opts="-n 1")
    return time.time() - t


# Prepare experiment
scale = np.logspace(0, np.log10(200), 20)
t_harry = np.zeros(len(scale))
t_pylev = np.zeros(len(scale))
runs = 10

# Compute ratio between python-Levenshtein and Harry
for r in range(runs):
    strs = gen_strs(np.max(scale))
    for (i, s) in enumerate(scale):
        t_pylev[i] += test_pylev(strs[0:int(s)])
        t_harry[i] += test_harry(strs[0:int(s)])

# Normalize ratios
t_pylev /= float(runs)
t_harry /= float(runs)

# Matplot stuff
plt.figure(figsize=(6, 3))
plt.plot(scale, t_pylev)
plt.plot(scale, t_harry)
plt.xlabel('Number of strings')
plt.ylabel('Run-time (s)')
plt.xlim(np.min(scale), np.max(scale))
plt.legend(['python-Levenshtein', 'python module of Harry'], loc=2)
plt.tight_layout()
plt.savefig('benchmark1.pdf')
