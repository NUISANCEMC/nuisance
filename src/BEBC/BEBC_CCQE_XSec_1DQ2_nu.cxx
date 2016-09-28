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

#include "BEBC_CCQE_XSec_1DQ2_nu.h"

//********************************************************************
/// @brief BEBC CCQE Q2 Measurement on Free Nucleons (Ref: Nuc.Phys.B.343 285)
///
/// @details Q2 Extracted assuming numu CCQE scattering of free nucleons.
BEBC_CCQE_XSec_1DQ2_nu::BEBC_CCQE_XSec_1DQ2_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************  

  // Measurement Details                        
  fName = name;
  EnuMin = 0.;
  EnuMax = 200.;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  isDiag = true;
  isRawEvents = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  // In future read most of these from a card file
  this->SetDataFromDatabase("BEBC/BEBC_CCQE_Data_NPB343_285.root", "BEBC_XSec_1DQ2_Data");  
  this->SetupDefaultHist();

  if (applyQ2correction){
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_XSec_1DQ2_Correction");
    this->fMCHist_NoCorr = (TH1D*) this->fMCHist->Clone();
    this->fMCHist_NoCorr->SetNameTitle( (this->fName + "_NOCORR").c_str(),(this->fName + "_NOCORR").c_str());
  }

  
  // Setup Covariance
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Generate events on H2 to get the normalisation right.
  this->fScaleFactor = (this->eventHist->Integral("width")/(nevents+0.))*1E-38 / (this->TotalIntegratedFlux("width")); // NEUT

  // Set starting scale factor
  scaleF = -1.0;
  
};


//********************************************************************
/// @details Extract q2qe from event assuming quasi-elastic scattering
void BEBC_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************   

  // Fill histogram with reconstructed Q2 Distribution
  q2qe = 0.0;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    q2qe        = FitUtils::Q2QErec( (event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    
    break;  
  }
  
  fXVar = q2qe;
  return;
};

//********************************************************************
/// @brief Signal is defined as True CCQE numu scattering
/// @details cut 1: numu event
/// @details cut 2: EnuMin < Enu < EnuMax
/// @details cut 3: Q2 non-zero
bool BEBC_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event){
//********************************************************************

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;

  // Q2 cut
  if (q2qe <= 0) return false;

  return true;
};

//********************************************************************
/// @details Reset the histogram uncorrect
void BEBC_CCQE_XSec_1DQ2_nu::ResetAll(){
//********************************************************************

  Measurement1D::ResetAll();
  this->fMCHist->Reset();

  if (applyQ2correction)
    this->fMCHist_NoCorr->Reset();


}

//******************************************************************** 
/// @details Apply additional event weights for free nucleon measurements
void BEBC_CCQE_XSec_1DQ2_nu::FillHistograms(){
//******************************************************************** 

  
  if (applyQ2correction){
    this->fMCHist_NoCorr->Fill(fXVar, Weight);

    if (fXVar < 0.225)
      this->Weight *= this->CorrectionHist->Interpolate(fXVar);
  }

  Measurement1D::FillHistograms();

}

//******************************************************************** 
void BEBC_CCQE_XSec_1DQ2_nu::ScaleEvents(){
//******************************************************************** 

  Measurement1D::ScaleEvents();
  if (applyQ2correction) this->fMCHist_NoCorr->Scale(this->fScaleFactor, "width");

  return;
}

//******************************************************************** 
void BEBC_CCQE_XSec_1DQ2_nu::ApplyNormScale(double norm){
//********************************************************************

  Measurement1D::ApplyNormScale(norm);
  if (norm == 0.0) scaleF = 0.0;
  else scaleF = 1.0/norm;
  
  if (applyQ2correction) this->fMCHist_NoCorr->Scale(scaleF);

  return;
}
  
//********************************************************************    
/// @brief Include Q2 Correction plots into data write
void BEBC_CCQE_XSec_1DQ2_nu::Write(std::string drawOpt){
//********************************************************************     

  Measurement1D::Write(drawOpt);

  if (applyQ2correction){
    this->CorrectionHist->Write();
    this->fMCHist_NoCorr->Write();
  }
    

  return;
}
