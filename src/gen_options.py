#!/usr/bin/env python
import sys

options = []

# Read data
for line in open(sys.argv[1]).readlines():
    if line.startswith('#'):
        continue
    tok = line.strip().split(';')
    options.append(tok)

# Prepare long options
longopts = 'static struct option longopts[] = {\n'
for opt in options:
    if len(opt[0]) == 0:
        continue

    # Short options
    if opt[1].isdigit and len(opt[1]) > 2:
        short = '%d' % int(opt[1])
    else:
        short = "'%s'" % opt[1]

    tuple = (opt[0], 1 if len(opt[2]) > 0 else 0, short)
    longopts += '    {"%s", %d, NULL, %s},\n' % tuple
longopts += '    {NULL, 0, NULL, 0}\n};\n'

# Prepare short options
shortopts = '#define OPTSTRING "'
for opt in options:
    if len(opt[0]) == 0:
        continue
    if opt[1].isdigit and len(opt[1]) > 2:
        continue
    tuple = (opt[1], ":" if len(opt[2]) > 0 else "")
    shortopts += '%s%s' % tuple
shortopts += '"\n'

# Prepare usage
space = 11 * ' '
usage = 'printf("Usage: harry [options] <input> [<input>] <output>\\n"\n'
for opt in options:
    # Headings
    if len(opt[0]) == 0:
        usage += '%s"\\n%s:\\n"\n' % (space, opt[4])
        continue

    # Short option
    if opt[1].isdigit and len(opt[1]) > 2:
        short = '   '
    else:
        short = '-%s,' % opt[1]

    param = '<%s>' % opt[2] if len(opt[2]) > 0 else ''
    tuple = (space, short, opt[0], param)
    line = '%s"  %s  --%s %s' % tuple

    # Align descriptions
    line += (45 - len(line)) * ' '
    usage += line + '%s\\n"\n' % opt[4]
usage += '%s"\\n");\n' % space

# Replace
f = open(sys.argv[2], 'w')
for line in open(sys.argv[2] + '.in'):
    line = line.replace('%LONGOPTS%', longopts)
    line = line.replace('%SHORTOPTS%', shortopts)
    line = line.replace('%USAGE%', usage)
    f.write(line)
f.close()
