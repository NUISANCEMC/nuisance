#!/bin/sh

# Setup NUISANCE
source nuisance/build/Linux/setup.sh

# Run NUISANCE comparison scripts
for obj in ./*allsamples.xml
do
    nuiscomp -c $obj -o ${obj/.xml/.root} 
done

wait
echo "Done running nuisance"