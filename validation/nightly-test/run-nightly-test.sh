#!/bin/sh

# Get Date
DATE=`date +%Y-%m-%d_%H-%M-%S`
echo $DATE

# Make new folder
mkdir nightly-test-${DATE}
cd nightly-test-${DATE}

# Copy scripts
cp ../base-comparisons-1/* ./

# Run NUISANCE
source run-test.sh >> ../nightly-test-log-${DATE}.txt 2>&1




