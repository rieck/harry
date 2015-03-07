#!/bin/sh

if ! which harry > /dev/null ; then
    echo "Could not find harry in PATH"
    exit
fi

# Compute Levenshtein distance
harry -v -m levenshtein alexa1000.txt distances.txt

# Print most similar sites
python most_similar.py distances.txt


