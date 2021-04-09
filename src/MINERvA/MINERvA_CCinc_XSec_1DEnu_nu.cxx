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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CCinc_XSec_1DEnu_nu.h"

//********************************************************************
MINERvA_CCinc_XSec_1DEnu_nu::MINERvA_CCinc_XSec_1DEnu_nu(std::string name,
                                                         std::string inputfile,
                                                         std::string type) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCinc_XSec_1DEnu_nu sample. \n"
                        "Target: CH \n"
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n"
                        "Signal: Any event with 1 muon, any nucleons, and no "
                        "other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(name, inputfile, type);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle("d#sigma/dE_{#nu} (cm^{2}/GeV/nucleon)");
  fSettings.SetAllowedTypes("FIX/DIAG/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(2.0, 20.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetTitle("MINERvA_CCinc_XSec_1DEnu_nu");

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
    NUIS_ABORT("target " << target << " was not found!");
  }
  // fSettings.SetSmearingInput( FitPar::GetDataBase() +
  // "/MINERvA/CCinc/CCinc_"+target+"_x_smear.csv" );

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  double binsx[9] = {2, 3, 4, 5, 6, 8, 10, 15, 20};
  CreateDataHistogram(8, binsx);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CCinc_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  fXVar = Pnu.E() / 1000.;
  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  return;
}

//********************************************************************
bool MINERvA_CCinc_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  //*******************************************************************

  if (!SignalDef::isCCINC(event, 14, this->EnuMin, this->EnuMax))
    return false;

  // Restrict the phase space to theta < 17 degrees
  if (ThetaMu > 0.296706)
    return false;

  return true;
};
