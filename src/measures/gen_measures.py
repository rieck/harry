#!/usr/bin/env python
import sys

measures = set()
modules = set()
aliases = {}
description = {}

# Read data
for line in open(sys.argv[1]).readlines():
    if line.startswith('#'):
        continue
    tok = line.strip().split(':')
    ms = tok[0].split(',')

    measures.add(ms[0])
    aliases[ms[0]] = ms[1:]
    modules.add(tok[1])
    description[ms[0]] = tok[2]
    
# Prepare includes
includes = ""
for m in sorted(modules):
    includes += '#include "%s.h"\n' % m

# Prepare interfaces
interfaces = 'measure_t func[] = {\n'
for m in sorted(measures):
    interfaces += '    {"%s", %s_config, %s_compare},\n' % (m,m,m)
    for a in aliases[m]:
        interfaces += '    {"%s", %s_config, %s_compare},\n' % (a,m,m)
interfaces += '    {NULL}\n};'

# Prepare list
list = '    fprintf(f,\n'
for m in sorted(measures):
    s = m
    #for a in aliases[m]:
    #    s += ', %s' % a
    list += '           "    %-20s %s\\n"\n' % (s, description[m])
list += '    );\n'

# Replace 
f = open(sys.argv[2], 'w')
for line in open(sys.argv[2] + '.in'):
    line = line.replace('%INCLUDES%', includes)
    line = line.replace('%INTERFACES%', interfaces)
    line = line.replace('%LIST%', list)
    f.write(line)
f.close()   
 
