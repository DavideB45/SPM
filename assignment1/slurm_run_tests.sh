#!/bin/bash
#SBATCH --job-name=db3_tests    # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=00:20:00         # Set a time limit (20 minutes)
#SBATCH --ntasks=1              # Number of tasks (processes)
#SBATCH --cpus-per-task=1       # Number of CPU cores

# Run make
srun make all
srun test.sh plain
srun test.sh auto
srun test.sh avx