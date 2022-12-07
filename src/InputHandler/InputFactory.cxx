// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
 *    This file is part of NUISANCE.
 *
 *    NUISANCE is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    NUISANCE is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include "FitEventInputHandler.h"
#include "GIBUUInputHandler.h"
#include "GiBUUNativeInputHandler.h"
#include "HistogramInputHandler.h"
#include "NUANCEInputHandler.h"
#include "SigmaQ0HistogramInputHandler.h"
#include "SplineInputHandler.h"

#ifdef HepMC3_ENABLED
#include "NuHepMCInputHandler.h"
#endif

#ifdef GENIE_ENABLED
#include "GENIEInputHandler.h"
#endif

#ifdef NEUT_ENABLED
#include "NEUTInputHandler.h"
#endif

#ifdef NuWro_ENABLED
#include "NuWroInputHandler.h"
#endif

#include "InputFactory.h"

#include "TFile.h"

namespace InputUtils {

InputHandlerBase *CreateInputHandler(std::string const &handle,
                                     InputUtils::InputType inpType,
                                     std::string const &inputs) {
  InputHandlerBase *input = NULL;
  std::string newinputs = InputUtils::ExpandInputDirectories(inputs);

  switch (inpType) {
  case (kNEUT_Input):
#ifdef NEUT_ENABLED
    input = new NEUTInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create NEUTInputHandler : " << handle << " " << inpType
                                                     << " " << inputs);
    NUIS_ABORT("NEUT is not enabled!");
#endif
    break;

  case (kGENIE_Input):
#ifdef GENIE_ENABLED
    input = new GENIEInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create GENIEInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    NUIS_ABORT("GENIE is not enabled!");
#endif
    break;

  case (kNuWro_Input):
#ifdef NuWro_ENABLED
    input = new NuWroInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create NuWroInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    NUIS_ABORT("NuWro is not enabled!");
#endif
    break;

  case (kGiBUU_Input):
#ifdef GiBUU_ENABLED
    input = new GiBUUNativeInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create GiBUUInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    NUIS_ABORT("GiBUU is not enabled!");
#endif
    break;

  case (kNUANCE_Input):
#ifdef NUANCE_ENABLED
    input = new NUANCEInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create NUANCEInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    NUIS_ABORT("NUANCE is not enabled!");
#endif
    break;
  case (kNuHepMC_Input):
#ifdef HepMC3_ENABLED
    input = new NuHepMCInputHandler(handle, newinputs);
#else
    NUIS_ERR(FTL, "Tried to create NuHepMCInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    NUIS_ABORT("NuHepMCInputHandler is not enabled!");
#endif
    break;

  case (kFEVENT_Input):
    input = new FitEventInputHandler(handle, newinputs);
    break;

  case (kEVSPLN_Input):
    input = new SplineInputHandler(handle, newinputs);
    break;

  case (kSIGMAQ0HIST_Input):
    input = new SigmaQ0HistogramInputHandler(handle, newinputs);
    break;

  case (kHISTO_Input):
    input = new HistoInputHandler(handle, newinputs);
    break;

  default:
    break;
  }

  /// Input failed
  if (!input) {
    NUIS_ABORT("Input handler creation failed!" << std::endl
                                            << "Generator Type " << inpType
                                            << " not enabled!");
  }

  return input;
};
} // namespace InputUtils
