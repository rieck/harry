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


def __run_harry(strs, opts):
    """
    Internal function to call the tool Harry
    :param strs: List of strings
    :return: Similarity matrix
    """

    # Decode special characters and prepare input
    strs = map(urllib.quote, strs)
    stdin = '\n'.join(strs)

    # Input: "-"  Read strings from standard input
    # Output: "="  Write raw matrix to standard output
    cmd = "harry %s --decode_str - =" % opts
    args = shlex.split(cmd)
    p = sp.Popen(args, stdout=sp.PIPE, stdin=sp.PIPE, stderr=None)
    stdout, _ = p.communicate(input=stdin)

    # Unpack dimensions of matrix
    xdim, ydim = struct.unpack("II", stdout[0:8])
    # Generate matrix form buffer and reshape it
    mat = np.frombuffer(stdout[8:], dtype=np.float32)
    return mat.reshape(xdim, ydim)


def compare(x, y=None, opts=""):
    """
    Compare strings and compute a similarity matrix.
    :param x: List of strings
    :return: Similarity matrix
    """

    # Fix incorrect usage
    x = x if type(x) is list else [x]

    if y:
        # Fix incorrect usage again
        y = y if type(y) is list else [y]

        # We merge the lists and use index ranges
        opts += " -x :%d -y %d:" % (len(x), len(x))
        x += y

    return __run_harry(x, opts)
