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

#include "BNL_CC1ppip_XSec_1DEnu_nu.h"

// The constructor
BNL_CC1ppip_XSec_1DEnu_nu::BNL_CC1ppip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "BNL_CC1ppip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/proton)";
  EnuMin = 0.;
  EnuMax = 3.0;
  fIsDiag = true;
  fNormError = 0.15;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/BNL/CC1pip_on_p/BNL_CC1pip_on_p_W14_1986_corr_edges.txt");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor = (this->fEventHist->Integral("width")*1E-38)/((fNEvents+0.))*16./8.;
  // D2 = 1 proton, 1 neutron
};

// Need this if we're reading a root file!
/*
void BNL_CC1ppip_XSec_1DEnu_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH1D *copy = (TH1D*)(dataFile->Get("BNL_reanalysis"));

  this->fNDataPointsX = copy->GetNbinsX()-1;

  xBinBounds = new double[this->fNDataPointsX];
  dataVals = new double[this->fNDataPointsX];
  dataErr = new double[this->fNDataPointsX];

  for (int i = 0; i < fNDataPointsX; i++) {
    xBinBounds[i] = copy->GetBinLowEdge(i+3);
    dataVals[i] = copy->GetBinContent(i+3)*1E-38;
    dataErr[i] = copy->GetBinError(i+3)*1E-38;
  }

  this->fXBins = xBinBounds;
  this->fDataValues = dataVals;
  this->fDataErrors = dataErr;

  this->fDataHist = new TH1D((this->fName+"_data").c_str(), (this->fName+this->fPlotTitles).c_str(), this->fNDataPointsX-1, this->fXBins);

  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    this->fDataHist->SetBinContent(i+1, this->fDataValues[i]);
    this->fDataHist->SetBinError(i+1, this->fDataErrors[i]);
  }

  fDataHist->SetDirectory(0);

  dataFile->Close();
};
*/

void BNL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 ||
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu     = -1.0;

  // Found a corrected one but only reliable to ~3GeV
  if (hadMass < 1400) Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);
  fXVar = Enu;

  return;
};

bool BNL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212,EnuMin,EnuMax);
}

/*
void BNL_CC1ppip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void BNL_CC1ppip_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

  fMCHist->Scale(fScaleFactor);
  fMCFine->Scale(fScaleFactor);

  return;
}
*/
