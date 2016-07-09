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

#include "ANL_CCQE_Evt_1DQ2_nu.h"

//********************************************************************
/// @brief ANL CCQE Q2 Measurement on Free Nucleons (Ref: PRD16 3103)
///
/// @details Q2 Extracted assuming numu CCQE scattering of free nucleons.
ANL_CCQE_Evt_1DQ2_nu::ANL_CCQE_Evt_1DQ2_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************  

  // Measurement Details                        
  measurementName = name;
  EnuMin = 0.;
  EnuMax = 6.;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  applyEnucorrection = type.find("ENUCORR") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  isDiag = true;
  isRawEvents = true;

  // In future read most of these from a card file
  if (!name.compare("ANL_CCQE_Evt_1DQ2_nu_PRL31")){

    this->SetDataFromDatabase("ANL/ANL_CCQE_Data_PRL31_844.root", "ANL_1DQ2_Data");
    applyEnucorrection = false;
    EnuMax = 3.0; // Move EnuMax down 

  } else if (!name.compare("ANL_CCQE_Evt_1DQ2_nu_PRD16")){

    applyEnucorrection = false;
    this->SetDataFromDatabase("ANL/ANL_CCQE_Data_PRD16_3103.root", "ANL_1DQ2_Data");

  } else {

    this->SetDataFromDatabase("ANL/ANL_Data_PRD26_537.root","ANL_1DQ2_Data");

  } 

  // Setup Histograms
  this->SetupDefaultHist();

  if (applyQ2correction){
    this->CorrectionHist = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
    this->mcHist_NoCorr = (TH1D*) this->mcHist->Clone();
    this->mcHist_NoCorr->SetNameTitle( (this->measurementName + "_NOCORR").c_str(),(this->measurementName + "_NOCORR").c_str());
  }

  if (applyEnucorrection){
    this->EnuRatePlot = PlotUtils::GetTH1DFromFile(std::string(std::getenv("EXT_FIT")) + "/data/ANL/ANL_Data_PRD26_537.root","ANL_1DEnu_Rate");
    this->EnuvsQ2Plot = PlotUtils::MergeIntoTH2D(dataHist, EnuRatePlot, "Events");

    this->EnuFluxUnfoldPlot = (TH1D*)this->EnuRatePlot->Clone();
    for (int i = 0; i < this->EnuFluxUnfoldPlot->GetNbinsX(); i++) this->EnuFluxUnfoldPlot->SetBinContent(i+1,1.0);
    PlotUtils::FluxUnfoldedScaling(EnuFluxUnfoldPlot, fluxHist);
  }
  
  // Setup Covariance
  //  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  //  covar     = StatUtils::GetInvert(fullcovar);

  //  this->eventHist->Scale(dataHist->Integral()/eventHist->Integral());
  this->scaleFactor = (this->dataHist->Integral("width")/(nevents+0.)); 

  // Set starting scale factor
  scaleF = -1.0;
  
};


//********************************************************************
/// @details Extract q2qe from event assuming quasi-elastic scattering
void ANL_CCQE_Evt_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************   

  // Fill histogram with reconstructed Q2 Distribution
  q2qe = 0.0;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    q2qe        = FitUtils::Q2QErec( (event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    
    break;  
  }
  
  this->X_VAR = q2qe;
  return;
};

//********************************************************************
/// @brief Signal is defined as True CCQE numu scattering
/// @details cut 1: CCQE event
/// @details cut 2: numu event
/// @details cut 3: EnuMin < Enu < EnuMax
/// @details cut 4: Q2 non-zero
bool ANL_CCQE_Evt_1DQ2_nu::isSignal(FitEvent *event){
//********************************************************************

  // Only look at CCQE Events                                                                                                                                                                        
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
/// @details Reset the histogram uncorrect
void ANL_CCQE_Evt_1DQ2_nu::ResetAll(){
//********************************************************************

  Measurement1D::ResetAll();
  this->mcHist->Reset();

  if (applyEnucorrection)
    this->EnuvsQ2Plot->Reset();

  if (applyQ2correction)
    this->mcHist_NoCorr->Reset();


}

//******************************************************************** 
/// @details Apply additional event weights for free nucleon measurements
void ANL_CCQE_Evt_1DQ2_nu::FillHistograms(){
//******************************************************************** 

  if (applyEnucorrection)
    this->EnuvsQ2Plot->Fill(X_VAR, Enu, Weight);
  
  if (applyQ2correction){
    this->mcHist_NoCorr->Fill(X_VAR, Weight);

    if (X_VAR < 0.225)
      this->Weight *= this->CorrectionHist->Interpolate(X_VAR);
  }

  Measurement1D::FillHistograms();

}

//******************************************************************** 
void ANL_CCQE_Evt_1DQ2_nu::ScaleEvents(){
//******************************************************************** 

  if (applyEnucorrection){
    this->EnuvsQ2Plot->Scale(eventHist->Integral()/(nevents+0.));
    for (int j = 0; j < EnuvsQ2Plot->GetNbinsY(); j++){
      for (int i = 0; i < EnuvsQ2Plot->GetNbinsX(); i++){
	this->EnuvsQ2Plot->SetBinContent(i+1,j+1, this->EnuvsQ2Plot->GetBinContent(i+1,j+1) * EnuFluxUnfoldPlot->GetBinContent(j+1));
      }
    }
  }


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
  



}


//********************************************************************    
/// @brief Include Q2 Correction plots into data write
void ANL_CCQE_Evt_1DQ2_nu::Write(std::string drawOpt){
//********************************************************************     

  Measurement1D::Write(drawOpt);

  if (applyQ2correction){
    this->CorrectionHist->Write();
    this->mcHist_NoCorr->Write();
  }

  if (applyEnucorrection){
    this->EnuvsQ2Plot->Write();
    ((TH1D*)this->EnuvsQ2Plot->ProjectionX())->Write();
    ((TH1D*)this->EnuvsQ2Plot->ProjectionY())->Write();
    EnuFluxUnfoldPlot->Write();
    EnuRatePlot->Write();
  }

  return;
}
