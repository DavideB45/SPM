#!/bin/bash

PAYLOADS=(1 64)
ARRAY_SIZE=(10000 100000 1000000 10000000 50000000 100000000)
TASK_SIZES=(-1 128 10000 25000 100000)
WORKERS=(1 3 7 15 31) # this are the numbers because the total is +1 for the emitter
# create a csv file for the sequential results
OUTPUT_FILE="results/working_emitter_ff.csv"  # Log file
#initialize the csv file
echo "trial,payload,size,task_size,workers,time" > $OUTPUT_FILE

for i in {1..5}; do 
    # loop through each payload
    for payload in "${PAYLOADS[@]}"; do
        # loop through each array size
        for size in "${ARRAY_SIZE[@]}"; do
            # loop through each task size
            for task_size in "${TASK_SIZES[@]}"; do
                # if task size > size, skip
                if [ $task_size -gt $size ]; then
                    continue
                fi
                for workers in "${WORKERS[@]}"; do
                    # run the sequential program
                    echo -n "$i,$payload,$size,$task_size,$workers," >> $OUTPUT_FILE
                    ./s_work_em_ff.a -r "$payload" -s "$size" -k "$task_size" -t "$workers" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
                done
            done
        done
    done
done