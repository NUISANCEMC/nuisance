#!/bin/env sh

# Read Inputs
PROFILEOUT=$1
PROGRAM=$(which $2)
ARGUMENTS=${@:3}
PPROF=$NUISANCE/build/Ext/src/gperftools/src/pprof

# Log
echo "Profiling : $PROGRAM"
echo "Saving to : $CPUPROFILE"
echo "Running : $PROGRAM $ARGUMENTS"

# Run PROFILING
echo ""
CPUPROFILE=$PROFILEOUT $2 $ARGUMENTS

# Make PDF + TXT
echo ""
echo "Building outputs"
$PPROF --pdf $PROGRAM $PROFILEOUT > ${PROFILEOUT}.pdf
$PPROF --text $PROGRAM $PROFILEOUT > ${PROFILEOUT}.txt
