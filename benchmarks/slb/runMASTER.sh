#!/bin/bash -l

#SBATCH
#SBATCH --job-name=MASTER
#SBATCH --time=4:0:0
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --partition=shared
#SBATCH --mail-type=end
#SBATCH --mail-user=jpatsol1@jhu.edu


module load R

mpirun Rscript runDataSet.r

## array

