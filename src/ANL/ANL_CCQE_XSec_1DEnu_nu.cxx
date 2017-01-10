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

#include "ANL_CCQE_XSec_1DEnu_nu.h"

ANL_CCQE_XSec_1DEnu_nu::ANL_CCQE_XSec_1DEnu_nu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  // Measurement Details
  fName = name;
  EnuMin = 0.;
  EnuMax = 6.;
  fIsDiag = true;
  applyQ2correction = type.find("Q2CORR") != std::string::npos;
  fDefaultTypes = "FIX/DIAG";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG/Q2CORR/ENUCORR";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  LOG(SAM) << "SETTING DATA"<<std::endl;

  // Setup Plots
  if (!name.compare("ANL_CCQE_XSec_1DEnu_nu_PRL31")){
    SetDataFromDatabase("ANL/ANL_CCQE_Data_PRL31_844.root", "ANL_1DEnu_Data");
    EnuMax = 3.0; // Move EnuMax down
  } else {
    SetDataFromDatabase("ANL/ANL_CCQE_Data_PRD16_3103.root", "ANL_1DEnu_fluxtuned_Data");
  }

  SetupDefaultHist();

  if (applyQ2correction){
    CorrectionHist = PlotUtils::GetTH1DFromFile(GeneralUtils::GetTopLevelDir() + "/data/ANL/ANL_CCQE_Data_PRL31_844.root","ANL_1DQ2_Correction");
  }

  // Setup Covariance
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Different generators require slightly different rescaling factors.
  fScaleFactor = (GetEventHistogram()->Integral("width")*2.0/1.0*1E-38/(fNEvents+0.)); // NEUT

};


/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void ANL_CCQE_XSec_1DEnu_nu::FillEventVariables(FitEvent *event){

  if (event->NumFSParticle(13) == 0)
    return;

  // Get Q2
  double q2qe = 0.0;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  q2qe = FitUtils::Q2QErec(Pmu, cos(ThetaMu), 0.,true);
  Enu_rec = FitUtils::EnuQErec(Pmu, cos(ThetaMu), 0.,true);

  fXVar = Enu_rec;

  // We save q2 into fYVar incase correction is applied
  fYVar = q2qe;
  return;
};


/// Signal is true CCQE scattering \n
/// \item Cut 1: numu event
/// \item Cut 2: Mode == 1
/// \item Cut 3: EnuMin < Enu < EnuMax
bool ANL_CCQE_XSec_1DEnu_nu::isSignal(FitEvent *event){

  return SignalDef::isCCQE(event, 14, EnuMin, EnuMax);
};


//! If Q2 correction is applied the CorrectionHist is interpolated
//! using the Q2QE value saved in fYVar
void ANL_CCQE_XSec_1DEnu_nu::FillHistograms(){

  if (applyQ2correction){
    if (fYVar < CorrectionHist->GetXaxis()->GetXmin())
      Weight *= CorrectionHist->Interpolate(fYVar);
  }

  Measurement1D::FillHistograms();
}


