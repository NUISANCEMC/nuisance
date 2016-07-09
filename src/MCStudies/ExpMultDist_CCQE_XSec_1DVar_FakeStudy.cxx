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

#include "ExpMultDist_CCQE_XSec_1DVar_FakeStudy.h"

//******************************************************************** 
/// @brief Class to perform CCQE Fake Data Studies on a custom measurement
ExpMultDist_CCQE_XSec_1DVar_FakeStudy::ExpMultDist_CCQE_XSec_1DVar_FakeStudy(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 
  
  // Measurement Details
  measurementName = name;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 6.;

  // Set default fitter flags
  isDiag = true;
  isShape = false;
  isRawEvents = false;

  // This function will sort out the input files automatically and parse all the inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile); 

  
  // Use the name to define what variable to measure
  int nbins;
  double binlow, binhigh;

  if (name.find("1DQ2") != std::string::npos){
    plottype = 1; plotTitles = "";
    nbins = 30;  binlow = 0.0;  binhigh = 2.0;
  } else if (name.find("1DTmu") != std::string::npos){
    plottype = 2; plotTitles = "";
    nbins = 20;  binlow = 0.0;  binhigh = 3.0;
  } else if (name.find("1DCos") != std::string::npos){
    plottype = 3;  plotTitles = "";
    nbins = 10;  binlow = -1.0; binhigh = 1.0; 
  }
  
  // Setup the datahist as empty, we will use fake data to fill it.
  this->dataHist = new TH1D((measurementName + "_data").c_str(), (measurementName + "_data" + plotTitles).c_str(), nbins, binlow, binhigh);
  
  // Once dataHist is setup this function will automatically generate matching MC histograms
  this->SetupDefaultHist();

  
  // Setup Covariance assuming a diagonal covar.
  // If you want a full covariance to be used examples are given in the MINERvA 1D classes
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);
  

  // 3. The generator is organised in SetupMeasurement so it gives the cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and divide by the number of neutrons 6.
  this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))  * (12.0 / 6.0)  /this->TotalIntegratedFlux();

};



//******************************************************************** 
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void ExpMultDist_CCQE_XSec_1DVar_FakeStudy::FillEventVariables(FitEvent *event){
//******************************************************************** 

  // MUST be defined for each new sample.
  // This function reads in the FitEvent format and lets you grab any information you need
  // from the event. This function is only called during the first and last iteration of each fit so that
  // a vector of X_VAR variables can be filled for the signal events.

  // Define empty variables
  this->X_VAR = -1.0;
  double q2qe = 0.0;
  double CosThetaMu = -2.0;
  double TMu = 0.0;
  
  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    // Look for the outgoing muon
    if ((event->PartInfo(j))->fPID != 13) continue;

    // Define any variables we may need
    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());

    Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);
    q2qe        = FitUtils::Q2QErec( (event->PartInfo(j))->fP, cos(ThetaMu), 0.,true);

    CosThetaMu = cos(ThetaMu);
    TMu = FitUtils::T((event->PartInfo(j))->fP);
    
    // Once lepton is found, don't continue the loop
    break;  
  }

  if (this->plottype == 1) this->X_VAR = q2qe;
  else if (this->plottype == 2) this->X_VAR = TMu;
  else if (this->plottype == 3) this->X_VAR = CosThetaMu;
  
  return;
};

//******************************************************************** 
/// @details Signal is true CCQE scattering
///
/// @details Cut 1: numu event
/// @details Cut 2: Mode == 1
/// @details Cut 3: EnuMin < Enu < EnuMax
bool ExpMultDist_CCQE_XSec_1DVar_FakeStudy::isSignal(FitEvent *event){
//******************************************************************** 

  // Place cuts on each of the events here.
  // MUST be defined for all new samples, there is no default signal definition.
  // Check FitUtils and CutUtils for example cuts that can be used quickly.

  // During a fit isSignal is only used in the first fit function call to define
  // which of the events actually need to be considered, so don't worry too much if
  // this function is inefficient.

  // Mode and Enu are automatically avaialble in this function, they don't need to be set earlier.
  
  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Only look at CCQE Events and MEC Events
  if (Mode != 1 and Mode != 2) return false;
  
  // Restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;

  return true;
};

