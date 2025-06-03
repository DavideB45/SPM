#!/bin/bash

payload=1
SIZES=(1000000 1900000 3650000 7000000 13400000)
WORKERS=(1 3 7 15 31) # this are the numbers because the total is +1 for the emitter

# create a csv file for the sequential results
OUTPUT_FILE="results/naive_ff_w.csv"  # Log file
#initialize the csv file
echo "size,workers,time" > $OUTPUT_FILE


for i in "${!WORKERS[@]}"; do
    workers=${WORKERS[$i]}
    size=${SIZES[$i]}
    # run the sequential program
    echo -n "$size,$workers," >> $OUTPUT_FILE
    srun --time=00:01:00 ./s_fastflow.a -r "$payload" -s "$size" -t "$workers" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
done

OUTPUT_FILE="results/working_emitter_ff_w.csv"  # Log file
echo "size,workers,time" > $OUTPUT_FILE
for i in "${!WORKERS[@]}"; do
    workers=${WORKERS[$i]}
    size=${SIZES[$i]}
    # run the sequential program
    echo -n "$size,$workers," >> $OUTPUT_FILE
    srun --time=00:01:00 ./s_work_em_ff.a -r "$payload" -s "$size" -t "$workers" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
done