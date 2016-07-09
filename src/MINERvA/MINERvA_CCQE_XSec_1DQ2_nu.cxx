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

  // Measurement Details                                                                                                           
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  isFluxFix      = name.find("_newflux") != std::string::npos;
  fullphasespace = name.find("_20deg")   == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10.;
  normError = 0.101;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  // Setup the Data Plots
  std::string basedir = std::string(std::getenv("NIWG_DATA"))+"/MINERvA/";
  int nbins = 8;
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
  scaleFactor = (this->eventHist->Integral("width")*1E-38*13.0/6.0/(nevents+0.))/this->TotalIntegratedFlux(); // NEUT
  
};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
  //********************************************************************

  // Get the relevant signal information
  for (int j = 0; j < event->Npart(); ++j){

    if ((event->PartInfo(j))->fPID != 13) continue;

    ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
    
    q2qe        = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(ThetaMu), 34.,true);
    Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 34.,true);

    break;

  }

  this->X_VAR = q2qe;
  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event){
//*******************************************************************

  // For now, define as the true mode being CCQE or npnh
  if (event->Mode != 1 and event->Mode != 2) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // If Restricted phase space
  if (!fullphasespace &&  ThetaMu > 0.34906585) return false;

  // restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;
  if (Enu_rec < this->EnuMin || Enu_rec > this->EnuMax) return false;

  return true;
};

