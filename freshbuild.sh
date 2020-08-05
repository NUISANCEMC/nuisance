#!/bin/bash
dir="build_neut_t2k"

mkdir $dir
cd $dir
if [[ -e CMakeCache.txt ]]; then
  rm -v CMakeCache.txt
fi

cmake -DUSE_NuWro=0 \
      -DUSE_NIWG=1 \
      -DUSE_MINERvA_RW=1 \
      -DUSE_NEUT=1 \
      -DUSE_GENIE=0 \
      -DUSE_T2K=1 \
      -DUSE_GiBUU=0 \
      -DUSE_NUANCE=0 \
      -DUSE_MINIMIZER=false ../

make clean && make -j8
cd -
