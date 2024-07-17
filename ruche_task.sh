#!/bin/bash
#SBATCH --job-name=tt_contr
#SBATCH --exclusive
#SBATCH --output=/gpfs/workdir/torria/pdominik/ruche_output/%x.o%j
#SBATCH --ntasks=1
#SBATCH --time=04:00:00
#SBATCH --cpus-per-task=40
#SBATCH --partition=cpu_med           # (see available partitions)

# To clean and load modules defined at the compile and link phases
module purge
module load miniconda3/24.5.0/gcc-13.2.0
module load gcc/13.2.0/gcc-4.8.5

# To compute in the submission directory
cd ${SLURM_SUBMIT_DIR}

# Load the conda environment
eval "$(conda shell.bash hook)"
source activate /gpfs/workdir/torria/pdominik/tt_contr_env

# Echo of commands
set -x

# execution
which python3
python3 tools/compute_tests.py "$1"