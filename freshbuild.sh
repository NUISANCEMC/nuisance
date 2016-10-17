
# Set Configuration
#export NEUT_ROOT=""
#export CERN_ROOT=""
#export GENIE=""
#export NUWRO=""

mkdir build
cd ./build
cmake -DUSE_NuWro=1 -DUSE_NIWG=1 -DUSE_NEUT=1 -DUSE_GENIE=1 -DUSE_GiBUU=1 -DUSE_NUANCE=1 ../
make 
cd -
