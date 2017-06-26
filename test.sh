#!/bin/bash
#
#$ -S /bin/bash
#$ -N omptest
#$ -pe openmp 16
#$ -l h_vmem=2G

export OMP_NUM_THREADS=64  # gets number from -pe
### for performance reasons, you might want to use some agressive scheduling ###
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false
export OMP_PROC_BIND=true

rm -r test_anis
mkdir test_anis
./poff_no_graph -l anisotropy_elastic_plastic.conf -s column.sc -e test_anis/test -es 10 -stop 100000
