#!/usr/bin/env python
import sys

options = []

# Read data
for line in open(sys.argv[1]).readlines():
    if line.startswith('#'):
        continue
    tok = line.strip().split(';')
    options.append(tok)

# Prepare keyargs
keyargs = []
line = '        '
for opt in options:
    # No I/O options
    if opt[3] == 'io':
        continue

    if len(opt[0]) == 0:
        continue

    if len(line) + len(opt[0]) + 3 > 77:
        keyargs.append(line)
        line = '        '

    line += '"%s", ' % opt[0]

keyargs.append(line[:-2] + '\n')
keyargs = "keyargs = set([\n" + '\n'.join(keyargs) + "    ])"

# Conversion of types to Python types
conversion = {
    "file": "str", "chars": "str", "num": "int", "bool": "bool",
    "start:end": "str", "blocks:id": "str", "name": "str",
    "type": "str"
}

# Prepare usage
usage = ""
for opt in options:
    # No I/O options
    if opt[3] == 'io':
        continue

    if len(opt[0]) == 0:
        usage += "\n    %s (optional)\n" % opt[4].replace("options", "args")
        continue

    type = "bool" if len(opt[2]) == 0 else opt[2]
    type = conversion[type]
    line = "      %s (%s):" % (opt[0], type)
    usage += line + (30 - len(line)) * " " + "%s\n" % opt[4]
usage = usage[5:-1]

# Replace
f = open(sys.argv[2], 'w')
for line in open(sys.argv[2] + '.in'):
    line = line.replace('%KEYARGS%', keyargs)
    line = line.replace('%USAGE%', usage)
    line = line.replace('%BINDIR%', sys.argv[3])
    f.write(line)
f.close()
