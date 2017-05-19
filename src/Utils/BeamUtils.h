#ifndef BEAM_UTILS_H
#define BEAM_UTILS_H
#include <vector>
#include "FitLogger.h"
#include "NuisConfig.h"

/// Namespace for any global util functions related to beam simulation
namespace BeamUtils{

  /// @brief Converts comma seperated string of beam ids into PDG vector
  /// 
  /// e.g. 'numu,nue,numub,e,16' = <14,12,-14,11,16>
  std::vector<int> ParseSpeciesToIntVect(std::string spc);
}

#endif
