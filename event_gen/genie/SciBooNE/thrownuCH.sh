#!/bin/bash

if [ -z $1 ]; then
  echo "Please pass a number of events to generate."
  exit 1
fi

if [ -z $GENIE_XSEC_FILE ] || [ ! -e $GENIE_XSEC_FILE ]; then
  echo "Cannot find GENIE xsec file @ \$ENV{GENIE_XSEC_FILE}."
  exit 1
fi

NEVS=$1

BEAMMODE="FHC"
NU_PDG=14
FLUX_FILE=${NUISANCE}/data/flux/sciboone_fhc.root
FLUX_HIST=flux_numu
if [ ! -z $2 ] && [ "${2}" == "RHC" ]; then
  echo "Do not have SciBooNE RHC Flux."
  exit 1
fi

RUNNUM=${RANDOM}

if [ -e gntp.${RUNNUM}.ghep.root ]; then
   echo "Already have file: gntp.${RUNNUM}.ghep.root, not overwriting."
   exit 1
fi

if [ -e SciBooNE.CH.${BEAMMODE}.prep.root ]; then
   echo "Already have file: SciBooNE.CH.${BEAMMODE}.prep.root, not overwriting."
   exit 1
fi

CHTARGET="1000060120[.923076],1000010010[.076924]"

gevgen \
   -p ${NU_PDG} -t ${CHTARGET} \
   -r ${RUNNUM} -e 0.1,10 \
   -f ${FLUX_FILE},${FLUX_HIST} \
   -n ${NEVS} --seed ${RUNNUM} \
   --cross-sections ${GENIE_XSEC_FILE} \
   --event-generator-list Default+MEC \
   --message-thresholds Messenger_whisper.xml

if [ -e gntp.${RUNNUM}.ghep.root ]; then
   rm -f input-flux.root
   rm -f genie-mcjob-${RUNNUM}.status

   mv gntp.${RUNNUM}.ghep.root SciBooNE.CH.${BEAMMODE}.prep.root
   PrepareGENIE -i SciBooNE.CH.${BEAMMODE}.prep.root \
               -f ${FLUX_FILE},${FLUX_HIST} \
               -t ${CHTARGET}
else
   echo "Failed to produce expected output file: gntp.${RUNNUM}.ghep.root"
   exit 1
fi
