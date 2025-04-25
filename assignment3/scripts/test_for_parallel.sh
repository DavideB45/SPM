#!/bin/bash

PROBLEMS=(
    "small_files/"
)

OMP_SCHEDULING_FLAGS=(
    "static,1"
    "dynamic,1"
    "static,3"
    "dynamic,3"
    "static,8"
    "dynamic,8"
)
THREADS=(8 16 32)

# create a csv file for the sequential results
OUTPUT_FILE="results/for_parallel_results.csv"  # Log file
#initialize the csv file
echo "trial,scheduling,block,threads,time" > $OUTPUT_FILE

for i in {1..9}; do 
    for thread in "${THREADS[@]}"; do
        for sched in "${OMP_SCHEDULING_FLAGS[@]}"; do
            # clear the folders in case there are zip files remaining
            rm small_files/*.zip 
            # run the sequential program
            echo -n "$i,$sched,$thread," >> $OUTPUT_FILE
            OMP_SCHEDULE=$sched ./minizforpar.a -C 0 -t $thread "small_files/" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
        done
    done
done
