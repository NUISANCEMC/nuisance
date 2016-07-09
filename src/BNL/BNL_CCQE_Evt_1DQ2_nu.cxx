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

#include "BNL_CCQE_Evt_1DQ2_nu.h"

//********************************************************************
/// @brief BNL CCQE Enu Measurement on Free Nucleons (Ref:PRD23 2499)
///
/// @details Enu Extracted assuming numu CCQE scattering of free nucleons.  
//********************************************************************
BNL_CCQE_Evt_1DQ2_nu::BNL_CCQE_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************  

  // Measurement Details
  measurementName = "BNL_CCQE_Evt_1DQ2_nu";
  EnuMin = 0.;
  EnuMax = 10.;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  
  // override input options
  isDiag = true;
  isRawEvents =true;
  EnuVsQ2 = new TH2D("EnuVsQ2","EnuVsQ2",25,0.0,10.0,60,0.0,3.0);

  //Setup Plots
  this->SetDataFromDatabase("BNL/BNL_Data_PRD23_2499.root", "BNL_1DQ2_Data");
  this->SetupDefaultHist();

  // Get correction hist
  if (applyQ2correction){
    LOG(SAM) <<"Retrieving Q2 Correction"<<std::endl;
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");

    LOG(SAM) << "Creating mcHist NoCORR"<<std::endl;
    this->mcHist_NoCorr = (TH1D*) this->mcHist->Clone();
    this->mcHist_NoCorr->SetNameTitle( (this->measurementName + "_NOCORR").c_str(),(this->measurementName + "_NOCORR").c_str());
  }

  
  // Setup Covariance
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  LOG(SAM)<<"Setting up scaling"<<std::endl;
  // Setup Scaling
  this->eventHist->Scale(this->dataHist->Integral()/this->eventHist->Integral());
  
  // Different generators require slightly different rescaling factors.
  this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); 
  scaleF = -1.0;
};

//********************************************************************                                                                                                                                                                     
/// @details Reset the histogram uncorrect                                                                                                                                                                                                  
void BNL_CCQE_Evt_1DQ2_nu::ResetAll(){
//********************************************************************                                                                                                                                                                       
  Measurement1D::ResetAll();
  this->mcHist->Reset();
  this->mcFine->Reset();
  EnuVsQ2->Reset();

  if (applyQ2correction)
    this->mcHist_NoCorr->Reset();

}



//********************************************************************
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering    
void BNL_CCQE_Evt_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************   

  q2qe = -0.1;
  X_VAR = q2qe;

  // Fill histogram with reconstructed Q2 Distribution
  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    q2qe        = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    
    break;  
  }
  

  this->X_VAR = q2qe;
  return;
};

//********************************************************************
/// @details Signal is true CCQE scattering
///
/// @details cut 1: CCQE Event 
/// @details Cut 2: numu event
/// @details cut 3: Q2 > 0.0
/// @details Cut 4: EnuMin < Enu < EnuMax        
bool BNL_CCQE_Evt_1DQ2_nu::isSignal(FitEvent *event){
//********************************************************************

  // Signal definition
  // - True CCQE numu scattering
  /// @details cut 1: numu event 
  if (Mode != 1) return false;

  // Only look at numu events                                                                                                                                                                          
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict energy range                                                                                                                                                                            
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;

  // Q2 cut                                                                                                                                                                                            
  if (q2qe <= 0) return false;

  return true;
};



//********************************************************************                                                                   
/// @details Apply Q2 scaling to weight if required
void BNL_CCQE_Evt_1DQ2_nu::FillHistograms(){
//********************************************************************  

  if (Signal){
    if (applyQ2correction){
      this->mcHist_NoCorr->Fill(X_VAR,Weight);


      if (X_VAR < 0.225)
	this->Weight *= this->CorrectionHist->Interpolate(X_VAR);
    }
    
    EnuVsQ2->Fill(Enu,X_VAR, Weight);
  }
  if (Signal)
    Measurement1D::FillHistograms();

  //  std::cout<<"X_VAR = "<<X_VAR<<" "<<Weight<<std::endl;
  return;
}



//********************************************************************                                                                                                                                                                      
/// @details Apply scaling to uncorrected mcHist_NoCorr and scale to match data
void BNL_CCQE_Evt_1DQ2_nu::ScaleEvents(){
  //********************************************************************                                                                                                                                                                   
  this->mcHist->Scale(scaleFactor);
  this->mcFine->Scale(scaleFactor);
  if (applyQ2correction) this->mcHist_NoCorr->Scale(scaleFactor);

  // Scale to match data
  scaleF = PlotUtils::GetDataMCRatio(dataHist, mcHist, maskHist);
  
  this->mcHist->Scale(scaleF);
  this->mcFine->Scale(scaleF);

  if (applyQ2correction){
    scaleF = PlotUtils::GetDataMCRatio(dataHist, mcHist_NoCorr, maskHist);
    this->mcHist_NoCorr->Scale(scaleF);
  }
  
  return;

}


//********************************************************************                                                                                                                                                                      
/// @brief Include Q2 Correction plots into data write                                                                                                                                                                                      
void BNL_CCQE_Evt_1DQ2_nu::Write(std::string drawOpt){
//********************************************************************                                                                                                                                                                       
  Measurement1D::Write(drawOpt);

  EnuVsQ2->Write();

  if (applyQ2correction){
    this->CorrectionHist->Write();
    this->mcHist_NoCorr->Write();
  }


  return;
}


