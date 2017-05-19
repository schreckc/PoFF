#!/bin/bash
#
#$ -S /bin/bash
#$ -N omptest
#$ -pe openmp 16
#$ -l h_vmem=2G

export OMP_NUM_THREADS=16  # gets number from -pe
### for performance reasons, you might want to use some agressive scheduling ###
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false
export OMP_PROC_BIND=true


mkdir test_snow0
./poff_no_graph -l snow0.conf -s falling_cube_cylinder.sc -e test_snow0/test -es 100 -stop 30000
mkdir test_snow1
./poff_no_graph -l snow1.conf -s falling_cube_cylinder.sc -e test_snow1/test -es 100 -stop 30000