#!/bin/bash
# use the bash shell
cd /jet/home/jjia1/Answer
module load openmpi/4.1.1-gcc8.3.1

set -x 
# echo each command to standard out before running it

#sbatch -o slurm-2.out -p RM-shared -t 00:16:00 --ntasks-per-node=64 slurm2.job
mpirun -np 16 program1 10000000000
mpirun -np 24 program1 10000000000
mpirun -np 32 program1 10000000000
mpirun -np 40 program1 10000000000
mpirun -np 48 program1 10000000000
mpirun -np 64 program1 10000000000

mpirun -np 16 program2 10000000000
mpirun -np 24 program2 10000000000
mpirun -np 32 program2 10000000000
mpirun -np 40 program2 10000000000
mpirun -np 48 program2 10000000000
mpirun -np 64 program2 10000000000


mpirun -np 16 program3 10000000000
mpirun -np 24 program3 10000000000
mpirun -np 32 program3 10000000000
mpirun -np 40 program3 10000000000
mpirun -np 48 program3 10000000000
mpirun -np 64 program3 10000000000


mpirun -np 16 program4 10000000000 32768
mpirun -np 24 program4 10000000000 32768
mpirun -np 32 program4 10000000000 32768
mpirun -np 40 program4 10000000000 32768
mpirun -np 48 program4 10000000000 32768
mpirun -np 64 program4 10000000000 32768

mpirun -np 16 program4_update 10000000000 32768
mpirun -np 24 program4_update 10000000000 32768
mpirun -np 32 program4_update 10000000000 32768
mpirun -np 40 program4_update 10000000000 32768
mpirun -np 48 program4_update 10000000000 32768
mpirun -np 64 program4_update 10000000000 32768
