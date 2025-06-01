#!/bin/bash
# test_mpi_seq.sh
# Launches the program on 1, 2, 4, and 8 nodes, one after the other, and logs results.

srun -n 1 make all

PAYLOADS=(1)
ARRAY_SIZE=(1000000 10000000 50000000 100000000)
MPI_BUFF_SIZE=(128 10000 25000 100000)
OUTPUT_FILE="results/multinode_seq.csv"  # Log file

# Initialize the CSV file
echo "nodes,trial,size,buff_size,time" > $OUTPUT_FILE

# Loop through node counts
for N in 2 4 8; do
    echo "Running on $N node(s)"
    # Loop through trials
    for i in {1..1}; do
        # Loop through each array size
        for size in "${ARRAY_SIZE[@]}"; do
            # Loop through each buffer size
            for buff_size in "${MPI_BUFF_SIZE[@]}"; do
                echo -n "$N,$i,${PAYLOADS[0]},$size,$buff_size," >> $OUTPUT_FILE
                srun --nodes=$N \
                     --ntasks-per-node=1 \
                     --time=00:04:00 \
                     --mpi=pmix \
                     ./mpi.a -r "${PAYLOADS[0]}" -s "$size" -b "$buff_size" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
            done
        done
    done
done