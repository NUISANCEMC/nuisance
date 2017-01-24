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

#include "SciBooNEUtils.h"

double SciBooNEUtils::CalcEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon){

  double eff = 1.;

  if (!effHist) return eff;

  double pmu = muon->fP.Vect().Mag()/1000.;
  double thetamu = nu->fP.Vect().Angle(muon->fP.Vect());
  eff = effHist->GetBinContent(effHist->FindBin(pmu, thetamu));

  return eff;
}



// Function to calculate the distance the particle travels in scintillator
TVector3 SciBooNEUtils::DistanceInScintillator(FitParticle* beam, FitParticle* particle){

  TVector3 vect(0.,0.,0.);

  if (particle->fP.Vect().Mag() > 150.) vect.SetZ(100.);

  double mom = particle->fP.Vect().Mag();
  double zmom = mom*cos(beam->fP.Vect().Angle(particle->fP.Vect()));

  // VERY TERRIBLE APPROXIMATION
  if (fabs(zmom) > 250.) vect.SetZ(100.); // <<< DRAGONS
  // ^^^ HERE BE DRAGONS
  LOG(DEB) << "MOM = " << mom << "; ZMOM = " << zmom << std::endl;
  return vect;
}

// Function to check whether the particle has travelled 8cm in the beam direction (3 layers)
bool SciBooNEUtils::PassesCOHDistanceCut(FitParticle* beam, FitParticle* particle){
  
  TVector3 vect = SciBooNEUtils::DistanceInScintillator(beam, particle);
  
  // False if the z-axis direction too low
  if (vect[2] < 80) return false;
  return true;
  
}

