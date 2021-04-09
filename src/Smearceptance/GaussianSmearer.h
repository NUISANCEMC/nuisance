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

#ifndef GAUSSIANSMEARER_HXX_SEEN
#define GAUSSIANSMEARER_HXX_SEEN

#include "ISmearcepter.h"

#include <map>

// #define DEBUG_GAUSSSMEAR

class GaussianSmearer : public ISmearcepter {
 public:
  enum GSmearType { kAbsolute, kFractional, kFunction, kNoType };
  enum DependVar { kMomentum, kKE, kKEVis, kTEVis, kCosTheta, kTheta, kNoVar };

 private:
  struct GSmear {
    GSmearType type;
    DependVar smearVar;
    double width;
    TF1 *func;
  };

  std::map<int, std::vector<GSmear> > TrackedGausSmears;
  std::map<int, GSmear> VisGausSmears;

  TRandom3 rand;

  void SpecifcSetup(nuiskey &);

 public:
  RecoInfo *Smearcept(FitEvent *);

  void SmearceptOneParticle(RecoInfo *ri, FitParticle *fp
#ifdef DEBUG_GAUSSSMEAR
                            ,
                            size_t p_it
#endif
                            );
  /// Helper method for using this class as a component in a more complex
  /// smearer
  void SmearRecoInfo(RecoInfo *);

  void SmearceptOneParticle(TVector3 &RecObjMom, int RecObjClass);

  void SmearceptOneParticle(double &RecVisibleEnergy, int TrueContribPDGs);
};

#endif
