/// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "MiniBooNE_CCQE_XSec_2DTcos_nu.h"

//******************************************************************** 
MiniBooNE_CCQE_XSec_2DTcos_nu::MiniBooNE_CCQE_XSec_2DTcos_nu(std::string name, std::string inputfile,
							     FitWeight *rw, std::string type,
							     std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details
  fName = name;
  EnuMin = 0.;
  EnuMax = 3.;
  fIsDiag = true;
  fNormError = 0.107;
  fDefaultTypes = "FIX/DIAG";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG/NORM";
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  fPlotTitles = "; T_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dT_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";
  ccqelike = name.find("CCQELike") != std::string::npos;

  // Define Bin Edges
  fNDataPointsX = 19;
  fNDataPointsY = 21;
  Double_t tempx[19] = { 0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
  Double_t tempy[21] = {-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  fXBins = tempx;
  fYBins = tempy;

  // Setup Data Plots
  if (!ccqelike){
    SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_con.txt", 1E-41, 
		  FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_err.txt", 1E-42);
  } else {
     SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_like.txt", 1E-41, 
		   FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_err.txt",  1E-42);
  }
  SetupDefaultHist();
  
  // Setup Covariances
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);
  fIsDiag = true;

   // Different generators require slightly different rescaling factors.
  fScaleFactor = (fEventHist->Integral("width")*1E-38/(fNEvents+0.))*14.08/6./TotalIntegratedFlux(); 
  
};

//******************************************************************** 
void  MiniBooNE_CCQE_XSec_2DTcos_nu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  if (event->NumFSParticle(13) == 0 &&
      event->NumFSParticle(-13)== 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  // The highest momentum mu+/mu-. The isSignal definition should make sure we only
  // accept events we want, so no need to do an additional check here.  
  int pdgs[] = {13, -13};
  TLorentzVector Pmu = event->GetHMFSParticle(GeneralUtils::makeVector(pdgs))->fP;

  // Now find the kinematic values and fill the histogram
  Ekmu     = Pmu.E()/1000.0 - PhysConst::mass_muon;
  costheta = cos(Pnu.Vect().Angle(Pmu.Vect()));

  // Set X and Y Variables
  fXVar = Ekmu;
  fYVar = costheta;

  return;
};

//******************************************************************** 
bool MiniBooNE_CCQE_XSec_2DTcos_nu::isSignal(FitEvent *event){
//******************************************************************** 

  // If CC0pi, include both charges
  if (ccqelike) {
    if (SignalDef::isCC0pi(event, 14, EnuMin, EnuMax) ||
        SignalDef::isCC0pi(event, -14, EnuMin, EnuMax))
      return true;
  } else {
    if (SignalDef::isCCQELike(event, 14, EnuMin, EnuMax))
      return true;
  }

  return false;

};

