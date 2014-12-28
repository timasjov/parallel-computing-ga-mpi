#!/bin/bash

module load openmpi-1.7.3
module load python-2.7.3

# script to download and compile example codes
# make sure you are in right directory
wget http://hpc.pnl.gov/globalarrays/tutorials/ga-tutorial.tgz
tar -zxvf ga-tutorial.tgz
cd ga-tutorial
cd code_answers
gmake all
