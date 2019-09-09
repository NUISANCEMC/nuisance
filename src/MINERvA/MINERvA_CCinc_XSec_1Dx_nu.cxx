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

#include "MINERvA_CCinc_XSec_1Dx_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
MINERvA_CCinc_XSec_1Dx_nu::MINERvA_CCinc_XSec_1Dx_nu(std::string name,
                                                     std::string inputfile,
                                                     std::string type) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCinc_XSec_1Dx_nu sample. \n"
                        "Target: CH \n"
                        "Flux: MiniBooNE Forward Horn Current nue + nuebar \n"
                        "Signal: Any event with 1 muon, any nucleons, and no "
                        "other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(name, inputfile, type);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Reconstructed Bjorken x");
  fSettings.SetYTitle("d#sigma/dx (cm^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX/DIAG/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(2.0, 20.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetTitle("MINERvA_CCinc_XSec_1Dx_nu");

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
    QTHROW("target " << target << " was not found!");
  }
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  // Plot Setup -------------------------------------------------------
  double binsx[7] = {0, 0.1, 0.3, 0.7, 0.9, 1.1, 1.5};
  CreateDataHistogram(6, binsx);

  std::string basedir = FitPar::GetDataBase() + "/MINERvA/CCinc/";
  std::string smearfilename = "CCinc_" + target + "_x_smear.csv";
  SetSmearingMatrix(basedir + smearfilename, 6, 7);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CCinc_XSec_1Dx_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  ThetaMu = Pnu.Vect().Angle(Pmu.Vect());
  TLorentzVector q = Pnu - Pmu;
  double q0 = q.E() / 1000.0;
  double Emu = (Pmu.E()) / 1000.0;
  Enu_rec = Emu + q0;
  double Q2 = 4 * Enu_rec * Emu * sin(ThetaMu / 2) * sin(ThetaMu / 2);
  bjork_x = Q2 / 2. / q0 /
            ((PhysConst::mass_proton + PhysConst::mass_neutron) /
             2.); // Average nucleon masses

  fXVar = bjork_x;
  return;
}

//********************************************************************
bool MINERvA_CCinc_XSec_1Dx_nu::isSignal(FitEvent *event) {
  //*******************************************************************

  // Only look at numu events
  if (!SignalDef::isCCINC(event, 14, EnuMin, EnuMax))
    return false;

  // Restrict the phase space to theta < 17 degrees
  if (ThetaMu > 0.296706)
    return false;

  return true;
};

//********************************************************************
void MINERvA_CCinc_XSec_1Dx_nu::ScaleEvents() {
  //********************************************************************

  this->ApplySmearingMatrix();
  Measurement1D::ScaleEvents();
}
