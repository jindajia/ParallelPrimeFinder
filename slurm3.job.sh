#!/bin/bash
# use the bash shell
cd /jet/home/jjia1/Answer
module load openmpi/4.1.1-gcc8.3.1

set -x 
# echo each command to standard out before running it

#sbatch -o slurm-3.out -p RM -t 00:09:00 -N 4 slurm3.job
mpirun -np 128 program1 10000000000
mpirun -np 256 program1 10000000000
mpirun -np 384 program1 10000000000
mpirun -np 512 program1 10000000000

mpirun -np 128 program2 10000000000
mpirun -np 256 program2 10000000000
mpirun -np 384 program2 10000000000
mpirun -np 512 program2 10000000000

mpirun -np 128 program3 10000000000
mpirun -np 256 program3 10000000000
mpirun -np 384 program3 10000000000
mpirun -np 512 program3 10000000000


mpirun -np 128 program4 10000000000 32768
mpirun -np 256 program4 10000000000 32768
mpirun -np 384 program4 10000000000 32768
mpirun -np 512 program4 10000000000 32768

mpirun -np 128 program4_update 10000000000 32768
mpirun -np 256 program4_update 10000000000 32768
mpirun -np 384 program4_update 10000000000 32768
mpirun -np 512 program4_update 10000000000 32768