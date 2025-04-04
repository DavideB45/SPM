#!/bin/bash

# PROBLEM 1 represents many small ranges
# PROBLEM 2 represents a few large ranges
# PROBLEM 3 is a mix of small and large ranges

# Define parameters
THREADS=(2 4 8 16 32 1)  # Different numbers of threads
CHUNKS=(1 10 50 100 1000)  # Different chunk sizes
# Many small ranges
PROBLEM_1=()
START=1
END=1000
OFFSET=357 # this is a number arbitrarely chosen to update the start and end values and make them not too close
for i in {1..3000}; do
	PROBLEM_1+=("$START-$END") # Add range to list
	START=$((END + OFFSET)) # Update start
	END=$((START + 999)) # Update end (1000 numbers in each range)
done

# Few large ranges
PROBLEM_2=("1-1000000" "4000001-6000000" "30000000-35000000" "4000001-5000000" "50000000-100000000")

# Mix of small and large ranges
PROBLEM_3=()
for i in {1..50}; do
	if [ $((i % 8)) -eq 0 ]; then
		PROBLEM_3+=("${PROBLEM_2[$((i % ${#PROBLEM_2[@]}))]}") # Add large range
	else
		PROBLEM_3+=("${PROBLEM_1[$i]}") # Add small range
	fi
done
EXECUTABLE="./parallel_collatz.out"  # Path to compiled C++ program
OUTPUT_FILE="collatz_results_2.csv"  # Log file

echo "threads,chunk_size,problem,dynamic,free,time" > $OUTPUT_FILE

SEQUENTIAL="./sequential_collatz.out"  # Path to compiled C++ program
################################## SEQUENTIAL ##################################
#        t c p d f
echo -n "0,0,1,0,0," >> $OUTPUT_FILE
$SEQUENTIAL ${PROBLEM_1[@]} | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
#        t c p d f
echo -n "0,0,2,0,0," >> $OUTPUT_FILE
$SEQUENTIAL ${PROBLEM_2[@]} | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
#        t c p d f
echo -n "0,0,3,0,0," >> $OUTPUT_FILE
$SEQUENTIAL ${PROBLEM_3[@]} | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
	
# Run tests
for threads in ${THREADS[@]}; do
	for chunk in ${CHUNKS[@]}; do
		############################## STATIC SCHEDULING ##############################
		#           t       c    p d f
		echo -n "$threads,$chunk,1,0,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_1[@]} "-n" $threads "-c" $chunk | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,2,0,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_2[@]} "-n" $threads "-c" $chunk | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,3,0,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_2[@]} "-n" $threads "-c" $chunk | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
	
		############################## DYNAMIC SCHEDULING ##############################
		################################## USING LOCK ##################################
		#           t       c    p d f
		echo -n "$threads,$chunk,1,1,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_1[@]} "-n" $threads "-c" $chunk "-d" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,2,1,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_2[@]} "-n" $threads "-c" $chunk "-d" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,3,1,0," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_3[@]} "-n" $threads "-c" $chunk "-d" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE

		############################## DYNAMIC SCHEDULING ##############################
		################################## LOCK FREE ###################################
		#           t       c    p d f
		echo -n "$threads,$chunk,1,1,1," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_1[@]} "-n" $threads "-c" $chunk "-d" "-f" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,2,1,1," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_2[@]} "-n" $threads "-c" $chunk "-d" "-f" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
		#           t       c    p d f
		echo -n "$threads,$chunk,3,1,1," >> $OUTPUT_FILE
		$EXECUTABLE ${PROBLEM_3[@]} "-n" $threads "-c" $chunk "-d" "-f" | grep 'total' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
	
	
	done
done