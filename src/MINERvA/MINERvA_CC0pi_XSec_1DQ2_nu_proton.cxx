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

#include <string>
#include <sstream>

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC0pi_XSec_1DQ2_nu_proton.h"

MINERvA_CC0pi_XSec_1DQ2_nu_proton::MINERvA_CC0pi_XSec_1DQ2_nu_proton(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  // Setup Measurement
  fName         = "MINERvA_CC0pi_XSec_1DQ2_nu_proton";
  fPlotTitles   = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2})";
  fDefaultTypes = "FIX/FULL";
  fAllowedTypes = "FIX/FULL,DIAG";
  fNormError    = 0.100;
  EnuMin        = 0.;
  EnuMax        = 100.0;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Data
  SetDataValues( FitPar::GetDataBase()+"/MINERvA/CCQE/proton_Q2QE_nu_data.txt" );
  SetCovarMatrixFromText(FitPar::GetDataBase()+"/MINERvA/CCQE/proton_Q2QE_nu_covar.txt", 7);
  SetupDefaultHist();

  // Quick Fix for Correl/Covar Issues
  fCorrel = (TMatrixDSym*)fFullCovar->Clone();
  delete fFullCovar;
  delete covar;
  delete fDecomp;
  fFullCovar = StatUtils::GetCovarFromCorrel(fCorrel,fDataHist);
  (*fFullCovar) *= 1E76;
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Setup Coplanar Hist
  fCoplanarMCHist   = NULL;
  fCoplanarDataHist = NULL;

  // Setup a scaling factor for evt->xsec
  fScaleFactor = (GetEventHistogram()->Integral("width")*1E-38/(fNEvents+0.))/TotalIntegratedFlux();
};


void MINERvA_CC0pi_XSec_1DQ2_nu_proton::FillEventVariables(FitEvent *event){

  // Has NuMuCC1p
  if (event->HasISNuMuon() &&
      event->HasFSMuon() &&
      event->HasFSProton()){

    TLorentzVector pnu    = event->GetHMISNuMuon()->fP;
    TLorentzVector pprot  = event->GetHMFSProton()->fP;
    TLorentzVector pmu    = event->GetHMFSMuon()->fP;

    // Q2QE rec from leading proton assuming 34 MeV Eb
    double protmax = pprot.E();
    double q2qe    = FitUtils::ProtonQ2QErec(protmax, 34.);

    // Coplanar is angle between muon and proton plane
    TVector3 plnprotnu = pprot.Vect().Cross(pnu.Vect());
    TVector3 plnmunu   = pmu.Vect().Cross(pnu.Vect());
    double copl        = plnprotnu.Angle(plnmunu);

    // Fill X Variables
    fXVar = q2qe;

    // Save Coplanar into spare y variable
    fYVar = copl;
  }

  return;
};


bool MINERvA_CC0pi_XSec_1DQ2_nu_proton::isSignal(FitEvent *event){
  return SignalDef::isCC0pi1p_MINERvA(event, EnuMin*1.E3, EnuMax*1.E3);
};


bool MINERvA_CC0pi_XSec_1DQ2_nu_proton::SortExtraPlots(int state){

/*
  switch(state){

  // Reset Histograms at start of event loop
  case kExtraPlotReset:
    fCoplanarMCHist->Reset();
    break;

  // Fill calls for extra histograms on each event
  case kExtraPlotFill:
    fCoplanarMCHist->Fill(fYVar, Weight);
    break;

  // Extra handling for histograms after event loop
  case kExtraPlotConvert:
    fCoplanarMCHist->Scale( fCoplanarDataHist->Integral() / fCoplanarMCHist->Integral() );
    break;

  // Save the extra histograms
  case kExtraPlotWrite:
    fCoplanarMCHist->Write();
    break;
  }
*/
  return true;
}
