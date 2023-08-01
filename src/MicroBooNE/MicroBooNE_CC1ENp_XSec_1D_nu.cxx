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

#include "MicroBooNE_CC1ENp_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_CC1ENp_XSec_1D_nu::MicroBooNE_CC1ENp_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");

  std::string DataFileName = "/MicroBooNE/CC1ENp/CCNp_nue_data_covmat_smearmat_Release.root";

  std::string ObjSuffix;

  if (!name.compare("MicroBooNE_CC1ENp_XSec_1DElecEnergy_nu")) {
    ObjSuffix = "elec_e";
    fDist = kElecEnergy;
    fSettings.SetXTitle("Electron Energy (GeV)");
    fSettings.SetYTitle("d#sigma/dE (cm^{2}/GeV/Nucleus)");
  }
  else if (!name.compare("MicroBooNE_CC1ENp_XSec_1DOpeningAngle_nu")) {
    ObjSuffix = "opening_angle";
    fDist = kOpeningAngle;
    fSettings.SetXTitle("Opening Angle cos #theta_{ep}");
    fSettings.SetYTitle("d#sigma/d(cos #theta_{ep}) (cm^{2}/GeV/Nucleus)");
  }
  else if (!name.compare("MicroBooNE_CC1ENp_XSec_1DTrueVisibleEnergy_nu")) {
    ObjSuffix = "true_e_visible";
    fDist = kTrueVisibleEnergy;
    fSettings.SetXTitle("True Visible Energy [GeV]");
    fSettings.SetYTitle("d#sigma/dE (cm^{2}/GeV/Nucleus)");
  }
  else {
    assert(false);
  }

  std::string DataHistName = "unf_xsec_"+ObjSuffix;
  std::string CovMatName = "unfcov_xsec_"+ObjSuffix;
  std::string ACMatName = "Ac_"+ObjSuffix;

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: NUMI nue\n" \
                        "Signal: CC1ENp\n";

  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("nue");
  FinaliseSampleSettings();

  // Load data --------------------------------------------------------- 
  std::string inputFile = FitPar::GetDataBase()+DataFileName;
  SetDataFromRootFile(inputFile, DataHistName);
  ScaleData(1E-38);

  // ScaleFactor for DiffXSec/cm2/Nucleus (According to L.P. 11/05/23)
  //fScaleFactor = GetEventHistogram()->Integral("width") / (double(fNEvents) * TotalIntegratedFlux("",0.05,20)); // Standard differential cross section per nucleon 
  fScaleFactor = GetEventHistogram()->Integral("width") / (double(fNEvents) * TotalIntegratedFlux()); // Standard differential cross section per nucleon 
  //fScaleFactor *= 40; // Convert to per nucleus (Ar40)
  fScaleFactor *= 1E-38; // Convert units

  std::cout << "fScaleFactor:" << fScaleFactor << std::endl;
  std::cout << "TotalIntegratedFlux():" << TotalIntegratedFlux() << std::endl;

  SetCovarFromRootFile(inputFile, CovMatName);

  // Load smearing matrix ---------------------------------------------------------
  // Set up the additional smearing matrix Ac
  // All the MC predictions need to be multiplied by Ac to move to the regularized phase space

  TFile* inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());
  TH2D* hsmear = (TH2D*)inputRootFile->Get(ACMatName.c_str());
  assert(hsmear);

  int nrows = hsmear->GetNbinsX();
  int ncols = hsmear->GetNbinsY();
  fSmearingMatrix = new TMatrixD(nrows, ncols);
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<ncols; j++) {
      (*fSmearingMatrix)(i,j) = hsmear->GetBinContent(i+1, j+1);
    }
  }

  inputRootFile->Close();

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
};


bool MicroBooNE_CC1ENp_XSec_1D_nu::isSignal(FitEvent* event) {
  return SignalDef::MicroBooNE::isCC1ENp(event, EnuMin, EnuMax);
};


void MicroBooNE_CC1ENp_XSec_1D_nu::FillEventVariables(FitEvent* event) {
  fXVar = -999.;
  if (event->NumFSParticle(11) == 0) return;
  if (event->NumFSParticle(2212) == 0) return;

  if (fDist == kElecEnergy) {
    fXVar = event->GetHMFSParticle(11)->fP.E()/1000.0;
  }
  else if (fDist == kOpeningAngle) {
    TVector3 HMProtonVec = event->GetHMFSParticle(2212)->P3();
    TVector3 HMElectronVec = event->GetHMFSParticle(11)->P3();

    fXVar = HMProtonVec.Angle(HMElectronVec);
  }
  else if (fDist == kTrueVisibleEnergy) {
    int nParticles = event->NParticles();

    double TrueVisibleEnergy = 0.;
    for (int iParticle=0;iParticle<nParticles;iParticle++) {
      FitParticle* Particle = event->GetParticle(iParticle);
      if (!Particle->IsFinalState()) {
	continue;
      }

      if (Particle->PDG() == 11) { //Electron
	TrueVisibleEnergy += Particle->E();
      }
      else if (Particle->PDG() == 2212) { //Proton
	TrueVisibleEnergy += Particle->KE();
      }
    }
    
    fXVar = TrueVisibleEnergy/1000.0;
  }
}


void MicroBooNE_CC1ENp_XSec_1D_nu::ConvertEventRates() {
  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply Weiner-SVD additional smearing Ac - Needs to be applied on 'event rate' units then converted back to 'xsec units'
  int nBins = fMCHist->GetNbinsX();
  double Flux_CV = 6699174026.68;
  double nTargets = 4.240685683288815e+31;
  
  std::cout << "Original MC (Xsec Units)" << std::endl;
  for (int iBin=0;iBin<nBins;iBin++) {
    std::cout << iBin << " " << fMCHist->GetBinContent(iBin+1) << std::endl;
  }

  // First convert to event rate units
  TVectorD MC_EVRUnits(nBins);
  for (int iBin=0;iBin<nBins;iBin++) {
    MC_EVRUnits(iBin) = fMCHist->GetBinContent(iBin+1) * nTargets * Flux_CV * fMCHist->GetXaxis()->GetBinWidth(iBin+1);
  }

  std::cout << "MC in EVR Units" << std::endl;
  for (int iBin=0;iBin<nBins;iBin++) {
    std::cout << iBin << " " << MC_EVRUnits(iBin) << std::endl;
  }

  // Apply smearing
  TVectorD SmearedMC_EVRUnits = (*fSmearingMatrix) * MC_EVRUnits;

  std::cout << "Smeared MC in EVR Units" << std::endl;
  for (int iBin=0;iBin<nBins;iBin++) {
    std::cout << iBin << " " << SmearedMC_EVRUnits(iBin) << std::endl;
  }

  // Convert back to xsec units
  TVectorD SmearedMC_XSecUnits(nBins);
  for (int iBin=0;iBin<nBins;iBin++) {
    SmearedMC_XSecUnits(iBin) = SmearedMC_EVRUnits(iBin) / (nTargets * Flux_CV * fMCHist->GetXaxis()->GetBinWidth(iBin+1));
  }

  std::cout << "Smeared MC in XSec Units" << std::endl;
  for (int iBin=0;iBin<nBins;iBin++) {
    std::cout << iBin << " " << SmearedMC_XSecUnits(iBin) << std::endl;
  }

  // Then copy results back to histogram
  for (int iBin=0;iBin<nBins;iBin++) {
    fMCHist->SetBinContent(iBin+1, SmearedMC_XSecUnits(iBin));
  }

}

