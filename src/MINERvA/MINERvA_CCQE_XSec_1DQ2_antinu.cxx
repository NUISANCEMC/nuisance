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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CCQE_XSec_1DQ2_antinu.h"

//********************************************************************
MINERvA_CCQE_XSec_1DQ2_antinu::MINERvA_CCQE_XSec_1DQ2_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************

  // Setup Measurement Defaults
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  isFluxFix      = name.find("_oldflux") == std::string::npos;
  fullphasespace = name.find("_20deg")   == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10.;
  fNormError = 0.110;
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG,FULL/NORM";
  fDefaultTypes = "FIX/FULL";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup the Data Plots
  std::string basedir = FitPar::GetDataBase()+"/MINERvA/CCQE/";
  std::string datafilename  = "";
  std::string covarfilename = "";

  // Full Phase Space
  if (fullphasespace){

    if (isFluxFix){
      if (fIsShape){
        ERR(WRN) << "SHAPE likelihood comparison not available for MINERvA "
                 << "datasets with fixed flux information. NUISANCE will scale MC to match "
                 << "data normalization but full covariance will be used. " << std::endl;
      }
      datafilename  = "Q2QE_numubar_data_fluxfix.txt";
      covarfilename = "Q2QE_numubar_covar_fluxfix.txt";

    } else {
      if (fIsShape){
	datafilename  = "Q2QE_numubar_data_SHAPE-extracted.txt";
	covarfilename = "Q2QE_numubar_covar_SHAPE-extracted.txt";
      } else {
	datafilename  = "Q2QE_numubar_data.txt";
	covarfilename = "Q2QE_numubar_covar.txt";
      }
    }

  // Restricted Phase Space
  } else {
    if (isFluxFix){
      if (fIsShape){
        ERR(WRN) << "SHAPE likelihood comparison not available for MINERvA "
                 << "datasets with fixed flux information. NUISANCE will scale MC to match "
                 << "data normalization but full covariance will be used. " << std::endl;
      }
      datafilename  = "20deg_Q2QE_numubar_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_numubar_covar_fluxfix.txt";

    } else {
      if (fIsShape){
	datafilename  = "20deg_Q2QE_numubar_data_SHAPE-extracted.txt";
	covarfilename = "20deg_Q2QE_numubar_covar_SHAPE-extracted.txt";
      } else {
	datafilename  = "20deg_Q2QE_numubar_data.txt";
	covarfilename = "20deg_Q2QE_numubar_covar.txt";
      }
    }
  }

  this->SetDataValues( basedir + datafilename );
  this->SetCovarMatrixFromText( basedir + covarfilename, 8 );

  // Setup Default MC Histograms
  this->SetupDefaultHist();

  // Set Scale Factor (EventHist/nucleons) * NNucl / NNeutons
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38/(fNEvents+0.))*13./7./this->TotalIntegratedFlux();

};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event){
//********************************************************************

  if (event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double ThetaMu  = Pnu.Vect().Angle(Pmu.Vect());
  double q2qe     = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 30.,true);

  fXVar = q2qe;
  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_antinu::isSignal(FitEvent *event){
//*******************************************************************
  return SignalDef::isCCQEnumubar_MINERvA(event, EnuMin, EnuMax, fullphasespace);
}
