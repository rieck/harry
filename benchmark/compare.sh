#!/bin/sh

DATASETS="arts sprot tweets webfp http"
DATADIR="/data/projects/harry/data"
RUNS="1 2 3 4 5"

export OMP_NUM_THREADS=1
for R in $RUNS ; do
    for DATA in $DATASETS ; do
        gunzip -c $DATADIR/$DATA.txt.gz | shuf | head -100 > data.txt

        printf "shogun $DATA subsequence "
        time -o /tmp/log -f "%e %U %S" python pyshogun.py data.txt
        cat /tmp/log
        printf "harry $DATA subsequence "
        time -o /tmp/log -f  "%e %U %S" ../src/harry -d '' -m subsequence -n 1 data.txt /dev/null 
        cat /tmp/log        

        #printf "pylevenshtein $DATA jarowinkler "
        #time -o /tmp/log -f "%e %U %S" python pylevenshtein.py data.txt jarowinkler
        #cat /tmp/log        
        #printf "harry $DATA jarowinkler "
        #time -o /tmp/log -f "%e %U %S" ../src/harry -d '' -m jarowinkler -n 1 data.txt /dev/null
        #cat /tmp/log        

        printf "complearn $DATA compression "
        time -o /tmp/log -f "%e %U %S" ncd -c zlib -t data.txt data.txt > /dev/null
        cat /tmp/log
        printf "harry $DATA compression "
        time -o /tmp/log -f "%e %U %S" ../src/harry -d '' -m compression -n 1 data.txt /dev/null
        cat /tmp/log

        printf "pylevenshtein $DATA levenshtein "
        time -o /tmp/log -f "%e %U %S" python pylevenshtein.py data.txt levenshtein
        cat /tmp/log
        printf "harry $DATA levenshtein "
        time -o /tmp/log -f "%e %U %S" ../src/harry -d '' -m levenshtein -n 1 data.txt /dev/null
        cat /tmp/log

        rm data.txt        
    done
done
