#!/bin/sh

# Checkout nuisance
git clone http://nuisance.hepforge.org/git/nuisance.git

# Setup Generators
source $PWD/generator-setup.sh

# Build NUISANCE
mkdir nuisance/build/
cd nuisance/build/
cmake ../ -DUSE_NEUT=1 -DUSE_GENIE=1 -DUSE_NUWRO=1 -DBUILD_GEVGEN=0 
make -j6 
make install
source Linux/setup.sh
cd ../../
pwd
echo "Finished building"





