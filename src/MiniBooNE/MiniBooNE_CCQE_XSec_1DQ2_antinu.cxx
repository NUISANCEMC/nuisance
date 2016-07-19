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

#include "MiniBooNE_CCQE_XSec_1DQ2_antinu.h"

//********************************************************************
/// @brief MiniBooNE CCQE antinumu 1DQ2 Measurement on CH2 (Ref: - )
///   
//******************************************************************** 
MiniBooNE_CCQE_XSec_1DQ2_antinu::MiniBooNE_CCQE_XSec_1DQ2_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details 
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.;
  EnuMax = 3.;
  normError = 0.130;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  this->ccqelike = name.find("CCQELike") != std::string::npos;

  this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_con.txt");

  this->SetupDefaultHist();

  // Setup Covariances                                                                                                                                                                                                                      
  /// Currently has a placeholder for the matrices as work fixing them is ongoing.                                                                                                                                                          
  /// Assume a diagonal shape-only error is default                                                                                                                                                                                         
  if (!this->isDiag) this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/MiniBooNE_1DQ2_antinu.root");
  else {
    fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
    covar     = StatUtils::GetInvert(fullcovar);
  }

  
  ///
  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG Histogram is saved
  if (ccqelike){

    dataHist_CCQELIKE = PlotUtils::GetTH1DFromFile(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_bkg_ccqe.txt", \
						   (this->measurementName+"_data_CCQELIKE"), this->plotTitles);

    dataHist_CCPIM = PlotUtils::GetTH1DFromFile(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_bkg_ccpim.txt", \
						(this->measurementName+"_data_CCPIM"), this->plotTitles);

    // Make NON CCPIM
    dataHist_NONCCPIM = (TH1D*) dataHist_CCQELIKE->Clone();
    dataHist_NONCCPIM->SetNameTitle((this->measurementName+"_data_NONCCPIM").c_str(), (this->measurementName+"_data_NONCCPIM").c_str());

    // Perform dataHist Sums
    for (int i = 0; i < dataHist->GetNbinsX(); i++){
      dataHist_NONCCPIM->SetBinContent(i+1, dataHist_CCQELIKE->GetBinContent(i+1) - dataHist_CCPIM->GetBinContent(i+1));
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1) + dataHist_CCQELIKE->GetBinContent(i+1));
    }

    PlotUtils::CreateNeutModeArray((TH1D*)this->mcHist,(TH1**)this->mcHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)this->mcHist_CCQELIKE);

    PlotUtils::CreateNeutModeArray((TH1D*)this->mcHist,(TH1**)this->mcHist_NONCCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->mcHist_NONCCPIM);

    PlotUtils::CreateNeutModeArray((TH1D*)this->mcHist,(TH1**)this->mcHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->mcHist_CCPIM);

  }

  /// Calculates a flux averaged cross-section from (Evt("width")/Flux("width")) * 14.08/8.0         
  this->scaleFactor = (this->eventHist->Integral("width")*1E-38/(nevents+0.))*14.08/8.; // NEUT
};

//******************************************************************** 
/// @details Extract q2qe(X_VAR) from the event
void  MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  bad_particle = false;
  q2qe = 0.0;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    int PID = fabs((event->PartInfo(j))->fPID);

    // Look for the outgoing muon
    if (PID == 13){
    
      // Now find the Q2QE value and fill the histogram
      q2qe = FitUtils::Q2QErec((event->PartInfo(j))->fP, 
			       cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 30., false);
     
      if (!ccqelike) break;
      continue;
    }

    // Check alive                                                                                                                                                                                                                         
    if (!event->PartInfo(j)->fIsAlive or event->PartInfo(j)->fStatus != 0) continue;

    // Check for bad particles;
    if (PID != 2112 and PID != 22  and \
	PID != 2212 and	PID != 13)      
	bad_particle = true;

    
    
  }

  // Set X Variables
  this->X_VAR = q2qe;
  
  return;
};

//******************************************************************** 
/// @details Signal is CCQE/CCQELike numu on CH
/// @details CCQE Cut 1 : Mode 1 or Mode 2
/// @details CCQELike Cut 1: Only nucleons, muons, and photons
/// @details Cut 2: Numu
/// @details Cut 3: Correct Energy Range 0.0 to 3.0 GeV
bool MiniBooNE_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event){
//******************************************************************** 

  // For now, define as the true mode being CCQE or npnh
  if (!ccqelike){
    if (Mode != -1 && Mode != -2) return false;
  }

  // Don't include Numu CCQE in antineutrino?
  if (Mode == 1 or Mode == 2) return false;

  // If CCQELike Signal
  if (ccqelike and bad_particle) return false;

  // Only look at numu events
  if (fabs((event->PartInfo(0))->fPID) != 14) return false;

  // Restrict energy range
  if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;

  if (X_VAR <= 0.0) return false;
  if (abs(Mode) >= 40) return false;

  return true;
};




//********************************************************************
/// @details Fills a ccqe-like and ccpim background plot if required
void MiniBooNE_CCQE_XSec_1DQ2_antinu::FillHistograms(){
//********************************************************************

  Measurement1D::FillHistograms();

  if (Mode != -1 && Mode != -2 and ccqelike and Signal){

    if (fabs(Mode) == 11 or fabs(Mode) == 13)  PlotUtils::FillNeutModeArray(mcHist_CCPIM, Mode, X_VAR, Weight);
    else PlotUtils::FillNeutModeArray(mcHist_NONCCPIM, Mode, X_VAR, Weight);

    PlotUtils::FillNeutModeArray(mcHist_CCQELIKE, Mode, X_VAR, Weight);
  }

}


//********************************************************************
/// @details Extra write command to save the CCQELike/CCPIM PDG if required
void MiniBooNE_CCQE_XSec_1DQ2_antinu::Write(std::string drawOpt){
//********************************************************************

  Measurement1D::Write(drawOpt);

  if (ccqelike){
    dataHist_CCQELIKE->Write();

    THStack combo_mcHist_CCQELIKE = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_CCQELIKE").c_str(), (TH1**)this->mcHist_CCQELIKE, 0);
    combo_mcHist_CCQELIKE.Write();

    dataHist_CCPIM->Write();

    THStack combo_mcHist_CCPIM = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_CCPIM").c_str(), (TH1**)this->mcHist_CCPIM, 0);
    combo_mcHist_CCPIM.Write();

    dataHist_NONCCPIM->Write();

    THStack combo_mcHist_NONCCPIM = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_NONCCPIM").c_str(), (TH1**)this->mcHist_NONCCPIM, 0);
    combo_mcHist_NONCCPIM.Write();


  }

}


//******************************************************************** 
/// @details Extra scale command for CCQELIKE/CCPIM PDG Hist      
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ScaleEvents(){
//******************************************************************** 

  Measurement1D::ScaleEvents();

  if (ccqelike){
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_CCQELIKE, scaleFactor,"width");
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_CCPIM, scaleFactor,"width");
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_NONCCPIM, scaleFactor,"width");
  }
}


//******************************************************************** 
/// @details Apply norm scaling to CCQELIKE/CCPIM PDG Hist        
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ApplyNormScale(double norm){
//******************************************************************** 

  Measurement1D::ApplyNormScale(norm);
  
  if (ccqelike){
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_CCQELIKE, 1.0/norm, "");
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_CCPIM, 1.0/norm, "");
    PlotUtils::ScaleNeutModeArray((TH1**)mcHist_NONCCPIM, 1.0/norm, "");
  }
}



//********************************************************************                                                                                                                                                                     /// @details Extra scale command for CCQELIKE PDG Hist                                                                                                                                                                                     
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ResetAll(){
  //********************************************************************                                                                                                                                                                      
  if (ccqelike){
    PlotUtils::ResetNeutModeArray((TH1**)mcHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)mcHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)mcHist_NONCCPIM);
  }

}
