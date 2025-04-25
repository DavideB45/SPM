#!/bin/bash

PROBLEMS=(
    "small_files/"
    "big_files/"
)

OMP_AFFINITY_FLAGS=(
    "true"
    "false"
    "master"
    "spread"
    "close"
)
THREADS=(8)

# create a csv file for the sequential results
OUTPUT_FILE="results/affinity_results.csv"  # Log file
#initialize the csv file
echo "trial,affinity,problem,time" > $OUTPUT_FILE

for i in {1..9}; do 
    for aff in "${OMP_AFFINITY_FLAGS[@]}"; do
        # clear the folders in case there are zip files remaining
        rm small_files/*.zip
        rm big_files/*.zip
        for problem in "${PROBLEMS[@]}"; do
            # run the sequential program
            echo -n "$i,$aff,$problem," >> $OUTPUT_FILE
            OMP_PROC_BIND=$aff ./minizpar.a -C 0 -t 8 "$problem" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
        done
    done
done