// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "ANL_CCQE_XSec_1DEnu_nu.h"

ANL_CCQE_XSec_1DEnu_nu::ANL_CCQE_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  // Measurement Details
  measurementName = name; 
  EnuMin = 0.;
  EnuMax = 6.;
  isDiag = true;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  default_types = "FIX/DIAG";
  allowed_types = "FIX,FREE,SHAPE/DIAG/Q2CORR/ENUCORR";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile); 

  LOG(SAM) << "SETTING DATA"<<std::endl;

  // Setup Plots
  if (!name.compare("ANL_CCQE_XSec_1DEnu_nu_PRL31")){
    SetDataFromDatabase("ANL/ANL_CCQE_Data_PRL31_844.root", "ANL_1DEnu_Data");
    EnuMax = 3.0; // Move EnuMax down
  } else { 
    SetDataFromDatabase("ANL/ANL_CCQE_Data_PRD16_3103.root", "ANL_1DEnu_fluxtuned_Data");
  }

  SetupDefaultHist();

  if (applyQ2correction){
    CorrectionHist = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
  }

  // Setup Covariance
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);
  
  // Different generators require slightly different rescaling factors.
  scaleFactor = (eventHist->Integral("width")*1E-38/(nevents+0.)); // NEUT

};


/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void ANL_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event){
  
  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;
    
    ThetaMu = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    Enu_rec = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    q2qe    = FitUtils::Q2QErec( (event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);

    // Once lepton is found, don't continue the loop
    break;  
  }
  
  X_VAR = Enu_rec;

  // We save q2 into Y_VAR incase correction is applied
  Y_VAR = q2qe;
  return;
};


/// Signal is true CCQE scattering \n
/// \item Cut 1: numu event
/// \item Cut 2: Mode == 1
/// \item Cut 3: EnuMin < Enu < EnuMax
bool ANL_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event){

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) 
    return false;

  // Only CCQE
  if (event->Mode != 1) 
    return false;
  
  // Restrict energy range
  if (event->Enu()/1000.0 < EnuMin || event->Enu()/1000.0 > EnuMax) 
    return false;

  return true;
};


//! If Q2 correction is applied the CorrectionHist is interpolated 
//! using the Q2QE value saved in Y_VAR
void ANL_CCQE_XSec_1DEnu_nu::FillHistograms(){

  if (applyQ2correction){
    if (Y_VAR < CorrectionHist->GetXaxis()->GetXmin())
      Weight *= CorrectionHist->Interpolate(Y_VAR);
  }

  Measurement1D::FillHistograms();
}


