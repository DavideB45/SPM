#!/bin/bash
#SBATCH --job-name=ff_tests     # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=00:10:00         # there are many tests
#SBATCH --nodes=4               # Number of nodes
#SBATCH --ntasks-per-node=1     # Number of tasks per node

# Run make
srun -n 1 make all


PAYLOADS=(1)
ARRAY_SIZE=(1000000 10000000 50000000 100000000)
MPI_BUFF_SIZE=(128 10000 25000 100000)
# create a csv file for the sequential results
OUTPUT_FILE="results/multinode_seq.csv"  # Log file
#initialize the csv file
echo "trial,size,buff_size,time" > $OUTPUT_FILE

for i in {1..1}; do 
    # loop through each array size
    for size in "${ARRAY_SIZE[@]}"; do
        # loop through each task size
        for buff_size in "${MPI_BUFF_SIZE[@]}"; do
            echo -n "$i,$payload,$size,$buff_size," >> $OUTPUT_FILE
            ./mpi.a -r "$payload" -s "$size" -b "$buff_size" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
        done
    done
done