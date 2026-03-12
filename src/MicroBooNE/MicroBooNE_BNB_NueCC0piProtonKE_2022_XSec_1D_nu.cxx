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

#include "MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu.h"

#include "TH1D.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu::MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu(nuiskey samplekey) {
  //********************************************************************
  fSettings = LoadSampleSettings(samplekey);

  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  if (!name.compare("MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1DProtonKE_nu")) {
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
                               "Signal: CC0pi 0p + Np\n"
                               "Contact: microboone_info@fnal.gov\n"
                               "Reference: Phys. Rev. D 106, L051102 (2022)\n"
                               "DOI: https://doi.org/10.1103/PhysRevD.106.L051102\n";

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
bool MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu::isSignal(FitEvent *event) {
  // Check CC nue
  if (!SignalDef::isCCINC(event, 12, EnuMin, EnuMax)) return false;
  
  // Veto events which don't have exactly 1 FS electron
  if (event->NumFSParticle(11) != 1 ) return false;
  
  // Veto events where the electron doesn't have KE > 30 MeV
  if (event->GetHMFSParticle(11)->KE() <= 30.0) return false;
  
  // Veto events with neutral pions of any energy
  if (event->NumFSParticle(111) != 0) return false;
  
  // Veto events with charged pions with KE > 40MeV
  if (event->NumFSParticle(211) != 0 && event->GetHMFSParticle(211)->KE() > 40.0) return false;
  if (event->NumFSParticle(-211) != 0 && event->GetHMFSParticle(-211)->KE() > 40.0) return false;

  // Proton KE distribution includes Np and 0p events
  // 0p events either have no final state proton or the leading proton has KE < 50MeV
  if ((event->NumFSParticle(2212) != 0 && event->GetHMFSParticle(2212)->KE() < 50.) || event->NumFSParticle(2212) == 0 ) {
    // additional phase space requirements on FS electron 
    // Electron energy > 0.5 GeV and cos th_e > 0.6
    TLorentzVector pe = event->GetHMFSParticle(11)->fP;
    if (pe.E() <= 500. || pe.CosTheta() < 0.6 ) return false;
  }
  
  // Events that pass selection are either NueCC0piNp or NueCC0pi0p
  return true;
}

void MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu::FillEventVariables(FitEvent *event) {

  // check that event is signal
  if (!isSignal(event)) return;

  // Must have an electron in the final state
  if (event->NumFSParticle(11) == 0 ) return;

  // initialize Proton KE variable
  double ProtonKE = 0.; 

  // if event has FS proton, calculate KE
  if (event->NumFSParticle(2212) != 0 ){
    // get highest momentum proton vector
    TLorentzVector vp = event->GetHMFSParticle(2212)->fP;
    ProtonKE = (vp.E() - vp.M()) / 1000.0 ; // in GeV
  }

  // Fill variable distributions
  if (fDist == kProtonKE) {
    fXVar = ProtonKE; 
  }
}    

void MicroBooNE_BNB_NueCC0piProtonKE_2022_XSec_1D_nu::ConvertEventRates() {
  // standard conversion
  Measurement1D::ConvertEventRates();
}

