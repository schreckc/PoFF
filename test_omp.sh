#!/bin/bash
#
#------------------------------------------------
#example script for running a single-CPU serial
# job via SLURM
#------------------------------------------------
#
#SBATCH --job-name=poff_a1
#SBATCH --output=poff.log
#
#
#Define the number of hours the job should run.
#Maximum runtime is limited to 10 days, ie. 240 hours
#SBATCH --time=10:00:00
#
#Define the amount of RAM used by your job in GigaBytes
#SBATCH --mem=6G
#
#Send emails when a job starts, it is finished or it exits
#SBATCH --mail-user=camille.schreck@ist.ac.at
#SBATCH --mail-type=ALL
#
## number of cpu cores
#SBATCH --ntasks=8
export OMP_NUM_THREADS=8
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false
export OMP_PROC_BIND=true


#run the respective binary through SLURM's srun
CMD="./poff_no_graph -l test_wood_a1.conf -s broken_cuboidv.sc -e test_wood_a1/testvv_ -es 10 -stop 15000"
srun --cpu_bind=verbose $CMD
