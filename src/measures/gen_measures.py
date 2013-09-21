#!/usr/bin/env python
import sys

measures = set()
modules = set()

# Read data
for line in open(sys.argv[1]).readlines():
    if line.startswith('#'):
        continue
    tok = line.strip().split(':')
    measures.add(tok[0])
    modules.add(tok[1])
    
# Prepare includes
includes = ""
for m in sorted(modules):
    includes += '#include "%s.h"\n' % m

# Prepare interfaces
interfaces = "func_t func[] = {\n"
for m in sorted(measures):
    interfaces += '    {"%s", %s_config, %s_compare},\n' % (m,m,m)
interfaces += "    {NULL}\n};"

# Replace 
f = open(sys.argv[2], 'w')
for line in open(sys.argv[2] + '.in'):
    line = line.replace("%INCLUDES%", includes)
    line = line.replace("%INTERFACES%", interfaces) 
    f.write(line)
f.close()   
 