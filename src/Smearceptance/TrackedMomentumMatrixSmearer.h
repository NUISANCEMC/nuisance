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

#ifndef TRACKEDMOMENTUMMATRIXSMEARER_HXX_SEEN
#define TRACKEDMOMENTUMMATRIXSMEARER_HXX_SEEN

#include "ISmearcepter.h"

#include "GaussianSmearer.h"

#include "TRandom3.h"
#include "TH2D.h"
#include "TH1D.h"

#include <vector>
#include <utility>

// #define DEBUG_MATSMEAR

class TrackedMomentumMatrixSmearer : public ISmearcepter {
 public:
  enum DependVar { kMomentum, kKE, kTE, kNoVar };

 private:
  class SmearMap {
    /// Input True -> Reco mapping.
    std::vector<std::pair<std::pair<double, double>, TH1D *> > RecoSlices;

   public:
    TH1D const *GetRecoSlice(double val);
    void SetSlicesFromMap(TH2D *, bool TruthIsY);
    /// Particle variable to smear: Momentum/KE
    ///
    /// In the future should be able to smear multi-kinematic property
    /// distributions, but requires the definition of axis mappings that I
    /// cannot be bothered with at this second.
    TrackedMomentumMatrixSmearer::DependVar SmearVar;

    double UnitsScale;
  };
  std::map<int, SmearMap> ParticleMappings;

  GaussianSmearer SlaveGS;

  void SpecifcSetup(nuiskey &);

 public:
  /// Will reject any particle that is not known about.
  RecoInfo *Smearcept(FitEvent *);
  /// Helper method for using this class as a component in a more complex
  /// smearer
  void SmearRecoInfo(RecoInfo *);
  ~TrackedMomentumMatrixSmearer();
};

#endif
