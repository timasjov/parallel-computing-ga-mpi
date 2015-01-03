#!/bin/bash

#SBATCH -N 1
#SBATCH -t 00:10:00
#SBATCH --mem 20000
#SBATCH -p testing
#SBATCH --exclusive

./mmm 1000