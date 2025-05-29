#!/bin/bash

PAYLOADS=(1 64)
ARRAY_SIZE=(10000 100000 1000000 10000000 50000000 100000000)
# create a csv file for the sequential results
OUTPUT_FILE="results/sequential.csv"  # Log file
#initialize the csv file
echo "trial,payload,size,time" > $OUTPUT_FILE

for i in {1..1}; do 
    # loop through each payload
    for payload in "${PAYLOADS[@]}"; do
        # loop through each array size
        for size in "${ARRAY_SIZE[@]}"; do
            # run the sequential program
            echo -n "$i,$payload,$size," >> $OUTPUT_FILE
            ./s_sequential.a -r "$payload" -s "$size" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
        done
    done
done
