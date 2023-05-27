#!/bin/bash
# use the bash shell
cd /jet/home/jjia1/Answer
module load openmpi/4.1.1-gcc8.3.1

set -x 
# echo each command to standard out before running it

#sbatch -p RM-shared -t 00:25:00 --ntasks-per-node=8 slurm1.job

mpirun -np 2 program1 10000000000
mpirun -np 4 program1 10000000000
mpirun -np 6 program1 10000000000
mpirun -np 8 program1 10000000000

mpirun -np 2 program2 10000000000
mpirun -np 4 program2 10000000000
mpirun -np 6 program2 10000000000
mpirun -np 8 program2 10000000000

mpirun -np 2 program3 10000000000
mpirun -np 4 program3 10000000000
mpirun -np 6 program3 10000000000
mpirun -np 8 program3 10000000000

mpirun -np 2 program4 10000000000 32768
mpirun -np 4 program4 10000000000 32768
mpirun -np 6 program4 10000000000 32768
mpirun -np 8 program4 10000000000 32768

mpirun -np 2 program4_update 10000000000 32768
mpirun -np 4 program4_update 10000000000 32768
mpirun -np 6 program4_update 10000000000 32768
mpirun -np 8 program4_update 10000000000 32768
