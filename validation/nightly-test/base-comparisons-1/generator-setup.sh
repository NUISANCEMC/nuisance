# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NUISANCE.
#
#    NUISANCE is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NUISANCE is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

#!/bin/sh
if ! [[ ":$PATH:" == *":/home/stowell/t2krep/NIWG/nuisance/branches/master_refactor/master/builds/default/Linux/bin:"* ]]; then
  export PATH=/home/stowell/t2krep/NIWG/nuisance/branches/master_refactor/master/builds/default/Linux/bin:$PATH
fi

if ! [[ ":$LD_LIBRARY_PATH:" == *":/home/stowell/t2krep/NIWG/nuisance/branches/master_refactor/master/builds/default/Linux/lib:"* ]]; then
  export LD_LIBRARY_PATH=/home/stowell/t2krep/NIWG/nuisance/branches/master_refactor/master/builds/default/Linux/lib:$LD_LIBRARY_PATH
fi

if [[ ! "${ROOTSYS}" ]]; then
  echo "[INFO]: Sourcing ROOT from: /usr/local/t2k-software/ROOT-NUISANCE/root"
  source "/usr/local/t2k-software/ROOT-NUISANCE/root/bin/thisroot.sh"
fi


if [[ "1" != "0" ]]; then
  echo "[INFO]: Adding NEUT library paths to the environment."
  export NEUT_ROOT=/data/stowell/NIWG/neut/branches/neut_5.3.6_freenucleonCCQE
  export CERN=/usr/local/t2k-software/cern/
  export CERN_LEVEL=2005
  if ! [[ ":$LD_LIBRARY_PATH:" == *":${NEUT_ROOT}/lib/Linux_pc:"* ]]; then
    export LD_LIBRARY_PATH=${NEUT_ROOT}/lib/Linux_pc:$LD_LIBRARY_PATH
  fi
  if ! [[ ":$LD_LIBRARY_PATH:" == *":${NEUT_ROOT}/src/reweight:"* ]]; then
    export LD_LIBRARY_PATH=${NEUT_ROOT}/src/reweight:$LD_LIBRARY_PATH
  fi
fi

if [[ "1" != "0" ]]; then
  echo "[INFO]: Adding NuWro library paths to the environment."
  export NUWRO="/usr/local/t2k-software/NuWro-v12"
  if ! [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/t2k-software/NuWro-v12/build/Linux/lib:"* ]]; then
    export LD_LIBRARY_PATH=/usr/local/t2k-software/NuWro-v12/build/Linux/lib:$LD_LIBRARY_PATH
  fi
fi

if [[ "TRUE" != "0" ]]; then
  echo "[INFO]: Adding PYTHIA6 library paths to the environment."
  export PYTHIA6="/usr/local/t2k-software/pythia6"
  if ! [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/t2k-software/pythia6:"* ]]; then
    export LD_LIBRARY_PATH=/usr/local/t2k-software/pythia6:$LD_LIBRARY_PATH
  fi
fi

if [[ "1" != "0" ]]; then
  echo "[INFO]: Adding GENIE paths to the environment."

  export GENIE="/data/stowell/NIWG/genie/branches/R-2_12_6"

  export LHAPDF_LIB="/usr/local/t2k-software/lhapdf/lib/"
  export LHAPDF_INC="/usr/local/t2k-software/lhapdf/include/"

  export LIBXML2_LIB="/usr/lib/"
  export LIBXML2_INC="/usr/include/libxml2/libxml/"

  export LOG4CPP_LIB="/usr/local/t2k-software/log4cpp/src/.libs"
  export LOG4CPP_INC="/usr/local/t2k-software/log4cpp/include"

  export LHAPATH="/usr/local/t2k-software/lhapdf/share/lhapdf/PDFsets"

  if ! [[ ":$LD_LIBRARY_PATH:" == *":/data/stowell/NIWG/genie/branches/R-2_12_6/lib:"* ]]; then
    export LD_LIBRARY_PATH=/data/stowell/NIWG/genie/branches/R-2_12_6/lib:$LD_LIBRARY_PATH
  fi

  if ! [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/t2k-software/lhapdf/lib/:"* ]]; then
    export LD_LIBRARY_PATH=/usr/local/t2k-software/lhapdf/lib/:$LD_LIBRARY_PATH
  fi

  if ! [[ ":$LD_LIBRARY_PATH:" == *":/usr/lib/:"* ]]; then
    export LD_LIBRARY_PATH=/usr/lib/:$LD_LIBRARY_PATH
  fi

  if ! [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/t2k-software/log4cpp/src/.libs:"* ]]; then
    export LD_LIBRARY_PATH=/usr/local/t2k-software/log4cpp/src/.libs:$LD_LIBRARY_PATH
  fi
fi

if [[ "0" != "0" ]]; then
  echo "[INFO]: Adding NIWG paths to the environment."
  export NIWG=
  export NIWGREWEIGHT_INPUTS=/inputs
  if ! [[ ":$LD_LIBRARY_PATH:" == *"::"* ]]; then
    export LD_LIBRARY_PATH=${NIWG}:${LD_LIBRARY_PATH}
  fi

fi

if [[ "0" != "0" ]]; then
  echo "[INFO]: Adding T2K paths to the environment."
  export T2KREWEIGHT=
  if ! [[ ":$LD_LIBRARY_PATH:" == *":/lib:"* ]]; then
    export LD_LIBRARY_PATH=${T2KREWEIGHT}/lib:${LD_LIBRARY_PATH}
  fi
fi

if [[ "0" != "0" ]]; then
  echo "[INFO]: Sourcing GiBUU tools."
  source /home/stowell/t2krep/NIWG/nuisance/branches/master_refactor/master/builds/default/GiBUUTools/src/GiBUUTools-build/Linux/setup.sh
fi

source $GENIE/genie_prereq.sh

source $GENIE/genie_setup.sh





