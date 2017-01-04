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

#include "BNL_CCQE_XSec_1DEnu_nu.h"

//********************************************************************
/// @brief BNL CCQE Enu Measurement on Free Nucleons (Ref:PRD23 2499)
///
/// @details Enu Extracted assuming numu CCQE scattering of free nucleons.  
//******************************************************************** 
BNL_CCQE_XSec_1DEnu_nu::BNL_CCQE_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details
  fName = "BNL_CCQE_XSec_1DEnu_nu";
  EnuMin = 0.;
  EnuMax = 6.;
  fIsDiag = true;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  SetupMeasurement(inputfile, type, rw, fakeDataFile);
  
  // Setup Plots
  this->SetDataFromDatabase("BNL/BNL_Data_PRD23_2499.root", "BNL_1DEnu_Data");
  this->SetupDefaultHist();

  // Get Correction Hist
  if (applyQ2correction){
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(GeneralUtils::GetTopLevelDir() + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
  }

  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = (this->fEventHist->Integral("width")*(2.0/1.0)*1E-38/(fNEvents+0.)); 
    
};


//********************************************************************
///@details Fill Enu for the event
void BNL_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event){
//******************************************************************** 

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  Enu_rec = FitUtils::EnuQErec(Pmu, cos(ThetaMu), 0.,true);
  
  fXVar = Enu_rec;
  return;
};

//********************************************************************
bool BNL_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event){
//******************************************************************** 
  return SignalDef::isCCQE(event, 14, EnuMin, EnuMax);
};

//********************************************************************   
/// @details Apply Q2 scaling to weight if required    
void BNL_CCQE_XSec_1DEnu_nu::FillHistograms(){
//********************************************************************        
    
  if (applyQ2correction){
    this->Weight *= this->CorrectionHist->Interpolate(q2qe);
  }

  Measurement1D::FillHistograms();
}

