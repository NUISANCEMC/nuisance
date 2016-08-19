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

#include "MiniBooNE_CCQE_XSec_1DQ2_antinu.h"

//********************************************************************
/// @brief MiniBooNE CCQE antinumu 1DQ2 Measurement on CH2 (Ref: - )
///   
//******************************************************************** 
MiniBooNE_CCQE_XSec_1DQ2_antinu::MiniBooNE_CCQE_XSec_1DQ2_antinu(std::string name, std::string inputfile,
								 FitWeight *rw,    std::string type,
								 std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details 
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.;
  EnuMax = 3.;
  normError = 0.130;
  default_types="FIX/DIAG";
  allowed_types="FIX,FREE,SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  this->ccqelike = name.find("CCQELike") != std::string::npos;

  this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_con.txt");
  this->SetupDefaultHist();
                                                    
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

  // ScaleFactor
  scaleFactor = ((eventHist->Integral("width")*1E-38/(nevents+0.))
		 *14.08/8.
		 / TotalIntegratedFlux()); 
};

//******************************************************************** 
/// @details Extract q2qe(X_VAR) from the event
void  MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  q2qe = -999.9;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    int PID = fabs((event->PartInfo(j))->fPID);

    // Look for the outgoing muon
    if (PID == -13 or (ccqelike and PID == 13)){
    
      // Now find the Q2QE value and fill the histogram
      q2qe = FitUtils::Q2QErec((event->PartInfo(j))->fP, 
			       cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 30., false);
     
      break;
    }
  }
 
  // Set X Variables
  this->X_VAR = q2qe;
  
  return;
};

//******************************************************************** 
bool MiniBooNE_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event){
//******************************************************************** 

  // 2 Different Signal Definitions
  if (ccqelike) return SignalDef::isMiniBooNE_CCQELike(event, EnuMin, EnuMax);
  else return SignalDef::isMiniBooNE_CCQEBar(event, EnuMin, EnuMax);

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
