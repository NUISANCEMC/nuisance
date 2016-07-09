#!/bin/sh

# Quick Script to submit cards immediately to a job handling system from the current directory.

# This is replaced with actual EXT FIT dir do not edit.
EXT_FIT=EXT_FIT_DIR

arguments="-c"
card=$1

scriptname=${card%.card}.sh
outputname=${card%.card}.root

for arg in $@
do
    arguments="$arguments $arg"
done

echo "#!/bin/sh" > $scriptname
echo "source $EXT_FIT/setup.sh" >> $scriptname
echo "ExtFit_minimizer.exe $arguments -o $outputname" >> $scriptname
#echo "rm $scriptname.e*" >> $scriptname
#echo "rm $scriptname.o*" >> $scriptname
#echo "rm $scriptname" >> $scriptname

# Default setup as a condor_q
condor_qsub $scriptname