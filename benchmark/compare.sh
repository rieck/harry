#!/bin/sh

DATASETS="webfp tweets sprot"
DATADIR="/data/projects/harry/data"
MEASURES="levenshtein jarowinkler"
RUNTIME=5

export OMP_NUM_THREADS=1

for DATA in $DATASETS ; do
    gunzip -c $DATADIR/$DATA.txt.gz | head -200 > data.txt

    printf "pylevenshtein $DATA jarowinkler "
    time -f "%e %U %S" python pylevenshtein.py data.txt jarowinkler
    printf "harry $DATA jarowinkler "
    time -f "%e %U %S" ../src/harry -d '' -m jarowinkler -n 1 data.txt /dev/null
    
    printf "complearn $DATA compression "
    time -f "%e %U %S" ncd -c zlib -t data.txt data.txt > /dev/null
    printf "harry $DATA compression "
    time -f "%e %U %S" ../src/harry -d '' -m compression -n 1 data.txt /dev/null

    printf "pylevenshtein $DATA levenshtein "
    time -f "%e %U %S" python pylevenshtein.py data.txt levenshtein
    printf "harry $DATA levenshtein "
    time -f "%e %U %S" ../src/harry -d '' -m levenshtein -n 1 data.txt /dev/null

    
    rm data.txt        
done
