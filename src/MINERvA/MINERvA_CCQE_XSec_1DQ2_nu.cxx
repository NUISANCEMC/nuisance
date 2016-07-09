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

#include "MINERvA_CCQE_XSec_1DQ2_nu.h"

//******************************************************************** 
MINERvA_CCQE_XSec_1DQ2_nu::MINERvA_CCQE_XSec_1DQ2_nu(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  // Measurement Defaults
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  isFluxFix      = name.find("_newflux") != std::string::npos;
  fullphasespace = name.find("_20deg")   == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10.;
  normError = 0.101;
  allowed_types = "FIX,FREE,SHAPE/DIAG,FULL/NORM";
  default_types = "FIX/FULL";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup the Data Plots
  std::string basedir = std::string(std::getenv("NIWG_DATA"))+"/MINERvA/";
  std::string datafilename  = "";
  std::string covarfilename = "";

  // Full Phase Space
  if (fullphasespace){

    if (isFluxFix){
      if (isShape) isShape = false;
      datafilename  = "Q2QE_numu_data_fluxfix.txt";
      covarfilename = "Q2QE_numu_covar_fluxfix.txt";

    } else {
      if (isShape){
	datafilename  = "Q2QE_numu_dataa_SHAPE-extracted.txt";
	covarfilename = "Q2QE_numu_covara_SHAPE-extracted.txt";
      } else {
	datafilename  = "Q2QE_numu_data.txt";
	covarfilename = "Q2QE_numu_covar.txt";
      }
    }

  // Restricted Phase Space
  } else {
    if (isFluxFix){
      if (isShape) isShape = false;
      datafilename  = "20deg_Q2QE_numu_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_numu_covar_fluxfix.txt";

    } else {
      if (isShape){
	datafilename  = "20deg_Q2QE_numu_dataa_SHAPE-extracted.txt";
	covarfilename = "20deg_Q2QE_numu_covara_SHAPE-extracted.txt";
      } else {
	datafilename  = "20deg_Q2QE_numu_data.txt";
	covarfilename = "20deg_Q2QE_numu_covar.txt";
      }
    }
  }

  this->SetDataValues( basedir + datafilename );
  this->SetCovarMatrixFromText( basedir + covarfilename, 8 );

  // Setup Default MC Histograms
  this->SetupDefaultHist();

  // Set Scale Factor (EventHist/nucleons) * NNucl / NNeutons
  scaleFactor = (this->eventHist->Integral("width")*1E-38*13.0/6.0/(nevents+0.))/this->TotalIntegratedFlux(); 
  
};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  double q2qe = -1.0;
  double ThetaMu = -1.0;
  
  // Get the relevant signal information
  for (UInt_t j = 0; j < event->Npart(); ++j){

    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());    
    q2qe        = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(ThetaMu), 34.,true);

    break;

  }

  // Set binning variable
  this->X_VAR = q2qe;
  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event){
//*******************************************************************
  return SignalDef::isCCQEnumu_MINERvA(event, EnuMin, EnuMax, fullphasespace);
}
 
