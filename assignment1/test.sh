#!/bin/bash

# Check if executable name is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <plain|auto|avx>"
    exit 1
fi

EXECUTABLE=softmax_$1.o

# Check if executable exists and is executable
if [ ! -f "./$EXECUTABLE" ]; then
    echo "Error: $EXECUTABLE not found!"
    exit 1
fi

if [ ! -x "./$EXECUTABLE" ]; then
    chmod +x ./$EXECUTABLE
fi

# Number of runs for each input size
NUM_RUNS=5

# Create output filename with timestamp
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUTPUT_FILE="${1}_results_${TIMESTAMP}.csv"

# Create CSV header
echo "size,run,time" > $OUTPUT_FILE

# Test with various input sizes
for size in 10 100 1003 10003 10003 100003 1000001 3000000 8000000 10000001 40000089 80000000 200000000 500000057 800000000; do
    for ((run=1; run<=NUM_RUNS; run++)); do
        # Run the binary and extract the timing information
        output=$(./$EXECUTABLE $size)
        time_value=$(echo "$output" | grep "elapsed time" | sed -E 's/.*: ([0-9.e-]+)s/\1/')
        
        # Save to CSV
        echo "$size,$run,$time_value" >> $OUTPUT_FILE
    done
done

echo "Results saved to $OUTPUT_FILE"