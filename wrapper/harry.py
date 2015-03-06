# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.  This program is distributed without any
# warranty. See the GNU General Public License for more details.

import subprocess as sp
import shlex
import urllib
import struct
import numpy as np


def harry(strs, opts=""):
    """
    Wrapper function for the tool Harry
    :param strs: List of strings
    :return: Similarity matrix
    """

    # Escape special chars in strings
    strs = map(urllib.quote, strs)
    # Assemble input data
    stdin = '\n'.join(strs)

    #  Input: "-"  Read strings from standard input
    # Output: "="  Write raw floats to standard output
    cmd = "harry %s --decode_str - =" % opts
    args = shlex.split(cmd)
    p = sp.Popen(args, stdout=sp.PIPE, stdin=sp.PIPE, stderr=None)
    (stdout, stderr) = p.communicate(input=stdin)

    # Unpack dimensions of matrix
    xdim, ydim = struct.unpack("II", stdout[0:8])

    # Generate matrix form buffer and reshape it
    mat = np.frombuffer(stdout[8:], dtype=np.float32)
    return mat.reshape(ydim, xdim)


import os
import time
test_file = "../config.sub"

# Command-line version
t = time.time()
os.system("harry %s /dev/null" % test_file)
print "Writing to a file: %fs" % (time.time() - t)

t = time.time()
data = open(test_file).read().splitlines()
mat = harry(data, opts="")
print "Writing to Python: %fs" % (time.time() - t)
