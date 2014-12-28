#!/bin/bash

# sample script for submitting Global Arrays file

#SBATCH -N 2
#SBATCH -t 00:10:00
#SBATCH --sockets-per-node=2
#SBATCH --cores-per-socket=10
#SBATCH --threads-per-core=1
#SBATCH -p testing 
#SBATCH --exclusive

export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi.so

mpirun -n 20 hello_world.x
