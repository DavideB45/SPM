#!/bin/bash
#SBATCH --job-name=ff_tests     # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=00:05:00         # there are many tests
#SBATCH --ntasks=1              # Number of tasks (processes)
#SBATCH --priority=1            # Set maximum priority

# Run make
srun make all -j

# Run the sequential tests
# srun ./scripts/test_sequential.sh

# Run the parallel tests
# srun ./scripts/test_naive_ff.sh
# srun ./scripts/test_working_emitter_ff.sh

# weak scaling tests
srun ./scripts/test_weak_scaling.sh