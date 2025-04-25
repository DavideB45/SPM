#!/bin/bash
#SBATCH --job-name=MoreTests    # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=02:00:00         # there are many tests
#SBATCH --ntasks=1              # Number of tasks (processes)

# Run make
srun make all -j

# Run the parallel for tests
srun ./scripts/test_for_parallel.sh

# Run the affinity tests
srun ./scripts/test_affinity.sh