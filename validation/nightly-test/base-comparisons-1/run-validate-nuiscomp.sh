#!/bin/sh

for obj in genie neut nuwro
do
    python validate-nuiscomp.py $PWD/../base-benchmark/${obj}_allsamples.root $PWD/${obj}_allsamples.root ${obj}_nuiscomp -b &
done
wait
