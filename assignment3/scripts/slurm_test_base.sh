#!/bin/bash
#SBATCH --job-name=CompTest     # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=02:00:00         # there are many tests
#SBATCH --ntasks=1              # Number of tasks (processes)

# Run make
srun make all

# Run the sequential tests
srun ./scripts/test_sequential.sh

# Run the parallel tests
srun ./scripts/test_parallel.sh