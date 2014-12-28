#!/bin/bash

module load openmpi-1.7.3
module load python-2.7.3

# script to install global arrays toolkit
# make sure you are in right directory
wget hpc.pnl.gov/globalarrays/download/ga-5-3b.tgz
tar -xvf ga-5-3b.tgz
export INSTALLDIR=$PWD/ga53binstall
cd ga-5-3b
./configure --prefix=$INSTALLDIR
make
make install
make checkprogs
export GA_CONFIG=~/Project/ga53binstall/bin/ga-config
