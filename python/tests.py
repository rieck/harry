# Harry - A Tool for Measuring String Similarity
# Copyright (C) 2013-2015 Konrad Rieck (konrad@mlsec.org)
# --
# Just a collection of test cases which mainly check the interfacing
# with the command-line tool

import harry

x = ["this is test", "this is not a test", "is this a test", "test it"]

# Default options
m = harry.compare(x)
m = harry.compare(x, x)

# Other measures
m = harry.compare(x, measure="dist_damerau")
m = harry.compare(x, measure="dist_compression")
m = harry.compare(x, measure="kern_wdegree")

# Granularity changes
print harry.compare(x, granularity="bytes")
print harry.compare(x, granularity="bits")
print harry.compare(x, granularity="tokens")
