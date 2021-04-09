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

#ifndef ISMEARCEPTER_HXX_SEEN
#define ISMEARCEPTER_HXX_SEEN

#include "FitEvent.h"
#include "NuisKey.h"

#include "TVector3.h"

#include <string>
#include <vector>

/// Base reconstructed information that a smearcepter should fill.
/// Smearcepters may allocate and return instances of RecoInfo subclasses.
struct RecoInfo {
  RecoInfo()
      : RecObjMom(),
        RecObjClass(),
        RecVisibleEnergy(0),
        TrueContribPDGs(),
        Weight(1){};
  /// Reconstructed 3-momentum
  std::vector<TVector3> RecObjMom;
  ///\brief 'Class' of a reconstructed object. Might be a PDG particle code, or
  /// some smearer-defined classification like MIP track/EMShower/...
  std::vector<int> RecObjClass;

  /// The visible energy not left by fully reconstructed tracks
  std::vector<double> RecVisibleEnergy;

  /// The true pdgs of particles that contributed to the visible energy
  std::vector<int> TrueContribPDGs;

  double Weight;
};

class ISmearcepter {
 protected:
  std::string ElementName;
  std::string InstanceName;

 public:
  void Setup(nuiskey &);
  virtual void SpecifcSetup(nuiskey &) = 0;

  std::string GetName() { return InstanceName; }
  std::string GetElementName() { return ElementName; }

  virtual RecoInfo *Smearcept(FitEvent *) = 0;
  /// Helper method for using this class as a component in a more complex
  /// smearer
  virtual void SmearRecoInfo(RecoInfo *) {
    NUIS_ABORT("Smearcepter: " << ElementName
                          << " doesn't implement SmearRecoInfo.");
    ;
  }
};

template <typename T>
ISmearcepter* BuildSmearcepter(nuiskey& nk) {
  ISmearcepter* rtn = new T();
  rtn->Setup(nk);
  return rtn;
}

typedef ISmearcepter* (*SmearceptionFactory_fcn)(nuiskey&);


#endif
