#!/bin/bash

payload=1
BASE_SIZE=1000000
WORKERS=(1 3 7 15 31) # this are the numbers because the total is +1 for the emitter

# create a csv file for the sequential results
OUTPUT_FILE="results/naive_ff.csv"  # Log file
#initialize the csv file
echo "size,workers,time" > $OUTPUT_FILE


for workers in "${WORKERS[@]}"; do
    size=$BASE_SIZE
    # run the sequential program
    echo -n "$size,$task_size,$workers," >> $OUTPUT_FILE
    ./s_fastflow.a -r "$payload" -s "$size" -t "$workers" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
done