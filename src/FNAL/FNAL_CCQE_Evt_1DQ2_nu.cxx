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

#include "FNAL_CCQE_Evt_1DQ2_nu.h"

//********************************************************************
/// @brief FNAL CCQE Q2 Measurement on Free Nucleons (Ref: PRD16 3103)
///
/// @details Q2 Extracted assuming numu CCQE scattering of free nucleons.
FNAL_CCQE_Evt_1DQ2_nu::FNAL_CCQE_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw,
					     std::string type, std::string fakeDataFile){
//********************************************************************  

  // Measurement Details                        
  fName = "FNAL_CCQE_Evt_1DQ2_nu";
  EnuMin = 0.;
  EnuMax = 200.;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  fIsDiag = true;
  fIsRawEvents = true;

  this->SetDataFromDatabase("FNAL/FNAL_CCQE_Data_PRD29_436.root", "FNAL_CCQE_Data_1DQ2");

  this->SetupDefaultHist();

  if (applyQ2correction){
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(GeneralUtils::GetTopLevelDir() + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
    this->fMCHist_NoCorr = (TH1D*) this->fMCHist->Clone();
    this->fMCHist_NoCorr->SetNameTitle( (this->fName + "_NOCORR").c_str(),(this->fName + "_NOCORR").c_str());
  }

  // Mask out the first bin if required
  this->SetBinMask(GeneralUtils::GetTopLevelDir() + "/data/FNAL/FNAL_CCQE_BinMask_PRD29_436.dat");
  
  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Different generators require slightly different rescaling factors.
  this->fScaleFactor = (this->fEventHist->Integral()/(fNEvents+0.)); // NEUT

  // Set starting scale factor
  scaleF = -1.0;
  
};


//********************************************************************
/// @details Extract q2qe from event assuming quasi-elastic scattering
void FNAL_CCQE_Evt_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************   

  // Fill histogram with reconstructed Q2 Distribution
  q2qe = 0.0;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  q2qe = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0.,true);
  
  fXVar = q2qe;
  return;
};

//********************************************************************
bool FNAL_CCQE_Evt_1DQ2_nu::isSignal(FitEvent *event){
//********************************************************************
  if (!SignalDef::isCCQE(event, 14, EnuMin, EnuMax)) return false;
  if (q2qe <= 0) return false;
  return true;
};

//********************************************************************
/// @details Reset the histogram uncorrect
void FNAL_CCQE_Evt_1DQ2_nu::ResetAll(){
//********************************************************************

  Measurement1D::ResetAll();
  this->fMCHist->Reset();

  if (applyQ2correction)
    this->fMCHist_NoCorr->Reset();


}

//******************************************************************** 
/// @details Apply additional event weights for free nucleon measurements
void FNAL_CCQE_Evt_1DQ2_nu::FillHistograms(){
//******************************************************************** 

  
  if (applyQ2correction){
    this->fMCHist_NoCorr->Fill(fXVar, Weight);

    if (fXVar < 0.225)
      this->Weight *= this->CorrectionHist->Interpolate(fXVar);
  }

  Measurement1D::FillHistograms();

}

//******************************************************************** 
void FNAL_CCQE_Evt_1DQ2_nu::ScaleEvents(){
//******************************************************************** 

  this->fMCHist->Scale(fScaleFactor);
  if (applyQ2correction) this->fMCHist_NoCorr->Scale(fScaleFactor);


  // Scale to match data
  scaleF = PlotUtils::GetDataMCRatio(fDataHist, fMCHist, fMaskHist);
  
  this->fMCHist->Scale(scaleF);
  this->fMCFine->Scale(scaleF);

  if (applyQ2correction){
    scaleF = PlotUtils::GetDataMCRatio(fDataHist, fMCHist_NoCorr, fMaskHist);
    this->fMCHist_NoCorr->Scale(scaleF);
  }

}


//********************************************************************    
/// @brief Include Q2 Correction plots into data write
void FNAL_CCQE_Evt_1DQ2_nu::Write(std::string drawOpt){
//********************************************************************     

  Measurement1D::Write(drawOpt);

  if (applyQ2correction){
    this->CorrectionHist->Write();
    this->fMCHist_NoCorr->Write();
  }
    

  return;
}
