mkdir build
cd ./build
cmake -DUSE_NuWro=1 -DUSE_NIWG=1 -DUSE_NEUT=1 -DUSE_GENIE=1 -DUSE_T2K=1 -DUSE_GiBUU=1 -DUSE_NUANCE=1  ../
make 
cd -
