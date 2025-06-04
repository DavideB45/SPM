# Computed n: 13993959.035198892 for p: 1
# Computed n: 26917759.447102636 for p: 2
# Computed n: 51848806.823173285 for p: 4
# Computed n: 99999999.99999982 for p: 8

#!/bin/bash

payload=1
workers=15 # it was the best
SIZES=( 26917759 51848806) # 13993959, 99999999 was already done 
COMPUTERS=(2 4) # 1 , 8 was already done

# # create a csv file for the sequential results
# OUTPUT_FILE="results/multinode_ff_weak_ref.csv"  # Log file
# # #initialize the csv file
# echo "nodes,size,workers,time" > $OUTPUT_FILE

# for i in "${!COMPUTERS[@]}"; do
#     nodes=1
#     size=${SIZES[$i]}
#     # run the sequential program
#     echo -n "$nodes,$size,$workers," >> $OUTPUT_FILE
#     srun --nodes=$nodes \
#          --ntasks-per-node=1 \
#          --time=00:02:00 \
#          --mpi=pmix \
#          ./s_mpi_ff.a -r "$payload" -s "$size" -t "$workers" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
# done

OUTPUT_FILE="results/multinode_seq_weak_ref.csv"  # Log file
# #initialize the csv file
echo "nodes,size,time" > $OUTPUT_FILE
for i in "${!COMPUTERS[@]}"; do
    nodes=1
    size=${SIZES[$i]}
    # run the sequential program
    echo -n "$nodes,$size," >> $OUTPUT_FILE
    srun --nodes=$nodes \
         --ntasks-per-node=1 \
         --time=00:02:00 \
         --mpi=pmix \
         ./s_mpi.a -r "$payload" -s "$size" -b "999999999" | grep 'sort_records' | grep -o '[0-9.]\+' >> $OUTPUT_FILE
done