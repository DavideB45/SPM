#!/bin/bash

# Define parameters
THREADS=(2 4 8 16)  # Different numbers of threads
CHUNKS=(50 100 500 1000)  # Different chunk sizes
RANGES=("1-1000 50000000-100000000" "1-10000 10000000-20000000") # Test ranges
EXECUTABLE="./parallel_collatz.out"  # Path to compiled C++ program
OUTPUT_FILE="collatz_results.txt"  # Log file

# Clear output file
echo "Collatz Parallel Execution Results" > "$OUTPUT_FILE"
echo "===================================" >> "$OUTPUT_FILE"

for n in "${THREADS[@]}"; do # Loop over thread counts
    for c in "${CHUNKS[@]}"; do # Loop over chunk sizes
        for r in "${RANGES[@]}"; do # Loop over ranges
            echo "Running with -n $n -c $c for ranges: $r"
            echo "-----------------------------------" >> "$OUTPUT_FILE"
            echo "Threads: $n | Chunk: $c | Ranges: $r" >> "$OUTPUT_FILE"

            # Measure execution time
            START_TIME=$(date +%s.%N)
            $EXECUTABLE -n $n -c $c $r >> "$OUTPUT_FILE" 2>&1
            END_TIME=$(date +%s.%N)

            # Compute elapsed time
            ELAPSED_TIME=$(echo "$END_TIME - $START_TIME" | bc)
            echo "Execution Time: $ELAPSED_TIME seconds" >> "$OUTPUT_FILE"
            echo "-----------------------------------" >> "$OUTPUT_FILE"
        done
    done
done

echo "All tests completed. Results saved in $OUTPUT_FILE."
