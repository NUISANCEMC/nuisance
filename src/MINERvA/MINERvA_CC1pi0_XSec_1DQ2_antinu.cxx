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

#include "MINERvA_CC1pi0_XSec_1DQ2_antinu.h"


//********************************************************************
MINERvA_CC1pi0_XSec_1DQ2_antinu::MINERvA_CC1pi0_XSec_1DQ2_antinu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC1pi0_XSec_1DQ2_antinu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current numubar \n" \
                        "Signal: Any event with 1 muon, 1 pion, no other tracks \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/(GeV^{2})/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC1pi0_XSec_1DQ2_antinu");
  fSettings.DefineAllowedSpecies("numu");
  // fFluxCorrected = fSettings.Found("name", "fluxcorr");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(GeneralUtils::GetTopLevelDir() + "/data/MINERvA/CC1pi0/2016/anu-cc1pi0-xsec-q2.csv");
  for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
    fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
  }

  SetCorrelationFromTextFile(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2016/anu-cc1pi0-correlation-q2.csv");

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


void MINERvA_CC1pi0_XSec_1DQ2_antinu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(-13) == 0) return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Q2      = -999;

  if (hadMass < 1800) {
    Q2 = -1*(Pnu-Pmu).Mag2()/1.E6;
  }

  fXVar = Q2;

  return;
};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1DQ2_antinu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCC1pi0_MINERvA_2016(event, EnuMin, EnuMax);
}
