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
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.;
  EnuMax = 3.;
  fNormError = 0.130;
  fDefaultTypes="FIX/DIAG";
  fAllowedTypes="FIX,FREE,SHAPE/DIAG";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  this->ccqelike = name.find("CCQELike") != std::string::npos;

  this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_con.txt");
  this->SetupDefaultHist();
                                                    
  if (!this->fIsDiag) this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/MiniBooNE_1DQ2_antinu.root");
  else {
    LOG(SAM) << "Making diagonal covar" << endl;
    fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
    covar      = StatUtils::GetInvert(fFullCovar);
  }

  
  ///
  /// If CCQELike is used an additional the CCQELike BKG is used and a PDG Histogram is saved
  if (ccqelike){

    fDataHist_CCQELIKE = PlotUtils::GetTH1DFromFile(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_bkg_ccqe.txt", \
						   (this->fName+"_data_CCQELIKE"), this->fPlotTitles);

    fDataHist_CCPIM = PlotUtils::GetTH1DFromFile(FitPar::GetDataBase()+"/MiniBooNE/anti-ccqe/asqq_bkg_ccpim.txt", \
						(this->fName+"_data_CCPIM"), this->fPlotTitles);

    // Make NON CCPIM
    fDataHist_NONCCPIM = (TH1D*) fDataHist_CCQELIKE->Clone();
    fDataHist_NONCCPIM->SetNameTitle((this->fName+"_data_NONCCPIM").c_str(), (this->fName+"_data_NONCCPIM").c_str());

    // Perform fDataHist Sums
    for (int i = 0; i < fDataHist->GetNbinsX(); i++){
      fDataHist_NONCCPIM->SetBinContent(i+1, fDataHist_CCQELIKE->GetBinContent(i+1) - fDataHist_CCPIM->GetBinContent(i+1));
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1) + fDataHist_CCQELIKE->GetBinContent(i+1));
    }

    PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_CCQELIKE);

    PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_NONCCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_NONCCPIM);

    PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_CCPIM);

  }

  // ScaleFactor
  fScaleFactor = ((fEventHist->Integral("width")*1E-38/(fNEvents+0.))
		 *14.08/8.
		 / TotalIntegratedFlux()); 
};

//******************************************************************** 
/// @details Extract q2qe(fXVar) from the event
void  MiniBooNE_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event){
//******************************************************************** 
 
  // Init
  q2qe = -999.9;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    int PID = ((event->PartInfo(j))->fPID);

    if (PID != -13 and !ccqelike)       continue;
    if (abs(PID) != 13 and ccqelike)   continue;
    
    // Now find the Q2QE value and fill the histogram
    q2qe = FitUtils::Q2QErec((event->PartInfo(j))->fP, 
			     cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 30., false);
    break;
  }
  
  // Set X Variables
  fXVar = q2qe;
  
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

    if (fabs(Mode) == 11 or fabs(Mode) == 13)  PlotUtils::FillNeutModeArray(fMCHist_CCPIM, Mode, fXVar, Weight);
    else PlotUtils::FillNeutModeArray(fMCHist_NONCCPIM, Mode, fXVar, Weight);

    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, Mode, fXVar, Weight);
  }

}


//********************************************************************
/// @details Extra write command to save the CCQELike/CCPIM PDG if required
void MiniBooNE_CCQE_XSec_1DQ2_antinu::Write(std::string drawOpt){
//********************************************************************

  Measurement1D::Write(drawOpt);

  if (ccqelike){
    fDataHist_CCQELIKE->Write();

    THStack combo_fMCHist_CCQELIKE = PlotUtils::GetNeutModeStack((this->fName + "_MC_CCQELIKE").c_str(), (TH1**)this->fMCHist_CCQELIKE, 0);
    combo_fMCHist_CCQELIKE.Write();

    fDataHist_CCPIM->Write();

    THStack combo_fMCHist_CCPIM = PlotUtils::GetNeutModeStack((this->fName + "_MC_CCPIM").c_str(), (TH1**)this->fMCHist_CCPIM, 0);
    combo_fMCHist_CCPIM.Write();

    fDataHist_NONCCPIM->Write();

    THStack combo_fMCHist_NONCCPIM = PlotUtils::GetNeutModeStack((this->fName + "_MC_NONCCPIM").c_str(), (TH1**)this->fMCHist_NONCCPIM, 0);
    combo_fMCHist_NONCCPIM.Write();


  }

}


//******************************************************************** 
/// @details Extra scale command for CCQELIKE/CCPIM PDG Hist      
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ScaleEvents(){
//******************************************************************** 

  Measurement1D::ScaleEvents();

  if (ccqelike){
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, fScaleFactor,"width");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCPIM, fScaleFactor,"width");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_NONCCPIM, fScaleFactor,"width");
  }
}


//******************************************************************** 
/// @details Apply norm scaling to CCQELIKE/CCPIM PDG Hist        
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ApplyNormScale(double norm){
//******************************************************************** 

  Measurement1D::ApplyNormScale(norm);
  
  if (ccqelike){
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, 1.0/norm, "");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCPIM, 1.0/norm, "");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_NONCCPIM, 1.0/norm, "");
  }
}



//********************************************************************                                                                                                                                                                     /// @details Extra scale command for CCQELIKE PDG Hist                                                                                                                                                                                     
void MiniBooNE_CCQE_XSec_1DQ2_antinu::ResetAll(){
  //********************************************************************                                                                                                                                                                   
  Measurement1D::ResetAll();   
  if (ccqelike){
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_CCQELIKE);
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_CCPIM);
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_NONCCPIM);
  }

}
