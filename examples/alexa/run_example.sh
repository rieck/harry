#!/bin/sh

if ! which harry > /dev/null ; then
    echo "Could not find harry in path"
    exit
fi

# Compute Levenshtein distance
harry -d '' -m levenshtein alex1000.txt dist_levenshtein.txt

# Print most similar sites
python most_similar.py dist_levenshtein.txt

# Compute Jaro-Winkler distance
harry -d '' -m jarowinkler alex1000.txt dist_jarowinkler.txt

# Print most similar sites
python most_similar.py dist_jarowinkler.txt


