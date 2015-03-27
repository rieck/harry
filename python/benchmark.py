# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Very simple benchmark to compare the module "python-Levenshtein" against
# the module "harry". Have fun.

import time
import random
import numpy as np
import matplotlib.pyplot as plt
import warnings

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


def test_jelly(strings):
    """ Compute Levenshtein distance; return time (jellyfish) """
    t = time.time()
    for i in range(len(strings)):
        for j in range(i + 1, len(strings)):
            jellyfish.levenshtein_distance(strings[i], strings[j])
    return time.time() - t


def test_harry(strings):
    """ Compute Levenshtein distance; return time (Harry) """
    t = time.time()
    harry.compare(strings, num_threads=1)
    return time.time() - t

# Prepare experiment
runs = 20
scale = np.logspace(0, np.log10(150), 20)
t_harry = np.zeros(len(scale))
t_pylev = np.zeros(len(scale))
t_jelly = np.zeros(len(scale))

for r in range(runs):
    strs = gen_strs(np.max(scale))

    try:
        import harry

        for (i, s) in enumerate(scale):
            t_harry[i] += test_harry(strs[0:int(s)])
    except ImportError:
        warnings.warn("harry is not available")

    try:
        import Levenshtein

        for (i, s) in enumerate(scale):
            t_pylev[i] += test_pylev(strs[0:int(s)])
    except ImportError:
        warnings.warn("python-Levenshtein is not available")

    try:
        import jellyfish

        for (i, s) in enumerate(scale):
            t_jelly[i] += test_jelly(strs[0:int(s)])
    except ImportError:
        warnings.warn("jellyfish is not available")

# Normalize run-time by runs
t_pylev /= runs
t_harry /= runs
t_jelly /= runs

# Matplot stuff
plt.figure(figsize=(6, 3))

plt.plot(scale, t_pylev, label="python-Levenshtein")
plt.plot(scale, t_harry, label="Python module of Harry")
plt.plot(scale, t_jelly, label="jellyfish")

plt.xlabel('Number of strings to compare')
plt.ylabel('Run-time (s)')
plt.xlim(np.min(scale), np.max(scale))
plt.ylim([1e-4, 0.5])
plt.legend(loc=2)
plt.tight_layout()
plt.savefig('benchmark1.pdf')

plt.yscale("log")
plt.legend(loc=0)
plt.savefig('benchmark2.pdf')

