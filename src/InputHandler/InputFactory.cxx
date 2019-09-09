// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "GENIEInputHandler.h"
#include "GIBUUInputHandler.h"
#include "HistogramInputHandler.h"
#include "NEUTInputHandler.h"
#include "NUANCEInputHandler.h"
#include "NuWroInputHandler.h"
#include "SigmaQ0HistogramInputHandler.h"
#include "SplineInputHandler.h"

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
#ifdef __NEUT_ENABLED__
    input = new NEUTInputHandler(handle, newinputs);
#else
    QERROR(FTL, "Tried to create NEUTInputHandler : " << handle << " " << inpType
                                                     << " " << inputs);
    QTHROW("NEUT is not enabled!");
#endif
    break;

  case (kGENIE_Input):
#ifdef __GENIE_ENABLED__
    input = new GENIEInputHandler(handle, newinputs);
#else
    QERROR(FTL, "Tried to create GENIEInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    QTHROW("GENIE is not enabled!");
#endif
    break;

  case (kNUWRO_Input):
#ifdef __NUWRO_ENABLED__
    input = new NuWroInputHandler(handle, newinputs);
#else
    QERROR(FTL, "Tried to create NuWroInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    QTHROW("NuWro is not enabled!");
#endif
    break;

  case (kGiBUU_Input):
#ifdef __GiBUU_ENABLED__
    input = new GIBUUInputHandler(handle, newinputs);
#else
    QERROR(FTL, "Tried to create GiBUUInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    QTHROW("GiBUU is not enabled!");
#endif
    break;

  case (kNUANCE_Input):
#ifdef __NUANCE_ENABLED__
    input = new NUANCEInputHandler(handle, newinputs);
#else
    QERROR(FTL, "Tried to create NUANCEInputHandler : "
                   << handle << " " << inpType << " " << inputs);
    QTHROW("NUANCE is not enabled!");
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
    QTHROW("Input handler creation failed!" << std::endl
                                            << "Generator Type " << inpType
                                            << " not enabled!");
  }

  return input;
};
} // namespace InputUtils
