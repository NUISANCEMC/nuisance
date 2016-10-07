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

#include "TemplateMeas1D.h"

//******************************************************************** 
TemplateMeas1D::TemplateMeas1D(std::string name, std::string inputfile,
			       FitWeight *rw, std::string type){
//******************************************************************** 

  // Setup Main Measurement Details                                                                
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  
  // Setup Enu Limits. NUISANCE selects only events between this limit
  // when a flux integrals are calculated.
  EnuMin = 0.;
  EnuMax = 3.;

  // Set normalisation error for data. This will be used to add a penalty
  // if NORM is supplied in the type.
  fNormError = 0.20; // 20%
  
  // Setup allowed/default types
  // These give the starting possible options that can be specified when making the class.
  // Different discrete fields should be seperated by '/' and conflicting options
  // should be seperated by ','. e.g. FIX, FREE, and SHAPE are conflicting types because
  // they all handle the normalisation. NORM is an extra option to add a penalty term
  // so should be kept seperate as below. We also want to force DIAG as there is no
  // covariance so we put that as the starting default option so it will be set even if
  // the user doesn't explicitly set it.
  fDefaultTypes = "FIX/DIAG";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG/NORM";
  
  // Multiple similar classes can be read by a single class.
  // e.g. MB numu CCQE or CC0pi.
  // The standard is to switch from the default by using fName.
  fAnalysis = kTemplateMeas1D_CC0pi_Tmu; // Default Analysis
  
  if (fName == "TemplateMeas1D_CCQE_Q2"){
    fAnalysis = kTemplateMeas1D_CCQE_Q2;  // Alternate Analysis
  }

  // Once all the options are set we setup the lower level event variables
  // !! This must go after all the settings above !!
  Measurement1D::SetupMeasurement(inputfile, type, rw, "");
  
  // Setup a scaling factor once the measurement has been setup
  // !! This must go after SetupMeasurement !!

  // The scalefactor goes from rawevents -> xsec prediction.
  // First we scale the histograms to match the fEventHist prediction.
  // Here the data is saved as cm^2 / neutron, but we generate on a
  // CH2 target. We must then convert it by multiplying by (14.0/6.0).
  // Finally we divide by the integrated flux to get the cross-section.
  fScaleFactor = ((fEventHist->Integral("width")*1E-38/(fNEvents+0.))
		  * (14.0/6.0)
		  / TotalIntegratedFlux());

  

  // After initial setup the constructor should setup the plots
  // according to what analysis has been stated.
  if (fAnalysis == kTemplateMeas1D_CC0pi_Tmu){
    SetDataFromDatabase( "/Template/TemplateMeas1D_Data.root",
		   "Tmu_CC0pi_Data.root" );
  } else {
    SetDataFromDatabase( "/Template/TemplateMeas1D_Data.root",
		   "Q2_CCQE_Data.root" );
  }

  // NUISANCE uses the data histogram to setup all the default MC histograms
  // fMCHist,fMCFine,fMCStat,fMaskHist,fMCHist_PDG are all set here.
  // !! It must go after the data hist has been set !!
  SetupDefaultHist();

  // Setup Covariance
  // Since its diagonal it is useful to setup a diagonal covariance matrix
  // for use in fake data study covariance throws.
  // If a covariance IS provided it should be setup here.                                            
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);



  // There will be cases where we want to save optional histograms
  // to help with validation/studies. They should be setup in the constructor.

  // Here we also save a true Q2 plot split by interaction channels in CC0pi for one analysis
  if (fAnalysis == kTemplateMeas1D_CC0pi_Tmu){

    // Create basic plot
    fMCHist_ExtraQ2Plot = new TH1D( (fName + "_MC_Q2").c_str(),
				    (fName + "_MC_Q2;"
				     "Q^{2}_{QE} (GeV^2);d#sigma/dQ^{2}_{QE} (cm^{2}/GeV^{2})").c_str(),
				    20, 0.0, 3.0 );

    // Create Channel Plot
    //    fMCHist_ExtraQ2Plot_PDG = NULL;
  }
  

};

//******************************************************************** 
/// @details Extract q2qe(fXVar) from the event
void  TemplateMeas1D::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  double q2qe = -999.9;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    int PID = abs((event->PartInfo(j))->fPID);
    if (!event->PartInfo(j)->fIsAlive) continue;
    
    if (PID != 13 and !ccqelike)     continue;
    if (abs(PID) != 13 and ccqelike) continue;
    
      // Now find the Q2QE value and fill the histogram
    q2qe = FitUtils::Q2QErec((event->PartInfo(j))->fP, 
			     cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 
			     34., true);
    
    break; 
  }

  // Set X Variables
  fXVar = q2qe;
  
  return;
};

//******************************************************************** 
bool TemplateMeas1D::isSignal(FitEvent *event){
//******************************************************************** 

  // 2 Different Signal Definitions
  //if (ccqelike) return SignalDef::isMiniBooNE_CCQELike(event, EnuMin, EnuMax);
  //  else return SignalDef::isMiniBooNE_CCQE(event, EnuMin, EnuMax);

  return true;
};

//******************************************************************** 
/// @details Fills a ccqe-like background plot if required
void TemplateMeas1D::FillHistograms(){
//******************************************************************** 

  Measurement1D::FillHistograms();

  //  if (Mode != 1 and Mode != 2 and ccqelike and Signal){
  //    PlotUtils::FillNeutModeArray(fMCHist_CCQELIKE, Mode, fXVar, Weight);
  //  }
}


//********************************************************************   
/// @details Extra write command to save the CCQELike PDG if required
void TemplateMeas1D::Write(std::string drawOpt){
//********************************************************************   

  Measurement1D::Write(drawOpt);

  /*
  if (ccqelike){
    fDataHist_CCQELIKE->Write();
    
    THStack combo_fMCHist_CCQELIKE = PlotUtils::GetNeutModeStack((this->fName + "_MC_CCQELIKE").c_str(), (TH1**)this->fMCHist_CCQELIKE, 0);
    combo_fMCHist_CCQELIKE.Write();
  }
  */

}


//********************************************************************   
void TemplateMeas1D::ResetAll(){
//******************************************************************** 
  Measurement1D::ResetAll();
  
//  if (ccqelike)
    //    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_CCQELIKE);
  
}

//******************************************************************** 
void TemplateMeas1D::ScaleEvents(){
//******************************************************************** 

  Measurement1D::ScaleEvents();
  //  if (ccqelike)
  //    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, fScaleFactor,"width");

}


//********************************************************************   
void TemplateMeas1D::ApplyNormScale(double norm){
//********************************************************************   
  Measurement1D::ApplyNormScale(norm);
  //  if (ccqelike)
    //    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_CCQELIKE, 1.0/norm, "");
}
