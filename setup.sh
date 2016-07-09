#!/bin/sh

#-------------------------------------------------
# PATH LIBRARY Functions
#-------------------------------------------------      
# Path PrePend Function
pathprepend() {
    for ARG in "$@"
    do
	if [ ! -d "$ARG" ]; then
	    echo "$ARG Directory does not exist. Cannot add to PATH"
	fi
	
	if [ -d "$ARG" ] && [[ ":$PATH:" != *":$ARG:"* ]]; then
	    PATH="$ARG${PATH:+":$PATH"}"
	fi
    done
}

libraryprepend() {
    for ARG in "$@"
    do

	if [ ! -d "$ARG" ]; then
	    echo "$ARG Directory does not exist. Cannot add to LD_LIBRARY_PATH"
	fi
	
	if [ -d "$ARG" ] && [[ ":$LD_LIBRARY_PATH:" != *":$ARG:"* ]]; then
	    LD_LIBRARY_PATH="$ARG${LD_LIBRARY_PATH:+":$LD_LIBRARY_PATH"}"
	fi
    done
}
#-------------------------------------------------




#-------------------------------------------------
# ExtFit Setup
#-------------------------------------------------
export EXT_FIT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $EXT_FIT/extfitter_env.sh

pathprepend ${EXT_FIT}/bin
libraryprepend ${EXT_FIT}/lib

source $ROOTSYS/bin/thisroot.sh 

#-------------------------------------------------
# NEUT
#------------------------------------------------- 
if [ -d "$NEUT" ]
then
    export NEUT_ROOT="$NEUT"
    export NEUT_RUNDIR="$NEUT/src/neutsmpl"
    export NEUT_REWEIGHT="$NEUT/src/reweight"
    
    pathprepend ${NEUT_REWEIGHT}
    pathprepend ${NEUT_ROOT}
    pathprepend ${NEUT_RUNDIR}/Linux_pc
    pathprepend ${NEUT_RUNDIR}/bin
    
    libraryprepend ${NEUT_REWEIGHT}
    
    for val in ${NEUT_ROOT}/src/*
    do
	if [ ! -d "$val" ] || [ "$val" != *"neutgeom"* ]; then
	    continue
	fi
	libraryprepend ${val}
    done
fi

#-------------------------------------------------
# GENIE
#------------------------------------------------- 
if [ -d "$GENIE" ]
then
    libraryprepend $GENIE/lib    
    pathprepend $GENIE/bin
fi

#-------------------------------------------------
# CERNLIB
#------------------------------------------------- 
if [ -d "$CERN" ]
then
    export CERN_ROOT=$CERN/$CERN_LEVEL
    export CERNLIB=$CERN_ROOT/lib

    pathprepend ${CERN_ROOT}/bin
    libraryprepend ${CERNLIB}
fi

#-------------------------------------------------
# NIWGREWEIGHT
#------------------------------------------------- 
if [ -d "$NIWGREWEIGHT" ]
then

    export NIWGREWEIGHT_INPUTS=$NIWGREWEIGHT/inputs
    
    pathprepend ${NIWGREWEIGHT}
    libraryprepend ${NIWGREWEIGHT}
fi

#-------------------------------------------------
# NUWRO
#------------------------------------------------- 
if [ -d "$NUWRO" ]
then
    pathprepend ${NUWRO}/bin $NUWRO $NUWRO/src
    libraryprepend $NUWRO/bin $NUWRO/src
fi

#-------------------------------------------------
# T2KREWEIGHT
#-------------------------------------------------
if [ -d "$T2KREWEIGHT" ] 
then
    pathprepend ${T2KREWEIGHT}/bin
    pathprepend ${T2KREWEIGHT}
    pathprepend ${T2KREWEIGHT}/app
    libraryprepend ${T2KREWEIGHT}/lib
fi

#-------------------------------------------------
# PYTHIA 6
#-------------------------------------------------
libraryprepend ${PYTHIA6_LIB}

#-------------------------------------------------  
# LIBXML2
#-------------------------------------------------
libraryprepend $LIBXML2

#-------------------------------------------------  
#LHAPDF
#-------------------------------------------------
libraryprepend $LHAPDF_LIB

#-------------------------------------------------
# LOG4CPP
#-------------------------------------------------
libraryprepend $LOG4CPP_LIB

#------------------------------------------------- 
# LHAPATH
#-------------------------------------------------
libraryprepend $LHAPATH
pathprepend $LHAPATH

#-------------------------------------------------
# oaAnalysis
#-------------------------------------------------
libraryprepend $OAANALYSISLIBS
