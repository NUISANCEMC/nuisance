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
#include "MINERvA_CCNpip_XSec_1DQ2_nu.h"


//********************************************************************
MINERvA_CCNpip_XSec_1DQ2_nu::MINERvA_CCNpip_XSec_1DQ2_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCNpip_XSec_1DQ2_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ^{2} (cm^{2}/GeV^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CCNpip_XSec_1DQ2_nu");
  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-q2.csv" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-q2.csv");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  // MINERvA has the error quoted as a percentage of the cross-section
  // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
  for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
    fDataHist->SetBinError(i + 1, fDataHist->GetBinContent(i + 1) * (fDataHist->GetBinError(i + 1) / 100.));
  }

  SetCorrelationFromTextFile(fSettings.GetCovarInput() );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


//********************************************************************
void MINERvA_CCNpip_XSec_1DQ2_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (event->NumFSParticle(13) == 0) return;
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  double q2 = -1 * (Pnu - Pmu).Mag2() / 1.E6;

  fXVar = q2;

};

//********************************************************************
bool MINERvA_CCNpip_XSec_1DQ2_nu::isSignal(FitEvent *event) {
//********************************************************************
  return SignalDef::isCCNpip_MINERvA(event, EnuMin, EnuMax, false);
}
