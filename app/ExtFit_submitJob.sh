#!/bin/sh

# Submit the input script as a job ( should be editted for user cluster system )
# Default is set for a condor queue with a standard job time of 24 hours

# The only purpose of this script is to allow the fitter to submit jobs to a scheduler from inside a fit. Change to whatever bash script submission system is required.
condor_qsub $1 -e /dev/null -o /dev/null