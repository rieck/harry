# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Very simple benchmark to compare the module "python-Levenshtein" against
# the module "harry". Have fun.

import time
import random

import Levenshtein
import numpy as np
import matplotlib.pyplot as plt

import harry


def rand_str(length):
    """ Generate random string """
    alph = " abcdefghijklmnopqrstuwxvyABCDEFGHIJKLMNOPQRSTUVWXYZ"
    s = ""
    for i in range(length):
        s += random.choice(alph)
    return s


def gen_strs(number):
    """ Generate a set of random strings, each of length 100 """
    s = []
    for i in np.arange(number):
        s.append(rand_str(100))
    return s


def test_pylev(strings):
    """ Compute Levenshtein distance; return time (python-Levensthein) """
    t = time.time()
    for i in range(len(strings)):
        for j in range(i + 1, len(strings)):
            Levenshtein.distance(strings[i], strings[j])
    return time.time() - t


def test_harry(strings):
    """ Compute Levenshtein distance; return time (Harry) """
    t = time.time()
    harry.compare(strings, num_threads=1)
    return time.time() - t


# Prepare experiment
scale = np.logspace(0, np.log10(150), 20)
t_harry = np.zeros(len(scale))
t_pylev = np.zeros(len(scale))
runs = 20

# Compute ratio between python-Levenshtein and Harry
for r in range(runs):
    strs = gen_strs(np.max(scale))
    for (i, s) in enumerate(scale):
        t_pylev[i] += test_pylev(strs[0:int(s)])
        t_harry[i] += test_harry(strs[0:int(s)])

# Normalize run-time by runs
t_pylev /= runs
t_harry /= runs

# Matplot stuff
plt.figure(figsize=(6, 3))

plt.plot(scale, t_pylev, label="python-Levenshtein")
plt.plot(scale, t_harry, label="python module of Harry")

plt.xlabel('Number of strings')
plt.ylabel('Run-time (s)')
plt.xlim(np.min(scale), np.max(scale))
plt.legend(loc=2)
plt.savefig('benchmark.pdf')
