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

#include "MINERvA_CCNpip_XSec_1DEnu_nu.h"

//********************************************************************
// The constructor
MINERvA_CCNpip_XSec_1DEnu_nu::MINERvA_CCNpip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile) {
//********************************************************************

  fName = "MINERvA_CCNpip_XSec_1DEnu_nu_2016";
  fPlotTitles = "; E_{#nu} (GeV); d#sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-enu.csv");

  // MINERvA has the error quoted as a percentage of the cross-section
  // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*(fDataHist->GetBinError(i+1)/100.));
  }

  // We're given a correlation matrix, so need to convert it to a covariance matrix
  this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-enu.csv", fDataHist->GetNbinsX());

  this->SetupDefaultHist();

  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents);
};

//********************************************************************
void MINERvA_CCNpip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(13) == 0) return;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);

  double Enu = -999;
  if (hadMass < 1800) Enu = Pnu.E()/1000.;

  fXVar = Enu;

  return;
};

//********************************************************************
// The false refers to that this cross-section uses the full phase space
bool MINERvA_CCNpip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCCNpip_MINERvA(event, EnuMin, EnuMax, false);
}
