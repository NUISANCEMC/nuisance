// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "MINERvA_CCDIS_XSec_1DEnu_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
MINERvA_CCDIS_XSec_1DEnu_nu::MINERvA_CCDIS_XSec_1DEnu_nu(std::string name,
                                                         std::string inputfile,
                                                         std::string type) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCDIS_XSec_1DEnu_nu sample.";

  // Setup common settings
  fSettings = LoadSampleSettings(name, inputfile, type);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("d#sigma/dE_{#nu} (cm^{2}/GeV/nucleon)");
  fSettings.SetAllowedTypes("FIX/DIAG/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(5.0, 50.0);
  fSettings.DefineAllowedTargets("Pb,Fe,C,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetTitle("MINERvA_CCDIS_XSec_1DEnu_nu");

  target = "";
  if (name.find("C12") != std::string::npos)
    target = "C12";
  else if (name.find("Fe56") != std::string::npos)
    target = "Fe56";
  else if (name.find("Pb208") != std::string::npos)
    target = "Pb208";
  if (name.find("DEN") != std::string::npos)
    target = "CH";
  if (target == "") {
    NUIS_ERR(WRN, "target " << target << " was not found!");
  }
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  double binsx[8] = {5.00, 10.00, 15.00, 20.00, 25.00, 30.00, 40.00, 50.00};
  CreateDataHistogram(7, binsx);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CCDIS_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  // Need to calculate Q2 and W using the MINERvA method
  double Enu = Pnu.E() / 1000.;
  double Emu = Pmu.E() / 1000.;
  double theta = FitUtils::th(Pnu, Pmu);

  Q2 = 4 * Enu * Emu * sin(theta / 2.) * sin(theta / 2.);
  W = sqrt(PhysConst::mass_nucleon * PhysConst::mass_nucleon +
           2 * PhysConst::mass_nucleon * (Enu - Emu) - Q2);
  fXVar = Enu;

  return;
}

//********************************************************************
bool MINERvA_CCDIS_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  //*******************************************************************

  if (!SignalDef::isCCINC(event, 14, this->EnuMin, this->EnuMax))
    return false;

  // Restrict the phase space to theta < 17 degrees
  if (!SignalDef::IsRestrictedAngle(event, 14, 13, 17))
    return false;

  if (Q2 < 1.0)
    return false;
  if (W < 2.0)
    return false;

  return true;
};
