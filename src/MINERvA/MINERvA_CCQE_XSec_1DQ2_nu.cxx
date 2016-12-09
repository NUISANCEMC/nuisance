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

#include "MINERvA_CCQE_XSec_1DQ2_nu.h"

//********************************************************************
MINERvA_CCQE_XSec_1DQ2_nu::MINERvA_CCQE_XSec_1DQ2_nu(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************

  // Measurement Defaults
  fName = name;
  fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  isFluxFix      = name.find("_oldflux") == std::string::npos;
  fullphasespace = name.find("_20deg")   == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10.;
  fNormError = 0.101;
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
      datafilename  = "Q2QE_numu_data_fluxfix.txt";
      covarfilename = "Q2QE_numu_covar_fluxfix.txt";

    } else {
      if (fIsShape){
	datafilename  = "Q2QE_numu_data_SHAPE-extracted.txt";
	covarfilename = "Q2QE_numu_covar_SHAPE-extracted.txt";
      } else {
	datafilename  = "Q2QE_numu_data.txt";
	covarfilename = "Q2QE_numu_covar.txt";
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
      datafilename  = "20deg_Q2QE_numu_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_numu_covar_fluxfix.txt";

    } else {
      if (fIsShape){
	datafilename  = "20deg_Q2QE_numu_data_SHAPE-extracted.txt";
	covarfilename = "20deg_Q2QE_numu_covar_SHAPE-extracted.txt";
      } else {
	datafilename  = "20deg_Q2QE_numu_data.txt";
	covarfilename = "20deg_Q2QE_numu_covar.txt";
      }
    }
  }

  this->SetDataValues( basedir + datafilename );
  this->SetCovarMatrixFromText( basedir + covarfilename, 8 );

  // Quick Fix for Correl/Covar Issues only for old data
  if (!isFluxFix){
    fCorrel = (TMatrixDSym*)fFullCovar->Clone();
    delete fFullCovar;
    delete covar;
    delete fDecomp;
    fFullCovar = StatUtils::GetCovarFromCorrel(fCorrel,fDataHist);
    (*fFullCovar) *= 1E76;
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Setup Default MC Histograms
  this->SetupDefaultHist();

  // Set Scale Factor (EventHist/nucleons) * NNucl / NNeutons
  fScaleFactor = (this->fEventHist->Integral("width")*1E-38*13.0/6.0/(fNEvents+0.))/this->TotalIntegratedFlux();

};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double ThetaMu  = Pnu.Vect().Angle(Pmu.Vect());
  double q2qe     = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 34.,true);

  // Set binning variable
  fXVar = q2qe;
  return;
}



//********************************************************************
bool MINERvA_CCQE_XSec_1DQ2_nu::isSignal(FitEvent *event){
//*******************************************************************
  return SignalDef::isCCQEnumu_MINERvA(event, EnuMin, EnuMax, fullphasespace);
}

