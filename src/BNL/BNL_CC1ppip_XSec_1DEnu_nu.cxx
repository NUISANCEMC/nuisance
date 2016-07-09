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

#include "BNL_CC1ppip_XSec_1DEnu_nu.h"

// The constructor
BNL_CC1ppip_XSec_1DEnu_nu::BNL_CC1ppip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "BNL_CC1ppip_XSec_1DEnu_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/proton)";
  EnuMin = 0.;
  EnuMax = 3.0;
  isDiag = true;
  normError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BNL/CC1pip_on_p/BNL_CC1pip_on_p_W14_1986_corr_edges.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/((nevents+0.))*16./8.;
  // D2 = 1 proton, 1 neutron
};

// Need this if we're reading a root file!
/*
void BNL_CC1ppip_XSec_1DEnu_nu::SetDataValues(std::string fileLocation) {
  std::cout << "Reading: " << this->measurementName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH1D *copy = (TH1D*)(dataFile->Get("BNL_reanalysis"));

  this->data_points = copy->GetNbinsX()-1;

  xBinBounds = new double[this->data_points];
  dataVals = new double[this->data_points];
  dataErr = new double[this->data_points];

  for (int i = 0; i < data_points; i++) {
    xBinBounds[i] = copy->GetBinLowEdge(i+3);
    dataVals[i] = copy->GetBinContent(i+3)*1E-38;
    dataErr[i] = copy->GetBinError(i+3)*1E-38;
  }

  this->xBins = xBinBounds;
  this->data_values = dataVals;
  this->data_errors = dataErr;

  this->dataHist = new TH1D((this->measurementName+"_data").c_str(), (this->measurementName+this->plotTitles).c_str(), this->data_points-1, this->xBins);

  for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
    this->dataHist->SetBinContent(i+1, this->data_values[i]);
    this->dataHist->SetBinError(i+1, this->data_errors[i]);
  }

  dataHist->SetDirectory(0);

  dataFile->Close();
};
*/

void BNL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pp;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;

    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 2212) {
      Pp = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu;

  // Found a corrected one but only reliable to ~3GeV
  if (hadMass < 1400) {
    Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  } else {
    Enu = -1.0;
  }

  this->X_VAR = Enu;

  return;
};

bool BNL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != 14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false;

  if (((event->PartInfo(2))->fPID != 13) && ((event->PartInfo(3))->fPID != 13)) return false;

  int pipCnt = 0;
  int lepCnt = 0;
  int protonCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 13) {
      lepCnt++;
    } else if (PID == 211) {
      pipCnt++;
    } else if (PID == 2212 /*&& event->Ibound == 0*/) {
      protonCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pipCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (protonCnt != 1) return false;

  return true;
}

/*
void BNL_CC1ppip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1ppip_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

  mcHist->Scale(scaleFactor);
  mcFine->Scale(scaleFactor);

  return;
}
*/
