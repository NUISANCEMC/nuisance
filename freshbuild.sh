
rm -rf build
mkdir build
cd build
cmake -DUSE_NuWro=1 -DUSE_NEUT=0 -DUSE_GENIE=0 -DUSE_T2K=0 -DUSE_NIWG=0 ../
make 
cd -
