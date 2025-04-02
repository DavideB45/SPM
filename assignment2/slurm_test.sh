#!/bin/bash
#SBATCH --job-name=Analysis     # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=01:00:00         # I don't know an upperbound to the collatz conjecture
#SBATCH --ntasks=1              # Number of tasks (processes)

# Run make
srun make all
srun test.sh