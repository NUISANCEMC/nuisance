#!/bin/bash


mkdir build
cd ./build
if [[ -e CMakeCache.txt ]]; then
  rm -v CMakeCache.txt
fi
cmake -DUSE_NuWro=0 -DUSE_NIWG=0 -DUSE_NEUT=1 -DUSE_GENIE=1 -DUSE_T2K=0 -DUSE_GiBUU=0 -DUSE_NUANCE=0 -DUSE_MINIMIZER=false ../
make clean && make -j8
cd -
