#!/bin/bash
# test_mpi_seq.sh
# Launches the program on 1, 2, 4, and 8 nodes, one after the other, and logs results.

srun -n 1 --time=00:02:00 make s_mpi_ff_db

# task size will be the default 10000
ARRAY_SIZE=(10000000 50000000 100000000)
MPI_BUFF_SIZE=(1000000 2000000 4000000 10000000 )
NODES_T=(2 4 8)
THREAD_COUNTS=(7 15)
OUTPUT_FILE="results/multinode_ff_db.csv"  # Log file

# Initialize the CSV file
echo "nodes,size,buff_size,thread,time" > $OUTPUT_FILE

# Loop through node counts
for N in ${NODES_T[@]}; do
    echo "Running on $N node(s)"
    # Loop through each array size
    for size in "${ARRAY_SIZE[@]}"; do
        # Loop through each buffer size
        for buff_size in "${MPI_BUFF_SIZE[@]}"; do
            # Loop through thread counts
            for threads in "${THREAD_COUNTS[@]}"; do
                if [ $((buff_size * N)) -ge $size ]; then
                    echo "Skipping buff_size=$buff_size, nodes=$N, size=$size as double buffering is not useful"
                    continue
                fi
                echo -n "$N,$size,$buff_size,$threads," >> $OUTPUT_FILE
                srun --nodes=$N \
                    --ntasks-per-node=1 \
                    --time=00:03:00 \
                    --mpi=pmix \
                    ./s_mpi_ff_db.a -r "1" -s "$size" -b "$buff_size" -t "$threads" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
            done
        done
    done
done
