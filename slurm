#!/bin/bash -e 

#SBATCH --account nesi99999
#SBATCH --job-name cpu-bench
#SBATCH --cpus-per-task 12
#SBATCH --mem 12G
#SBATCH --output slog/%j.out

module purge >/dev/null 2>&1
module load GCC/12.3.0


export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}

./cpu_benchmark
