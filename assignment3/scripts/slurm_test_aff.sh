#!/bin/bash
#SBATCH --job-name=Personal     # Job name
#SBATCH --output=output.log     # Save output to a file
#SBATCH --error=error.log       # Save error messages
#SBATCH --time=00:30:00         # there are many tests
#SBATCH --ntasks=1              # Number of tasks (processes)

# Run the sequential tests
srun ./scripts/test_affinity_personalized.sh