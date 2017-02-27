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

#include "BNL_CCQE_Evt_1DQ2_nu.h"

//********************************************************************
/// @brief BNL CCQE Enu Measurement on Free Nucleons (Ref:PRD23 2499)
///
/// @details Enu Extracted assuming numu CCQE scattering of free nucleons.
//********************************************************************
BNL_CCQE_Evt_1DQ2_nu::BNL_CCQE_Evt_1DQ2_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************

  // Measurement Details
  fName = "BNL_CCQE_Evt_1DQ2_nu";
  EnuMin = 0.;
  EnuMax = 10.;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // override input options
  fIsDiag = true;
  fIsRawEvents =true;
  EnuVsQ2 = new TH2D("EnuVsQ2","EnuVsQ2",25,0.0,10.0,60,0.0,3.0);

  //Setup Plots
  this->SetDataFromDatabase("BNL/BNL_Data_PRD23_2499.root", "BNL_1DQ2_Data");
  this->SetupDefaultHist();

  // Get correction hist
  if (applyQ2correction){
    LOG(SAM) <<"Retrieving Q2 Correction"<<std::endl;
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(GeneralUtils::GetTopLevelDir() + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");

    LOG(SAM) << "Creating fMCHist NoCORR"<<std::endl;
    this->fMCHist_NoCorr = (TH1D*) this->fMCHist->Clone();
    this->fMCHist_NoCorr->SetNameTitle( (this->fName + "_NOCORR").c_str(),(this->fName + "_NOCORR").c_str());
  }


  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  LOG(SAM)<<"Setting up scaling"<<std::endl;
  // Setup Scaling
  //  GetEventHistogram()->Scale(this->fDataHist->Integral()/GetEventHistogram()->Integral());

  // Different generators require slightly different rescaling factors.
  this->fScaleFactor = (GetEventHistogram()->Integral()/(fNEvents+0.));
  //  scaleF = -1.0;
};

//********************************************************************
/// @details Reset the histogram uncorrect
void BNL_CCQE_Evt_1DQ2_nu::ResetAll(){
//********************************************************************
  Measurement1D::ResetAll();
  this->fMCHist->Reset();
  this->fMCFine->Reset();
  EnuVsQ2->Reset();

  if (applyQ2correction)
    this->fMCHist_NoCorr->Reset();

}



//********************************************************************
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void BNL_CCQE_Evt_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  q2qe = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0.,true);

  fXVar = q2qe;
  return;
};

//********************************************************************
bool BNL_CCQE_Evt_1DQ2_nu::isSignal(FitEvent *event){
//********************************************************************
  if (!SignalDef::isCCQE(event, 14, EnuMin, EnuMax)) return false;
  if (q2qe <= 0) return false;
  return true;
};



//********************************************************************
/// @details Apply Q2 scaling to weight if required
void BNL_CCQE_Evt_1DQ2_nu::FillHistograms(){
//********************************************************************

  if (Signal){
    if (applyQ2correction){
      this->fMCHist_NoCorr->Fill(fXVar,Weight);


      if (fXVar < 0.225)
	this->Weight *= this->CorrectionHist->Interpolate(fXVar);
    }

    EnuVsQ2->Fill(Enu,fXVar, Weight);
  }
  if (Signal)
    Measurement1D::FillHistograms();

  LOG(DEB) <<"fXVar = "<<fXVar<<" "<<Weight<<std::endl;
  return;
}



//********************************************************************
/// @details Apply scaling to uncorrected fMCHist_NoCorr and scale to match data
void BNL_CCQE_Evt_1DQ2_nu::ScaleEvents(){
  //********************************************************************

  this->fMCHist->Scale(fScaleFactor);
  this->fMCFine->Scale(fScaleFactor);
  if (applyQ2correction) this->fMCHist_NoCorr->Scale(fScaleFactor);

  // Scale to match data
  scaleF = PlotUtils::GetDataMCRatio(fDataHist, fMCHist, fMaskHist);

  this->fMCHist->Scale(scaleF);
  this->fMCFine->Scale(scaleF);

  if (applyQ2correction){
    scaleF = PlotUtils::GetDataMCRatio(fDataHist, fMCHist_NoCorr, fMaskHist);
    this->fMCHist_NoCorr->Scale(scaleF);
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
    this->fMCHist_NoCorr->Write();
  }


  return;
}


