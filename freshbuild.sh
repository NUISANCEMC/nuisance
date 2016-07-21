
# Set Configuration
#export NEUT_ROOT=""
#export CERN_ROOT=""
#export GENIE=""
#export NUWRO=""

rm -rf build
mkdir build
cd build
cmake -DUSE_NuWro=1 -DUSE_NEUT=1 -DUSE_GENIE=0 -DUSE_GiBUU=1 ../
make 
cd -
