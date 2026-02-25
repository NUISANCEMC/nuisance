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
// Only considers pure Np measurements : E_e, cos th_e, and cos th_p

#include "MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu::MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu(nuiskey samplekey) {
  //********************************************************************
  fSettings = LoadSampleSettings(samplekey);

  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  // work out which sample you need, and set axii
  if (!name.compare("MicroBooNE_BNB_NueCC0piNp_2022_XSec_1DElecCosTheta_nu")) {
    fDist = kElecCosTheta;
    objSuffix = "ElecCosTheta";
    fSettings.SetXTitle("cos#theta_{e}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{e} (cm^{2}/nucleon)");
  }
  else if (!name.compare("MicroBooNE_BNB_NueCC0piNp_2022_XSec_1DElecEnergy_nu")) {
    fDist = kElecEnergy;
    objSuffix = "ElecEnergy";
    fSettings.SetXTitle("E_{e} (GeV)");
    fSettings.SetYTitle("d#sigma/dE_{e} (cm^{2}/GeV/nucleon)");
  }
  else if (!name.compare("MicroBooNE_BNB_NueCC0piNp_2022_XSec_1DProtonCosTheta_nu")) {
    fDist = kProtonCosTheta;
    objSuffix = "ProtonCosTheta";
    fSettings.SetXTitle("cos#theta_{P}");
    fSettings.SetYTitle("d#sigma/dcos#theta_{P} (cm^{2}/nucleon)");
  }
  else {
    assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n"
                               "Target: Ar\n"
                               "Flux: BNB FHC Nue\n"
                               "Signal: CC0piNp\n";
  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.8);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("nue");
  FinaliseSampleSettings();
  
  // Load data ---------------------------------------------------------
  std::string inputFile = FitPar::GetDataBase() +
              "/MicroBooNE/NueCC0pi/NueCC0pi_xsec_data_cov.root";
  SetDataFromRootFile(inputFile, "DataXSec_" + objSuffix);
  ScaleData(1E-39);

  // ScaleFactor for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") *1E-38) / double(fNEvents) / TotalIntegratedFlux();

  SetCovarFromRootFile(inputFile, "NuisanceScaledCovarianceMatrix_" + objSuffix);
  
  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
}

// check if event meets signal definition
bool MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu::isSignal(FitEvent *event) {
  return SignalDef::MicroBooNE::isNueCC0piNp(event, EnuMin, EnuMax);
}

void MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu::FillEventVariables(FitEvent *event) {  
  // check that event is signal
  if (!isSignal(event)) return;

  // Must have an electron and at least one proton in the final state
  if (event->NumFSParticle(11) == 0 ) return;
  if (event->NumFSParticle(2212) == 0) return;

  // get highest momentum electron and proton vectors
  TLorentzVector ve = event->GetHMFSParticle(11)->fP;
  TLorentzVector vp = event->GetHMFSParticle(2212)->fP;

  // Calculate electron variables
  double ElecEnergy = ve.E() / 1000.0; // electron energy in GeV
  double ElecCosTheta = ve.CosTheta(); // electron angle
 
  // Calculte proton variables
  double ProtonCosTheta = vp.CosTheta();
  double ProtonKE = (vp.E() - vp.M()) / 1000.0; // in GeV 
  
  // Fill variable distributions
  if (fDist == kElecCosTheta) {
    fXVar = ElecCosTheta;
  }
  else if (fDist == kElecEnergy) {
    fXVar = ElecEnergy;
  }
  else if (fDist == kProtonCosTheta) {
    fXVar = ProtonCosTheta;
  }
}    

void MicroBooNE_BNB_NueCC0piNp_2022_XSec_1D_nu::ConvertEventRates() {
  // standard conversion
  Measurement1D::ConvertEventRates();
}

