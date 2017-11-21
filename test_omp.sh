#!/bin/bash
#
#------------------------------------------------
#example script for running a single-CPU serial
# job via SLURM
#------------------------------------------------
#
#SBATCH --job-name=poff
#SBATCH --output=poff.log
#
#
#Define the number of hours the job should run.
#Maximum runtime is limited to 10 days, ie. 240 hours
#SBATCH --time=12:00:00
#
#Define the amount of RAM used by your job in GigaBytes
#SBATCH --mem=6G
#
#Send emails when a job starts, it is finished or it exits
#SBATCH --mail-user=camille.schreck@ist.ac.at
#SBATCH --mail-type=ALL
#
## number of cpu cores
#SBATCH --ntasks=64
export OMP_NUM_THREADS=64

#run the respective binary through SLURM's srun
CMD=“./poff_no_graph -l test_wood.sc -s broken_cuboidv.sc -e test_wood/testv_ -es 10 -stop 20000”
srun --cpu_bind=verbose --partition chris $CMD
