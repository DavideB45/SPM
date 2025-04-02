#!/bin/bash

# Define parameters
THREADS=(2 4 8 16)  # Different numbers of threads
CHUNKS=(50 100 500 1000)  # Different chunk sizes
# Many small ranges
PROBLEM_1=()
START=1
END=1000
OFFSET=357 # this is a number arbitrarely chosen to update the start and end values and make them not too close
for i in {1..10}; do
	PROBLEM_1+=("$START-$END") # Add range to list
	START=$((END + OFFSET)) # Update start
	END=$((START + 999)) # Update end (1000 numbers in each range)
done
echo "Problem 1: ${PROBLEM_1[@]}"
exit 0
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
