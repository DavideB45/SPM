#!/bin/bash

PROBLEMS=(
    "small_files/"
    "mixed_files/"
    "big_files/"
)

THREADS=(2 4 8 10 12 14 16 18 20 22 24 28 30 32)

# create a csv file for the sequential results
OUTPUT_FILE="results/parallel_results.csv"  # Log file
#initialize the csv file
echo "trial,problem,threads,time" > $OUTPUT_FILE

for i in {1..7}; do 
    for thread in "${THREADS[@]}"; do
        # clear the folders in case there are zip files remaining
        ./scripts/build_tests.sh r 
        for problem in "${PROBLEMS[@]}"; do
            # run the sequential program
            echo -n "$i,$problem,$thread," >> $OUTPUT_FILE
            ./minizpar.a -C 0 -t $thread "$problem" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
        done
    done
done
