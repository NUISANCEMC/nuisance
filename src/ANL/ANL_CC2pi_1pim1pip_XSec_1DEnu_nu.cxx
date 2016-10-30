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

/**
 * D.Day et al, "Study of \nud charged-current two-pion production in the threshold region", Physical Review D, Volume 28, Number 11, 1 December 1983 \n
 * Derrick, Musgrave, Ammar, Day, Kafka and Mann, "Two- and three-pion productin by \nu\mud recations nears threshold: The implication for nucleon-decay experiments", Physical Review D, Vol 30, Number 7, 1 October 1984
*/

#include "ANL_CC2pi_1pim1pip_XSec_1DEnu_nu.h"

// The constructor
ANL_CC2pi_1pim1pip_XSec_1DEnu_nu::ANL_CC2pi_1pim1pip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  fName = "ANL_CC2pi_1pim1pip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 6.0;
  fIsDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  fNormError = 0.20; // normalisation error on ANL BNL flux
  fIsEnu1D = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/ANL/CC2pi/1pim1pip/CC2pi_1pim1pip1p_xsec.csv");
  this->SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  // Need to multiply the data by a factor because of the way the data is scanned (e.g. 1E-38)
  fDataHist->Scale(1.E-41);

  this->fScaleFactor = this->fEventHist->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.);
};



void ANL_CC2pi_1pim1pip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  double Enu = Pnu.E()/1000.;

  // No hadronic mass cut or similar here so very simple FillEventVariables

  this->fXVar = Enu;

  return;
}

// Signal asks for 1pi-, 1pi+, 1mu-, 1p
bool ANL_CC2pi_1pim1pip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  int pdgs[] = {13, 211, -211, 2212};
  return SignalDef::isCCWithFS(event, 14, pdgs,
                               EnuMin, EnuMax);
}


/*
void ANL_CC2pi_1pim1pip_XSec_1DEnu_nu::FillHistograms() {

  if (makeHadronicMassHist) {
    hadMassHist->Fill(hadMass);
  }

  Measurement1D::FillHistograms();

}


void ANL_CC2pi_1pim1pip_XSec_1DEnu_nu::ScaleEvents() {

  PlotUtils::FluxUnfoldedScaling(mcHist, fFluxHist);
  PlotUtils::FluxUnfoldedScaling(mcFine, fFluxHist);

  mcHist->Scale(fScaleFactor);
  mcFine->Scale(fScaleFactor);

  return;
}
*/
