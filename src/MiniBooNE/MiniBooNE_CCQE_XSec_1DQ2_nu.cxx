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

#include "MiniBooNE_CCQE_XSec_1DQ2_nu.h"

//******************************************************************** 
MiniBooNE_CCQE_XSec_1DQ2_nu::MiniBooNE_CCQE_XSec_1DQ2_nu(std::string name, std::string inputfile,
							 FitWeight *rw, std::string type,
							 std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details                                                                
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";

  /// Using the sample name "MiniBooNE_CCQE_XSec_1DQ2_nu_CCQELike" will allow
  /// the CCQELike sample without background subtraction to be fitted.
  ccqelike = name.find("CCQELike") != std::string::npos;
  EnuMin = 0.;
  EnuMax = 3.;
  normError = 0.107;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ccqe/asqq_con.txt");  
  this->SetupDefaultHist();

  // Setup Covariance 
  if (!this->isDiag) {

    /// Currently has a placeholder for the matrices as work fixing them is ongoing.                   
    this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/ccqe/MiniBooNE_1DQ2_nu.root");
    StatUtils::SetDataErrorFromCov(fDataHist, fullcovar, 1E-38);

  } else {
    /// Assume a diagonal shape-only error is default                                                                                                                  
    fullcovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar     = StatUtils::GetInvert(fullcovar);
  }

  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG Histogram is saved
  if (ccqelike){

    fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile(FitPar::GetDataBase()+"/MiniBooNE/ccqe/asqq_bkg.txt",
						   (this->fName+"_data_CCQELIKE"), this->fPlotTitles);
    
    for (int i = 0; i < fDataHist->GetNbinsX(); i++){
      this->fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1) + fDataHist_CCQELIKE->GetBinContent(i+1));
    }
    
    PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_CCQELIKE);

  }

  // Get Scale Factor
  fScaleFactor = ((fEventHist->Integral("width")*1E-38/(nevents+0.))
		 * (14.08/6.0)
		 / TotalIntegratedFlux());

};

//******************************************************************** 
/// @details Extract q2qe(fXVar) from the event
void  MiniBooNE_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  q2qe = -999.9;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    int PID = abs((event->PartInfo(j))->fPID);

    // Look for the outgoing muon
    if (PID == 13 or (ccqelike and PID == -13)){
    
      // Now find the Q2QE value and fill the histogram
      q2qe = FitUtils::Q2QErec((event->PartInfo(j))->fP, 
			       cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 
			       34., true);
     
      break;
    }
  }

  // Set X Variables
  fXVar = q2qe;
  
  return;
};

//******************************************************************** 
bool MiniBooNE_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event){
//******************************************************************** 

  // 2 Different Signal Definitions
  if (ccqelike) return SignalDef::isMiniBooNE_CCQELike(event, EnuMin, EnuMax);
  else return SignalDef::isMiniBooNE_CCQE(event, EnuMin, EnuMax);

  return true;
};

//******************************************************************** 
/// @details Fills a ccqe-like background plot if required
void MiniBooNE_CCQE_XSec_1DQ2_nu::FillHistograms(){
//******************************************************************** 

  Measurement1D::FillHistograms();

  if (Mode != 1 and Mode != 2 and ccqelike and Signal){
    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, Mode, fXVar, Weight);
  }
}


//********************************************************************   
/// @details Extra write command to save the CCQELike PDG if required
void MiniBooNE_CCQE_XSec_1DQ2_nu::Write(std::string drawOpt){
//********************************************************************   

  Measurement1D::Write(drawOpt);
  
  if (ccqelike){
    fDataHist_CCQELIKE->Write();
    
    THStack combo_fMCHist_CCQELIKE = PlotUtils::GetNeutModeStack((this->fName + "_MC_CCQELIKE").c_str(), (TH1**)this->fMCHist_CCQELIKE, 0);
    combo_fMCHist_CCQELIKE.Write();
  }

}


//********************************************************************   
/// @details Extra scale command for CCQELIKE PDG Hist    
void MiniBooNE_CCQE_XSec_1DQ2_nu::ResetAll(){
//******************************************************************** 
  
  if (ccqelike)
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_CCQELIKE);
  
}

//********************************************************************                                                                                                                                                                    
/// @details Extra scale command for CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_nu::ScaleEvents(){
//******************************************************************** 

  Measurement1D::ScaleEvents();
  if (ccqelike)
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, fScaleFactor,"width");

}


//********************************************************************   
/// @details Apply norm scaling to CCQELIKE PDG Hist
void MiniBooNE_CCQE_XSec_1DQ2_nu::ApplyNormScale(double norm){
//********************************************************************   
  Measurement1D::ApplyNormScale(norm);
  if (ccqelike)
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, 1.0/norm, "");
}
