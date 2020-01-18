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

#ifndef SigmaEnuHists_H_SEEN
#define SigmaEnuHists_H_SEEN
#include "FitEvent.h"
#include "Measurement1D.h"

class SigmaEnuHists : public Measurement1D {

public:
  SigmaEnuHists(std::string name, std::string inputfile, FitWeight *rw,
                std::string type, std::string fakeDataFile);
  virtual ~SigmaEnuHists(){};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);
  double GetLikelihood() { return 0; }

private:
  std::map<int, TH1D *> NEUTModeHists;
  std::map<int, TH1D *> GENIEModeHists;

  enum Topologies {
    kCC = 0,
    kCC0Pi,
    kCC1Pi,
    kCC1Pip,
    kCC1Pi0,
    kCC1Pim,
    kCCNPi,
    kNC,
    kNC0Pi,
    kNC1Pi,
    kNCNPi,
    kNTopologies
  };
  std::map<int, std::string> TopologyNames;
  std::map<int, TH1D *> TopologyHists;

  bool PerE;
};

#endif
