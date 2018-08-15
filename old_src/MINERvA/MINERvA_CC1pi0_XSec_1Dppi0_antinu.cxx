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

#include "MINERvA_CC1pi0_XSec_1Dppi0_antinu.h"



//********************************************************************
MINERvA_CC1pi0_XSec_1Dppi0_antinu::MINERvA_CC1pi0_XSec_1Dppi0_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC1pi0_XSec_1Dppi0_antinu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current numubar \n" \
                        "Signal: Any event with 1 muon, 1 pion, no other tracks \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#pi^{0}} (GeV/c)");
  fSettings.SetYTitle("d#sigma/dp_{#pi^{0}} (cm^{2}/(GeV/c)/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC1pi0_XSec_1Dppi0_antinu");
  fSettings.DefineAllowedSpecies("numu");
  fFluxCorrected = fSettings.Found("name", "fluxcorr");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(GeneralUtils::GetTopLevelDir() + "/data/MINERvA/CC1pi0/2015/ccpi0_ppi0.csv");
  SetCovarFromDiagonal();

  if (fFluxCorrected) {
    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) * 1.11);
    }
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void MINERvA_CC1pi0_XSec_1Dppi0_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(-13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  // 2015 does pion momentum in GeV
  double ppi0 = FitUtils::p(Ppi0);

  fXVar = ppi0;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1Dppi0_antinu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC1pi0_MINERvA_2015(event, EnuMin, EnuMax);
}
