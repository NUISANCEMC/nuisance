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

// MicroBooNE BNB nue CC0pi https://doi.org/10.1103/PhysRevD.106.L051102

#include "MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu::MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu(nuiskey samplekey) {
  //********************************************************************
  fSettings = LoadSampleSettings(samplekey);

  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  // work out which sample you need, and set axii
  if (!name.compare("MicroBooNE_BNB_NueCC0pi_2022_XSec_1DElecCosTheta_nu")) {
    fDist = kElecCosTheta;
    objSuffix = "ElecCosTheta";
    fSettings.SetXTitle("cos#theta_{e}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{e} (cm^{2}/nucleon)");
  }
  else if (!name.compare("MicroBooNE_BNB_NueCC0pi_2022_XSec_1DElecEnergy_nu")) {
    fDist = kElecEnergy;
    objSuffix = "ElecEnergy";
    fSettings.SetXTitle("E_{e} (GeV)");
    fSettings.SetYTitle("d#sigma/dE_{e} (cm^{2}/GeV/nucleon)");
  }
  else if (!name.compare("MicroBooNE_BNB_NueCC0pi_2022_XSec_1DProtonCosTheta_nu")) {
    fDist = kProtonCosTheta;
    objSuffix = "ProtonCosTheta";
    fSettings.SetXTitle("cos#theta_{P}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{P} (cm^{2}/nucleon)");
  }
  else if (!name.compare("MicroBooNE_BNB_NueCC0pi_2022_XSec_1DProtonKE_nu")) {
    fDist = kProtonKE;
    objSuffix = "ProtonKE";
    fSettings.SetXTitle("KE_{P} (GeV)");
    fSettings.SetYTitle("d#sigma/dKE_{P} (cm^{2}/GeV/nucleon)"); 
  }
  else {
  assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n"
                               "Target: Ar\n"
                               "Flux: BNB FHC Nue\n"
                               "Signal: CC0pi\n";
  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.0);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("nue");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------
  std::string inputFile = FitPar::GetDataBase() +
              "/MicroBooNE/NueCC0pi/NueCC0pi_xsec_data_cov.root";
  SetDataFromRootFile(inputFile, "DataXSec_" + objSuffix);
  ScaleData(1E-39);

  // ScaleFactor for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1.0E-38 /
                 TotalIntegratedFlux();

  SetCovarFromRootFile(inputFile, "NuisanceScaledCovarianceMatrix_" + objSuffix);
  
  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
}

bool MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu::isSignal(FitEvent *event) {
  return SignalDef::MicroBooNE::isNueCC0pi(event, EnuMin, EnuMax);
}

void MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu::FillEventVariables(FitEvent *event) {

  if (!isSignal(event)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }

  // Must have an electron and proton in the final state
  if (event->NumFSParticle(11) == 0) return; 
  if (event->NumFSParticle(2212) == 0) return;

  // get highest momentum FS particle vectors
  TLorentzVector ve = event->GetHMFSParticle(11)->fP;
  TLorentzVector vp = event->GetHMFSParticle(2212)->fP;

  // calculate variables
  // Electron energy in GeV
  double ElecEnergy = ve.E() / 1000.0;
  
  // Electron angle
  double ElecCosTheta = ve.CosTheta();
  
  // Proton angle
  double ProtonCosTheta = vp.CosTheta();
 
  // Proton energy
  double ProtonEnergy = vp.E() ; // in MeV
  double ProtonMass = vp.M() ; // in MeV / c^2
  double ProtonKE = ProtonEnergy - ProtonMass; // in MeV

  // events separated into 1eNp0pi and 1e0p0pi
  // 1e0p0pi events fall below proton visibility threshold and are subject to additional phase space requirements
  double ProtonKEThreshold = 50.0; //MeV
  double EEnergyThresholdFor0p = 0.5; // in GeV
  double ECosThetaThresholdFor0p = 0.6;
  bool is1e0p0pi = false;

  // if no protons above visibility threshold and phase space requirements are met, then event is 1e0p0pi
  if (ProtonKE < ProtonKEThreshold && ElecEnergy > EEnergyThresholdFor0p && ElecCosTheta > ECosThetaThresholdFor0p) {
   is1e0p0pi = true;
  }

  // 1eNp0pi for E_e, cos theta_e, cos theta_p, KE_p
  if (ProtonKE >= ProtonKEThreshold) {
    // Assign variables to distributions
    if (fDist == kElecCosTheta) {
      fXVar = ElecCosTheta;
    }
    else if (fDist == kElecEnergy) {
      fXVar = ElecEnergy;
    }
    else if (fDist == kProtonCosTheta) {
      fXVar = ProtonCosTheta;
    }
    else if (fDist == kProtonKE) {
      fXVar = ProtonKE / 1000.0; // convert to GeV
     }
  }

  // KE_p also containss 0p 
  if (is1e0p0pi == true) {
     if (fDist == kProtonKE) {
      fXVar = ProtonKE / 1000.0; // convert to GeV
    }
  }
}

void MicroBooNE_BNB_NueCC0pi_2022_XSec_1D_nu::ConvertEventRates() {
  // standard conversion
  Measurement1D::ConvertEventRates();


}
