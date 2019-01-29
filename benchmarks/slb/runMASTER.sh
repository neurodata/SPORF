#!/bin/bash -l

#SBATCH
#SBATCH --job-name=UCI_RerF_Benchmarks
#SBATCH --time=12:0:0
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --partition=parallel,shared,lrgmem
#SBATCH --mail-type=end
#SBATCH --mail-user=jpatsol1@jhu.edu


## Run this with the following command
## sbatch --array=1-25%4 runMASTER.sh

module load gsl
module load R

PARAMS_FILE="uci_params.csv"

Rscript single_dataset_run.R $PARAMS_FILE $SLURM_ARRAY_TASK_ID
