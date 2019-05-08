#!/usr/bin/env python2
# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2014 Konrad Rieck (konrad@mlsec.org)
# --
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.  This program is distributed without any
# warranty. See the GNU General Public License for more details.

import zlib

tests = [
    ("", "abc"),
    ("abc", ""),
    ("abc", "abc"),
    ("dslgjasldjfkasdjlkf", "dslkfjasldkf"),
    ("kasjhdgkjad", "kasjhdgkjad"),
    ("fkjhskljfhalsdkfhalksjdfhsdf", "djfh"),    
    ("fkjhskljfhalsdkfhalksjdfhsdf", ""),
    ("", "fkjhskljfhalsdkfhalksjdfhsdf"),
    ("6s6sd7as6d", "7sad8asd76"),
    ("aaaaaaaaaa", "bbbbbbbbb"),
]

for (x,y) in tests:
    xl = float(len(zlib.compress(x, 9)))
    yl = float(len(zlib.compress(y, 9)))
    xyl = float(len(zlib.compress(x + y, 9)))

    d = (xyl - min(xl, yl)) / max(xl, yl);
    print '    {"%s", "%s", %f},' % (x,y,d)
