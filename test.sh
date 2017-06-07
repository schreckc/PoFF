-s falling_cube_cylinder.scq#!/bin/bash
#
#$ -S /bin/bash
#$ -N omptest
#$ -pe openmp 16
#$ -l h_vmem=2G

export OMP_NUM_THREADS=90  # gets number from -pe
### for performance reasons, you might want to use some agressive scheduling ###
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false
export OMP_PROC_BIND=true


mkdir test_anis
./poff_no_graph -l anisotropy.conf -s falling_cube_cylinder.sc -e test_anis/test -es 10 -stop 10000
