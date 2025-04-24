#!/bin/bash

PROBLEMS=(
    "small_files/"
    "mixed_files/"
    "big_files/"
)

# create a csv file for the sequential results
OUTPUT_FILE="results/sequential_results.csv"  # Log file
#initialize the csv file
echo "trial,problem,time" > $OUTPUT_FILE

for i in {1..5}; do 
    # clear the folders in case there are zip files remaining
    ./scripts/build_tests.sh r 
    for problem in "${PROBLEMS[@]}"; do
        # run the sequential program
        echo -n "$i,$problem," >> $OUTPUT_FILE
        ./minizseq.a -C 0 "$problem" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
    done
done
