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

//******************************************************************** 
/// @brief ANL CCQE Enu Measurement on Free Nucleons (Ref:PRD16 3103)            
///        
/// @details Enu Extracted assuming numu CCQE scattering of free nucleons.   
ANL_CCQE_XSec_1DEnu_nu::ANL_CCQE_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 


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
    this->SetDataFromDatabase("ANL/ANL_CCQE_Data_PRL31_844.root", "ANL_1DEnu_Data");
    EnuMax = 3.0; // Move EnuMax down
  } else { 
    this->SetDataFromDatabase("ANL/ANL_CCQE_Data_PRD16_3103.root", "ANL_1DEnu_fluxtuned_Data");
  }
  LOG(SAM) << "SETTING UP DEFAULT"<<std::endl;
  this->SetupDefaultHist();

  if (applyQ2correction){
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
  }

  // Setup Covariance
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);
  
  // Different generators require slightly different rescaling factors.
  this->scaleFactor = (this->eventHist->Integral("width")*1E-38/(nevents+0.)); // NEUT

};



//******************************************************************** 
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void ANL_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;
    
    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    q2qe        = FitUtils::Q2QErec( (event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);

    // Once lepton is found, don't continue the loop
    break;  
  }
  
  this->X_VAR = Enu_rec;
  this->Y_VAR = q2qe;
  return;
};

//******************************************************************** 
/// @details Signal is true CCQE scattering
///
/// @details Cut 1: numu event
/// @details Cut 2: Mode == 1
/// @details Cut 3: EnuMin < Enu < EnuMax
bool ANL_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event){
//******************************************************************** 

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  if (event->Mode != 1) return false;
  
  // Restrict energy range
  if (event->Enu()/1000.0 < this->EnuMin || event->Enu()/1000.0 > this->EnuMax) return false;

  return true;
};

//********************************************************************         
/// @details Apply additional event weights for free nucleon measurements      
void ANL_CCQE_XSec_1DEnu_nu::FillHistograms(){
//********************************************************************         

  if (applyQ2correction){
    if (this->Y_VAR < this->CorrectionHist->GetXaxis()->GetXmin())
      this->Weight *= this->CorrectionHist->Interpolate(this->Y_VAR);
  }

  Measurement1D::FillHistograms();

}

//******************************************************************** 
/// @details Perform flux unfolding
void ANL_CCQE_XSec_1DEnu_nu::ScaleEvents(){
//********************************************************************   

  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
};
