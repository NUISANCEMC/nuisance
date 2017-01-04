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

#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"

//********************************************************************
MINERvA_CCinc_XSec_2DEavq3_nu::MINERvA_CCinc_XSec_2DEavq3_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************

  // Measurement Details
  fName = "MINERvA_CCinc_XSec_2DEavq3_nu";
  fPlotTitles = "; q_{3} (GeV); E_{avail} (GeV); d^{2}#sigma/dq_{3}dE_{avail} (cm^{2}/GeV^{2})";
  EnuMin = 2.;
  EnuMax = 6.;
  hadroncut = FitPar::Config().GetParB("MINERvA_CCinc_XSec_2DEavq3_nu.hadron_cut");
  useq3true = FitPar::Config().GetParB("MINERvA_CCinc_XSec_2DEavq3_nu.useq3true");
  splitMEC_PN_NN = FitPar::Config().GetParB("Modes.split_PN_NN");
  fNormError = 0.107;
  fDefaultTypes = "FIX/FULL";
  fAllowedTypes = "FIX,FREE,SHAPE/FULL,DIAG/MASK";
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Binning for the 2D Histograms
  this->fNDataPointsX = 7;
  this->fNDataPointsY = 17;
  Double_t tempx[7] = {0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8};
  Double_t tempy[17] = {0.0, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.20, 0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.80};
  this->fXBins = tempx;
  this->fYBins = tempy;

  // Fill data and 1Dto2D Maps for covariance
  SetDataValuesFromText(   FitPar::GetDataBase()+"/MINERvA/CCEavq3/data_2D.txt", 1E-42);
  SetMapValuesFromText(    FitPar::GetDataBase()+"/MINERvA/CCEavq3/map_2D.txt");
  SetCovarMatrixFromChol(  FitPar::GetDataBase()+"/MINERvA/CCEavq3/covar_2D.txt", 67);

  // Data is in 1E-42 and so is the covariance, need to scale accordingly.
  (*this->fFullCovar) *= 1E-16;
  (*this->covar)      *= 1E16;

  // Set data errors from covariance matrix
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Setup mc Histograms
  SetupDefaultHist();

  // Set Scale Factor
  fScaleFactor = (this->fEventHist->Integral("width")*1E-42/(fNEvents+0.))/this->TotalIntegratedFlux();
};



//********************************************************************
void MINERvA_CCinc_XSec_2DEavq3_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  // Seperate MEC
  if (splitMEC_PN_NN){
    int npr = 0;
    int nne = 0;

    for (UInt_t j = 0; j < event->Npart(); j++){
      if ((event->PartInfo(j))->fIsAlive) continue;

      if (event->PartInfo(j)->fPID == 2212) npr++;
      else if (event->PartInfo(j)->fPID == 2112) nne++;
    }

    if (event->Mode == 2 and npr == 1 and nne == 1){
      event->Mode = 2;
      Mode = 2;

    } else if (event->Mode == 2 and npr == 0 and nne == 2){
      event->Mode = 3;
      Mode = 3;

    }
  }

  // Set Defaults
  double Eav = -999.9;
  double q3 = -999.9;

  // If muon found get kinematics
  FitParticle* muon      = event->GetHMFSParticle(13);
  FitParticle* neutrino  = event->GetNeutrinoIn();
  if (muon && neutrino){

    // Set Q from Muon
    TLorentzVector q = neutrino->fP - muon->fP;
    double q0 = (q.E())/1.E3;
    //double q3_true = (q.Vect().Mag())/1.E3;
    double thmu = muon->fP.Vect().Angle(neutrino->fP.Vect());
    double pmu  = muon->fP.Vect().Mag()/1.E3;
    double emu  = muon->fP.E()/1.E3;
    double mmu  = muon->fP.Mag()/1.E3;

    // Get Enu Rec
    double enu_rec = emu + q0;

    // Set Q2 QE
    double q2qe = 2*enu_rec * (emu - pmu * cos(thmu)) - mmu*mmu;

    // Calc Q3 from Q2QE and EnuTree
    q3 = sqrt(q2qe + q0*q0);

    // Get Eav too
    Eav = FitUtils::GetErecoil_MINERvA_LowRecoil(event) / 1.E3;
  }

  // Set Hist Variables
  fXVar = q3;
  fYVar = Eav;

  return;
}

//********************************************************************
bool MINERvA_CCinc_XSec_2DEavq3_nu::isSignal(FitEvent *event){
//********************************************************************
  return SignalDef::isCCincLowRecoil_MINERvA(event, EnuMin, EnuMax);
}
