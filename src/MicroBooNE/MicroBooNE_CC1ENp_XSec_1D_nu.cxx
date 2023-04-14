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

  std::string ObjSuffix;

  if (!name.compare("MicroBooNE_CC1ENp_XSec_1DElecEnergy_nu")) {
    fDist = kElecEnergy;
    ObjSuffix = "ElecEnergy";
    fSettings.SetXTitle("Electron Energy (GeV)");
    fSettings.SetYTitle("d#sigma/dE_{e} (cm^{2}/^{40}Ar)");
  }
  else {
    assert(false);
  }

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
  std::string inputFile = FitPar::GetDataBase() + "/MicroBooNE/CC1ENp/CC1ENp_data_MC_cov_data_smear_Release.root";
  SetDataFromRootFile(inputFile, "DataXsec_"+ObjSuffix);
  ScaleData(1E-38);

  // ScaleFactor for DiffXSec/cm2/Nucleus
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1E-38 / TotalIntegratedFlux();

  SetCovarFromRootFile(inputFile, "CovarianceMatrix_"+ObjSuffix);

  // Load smearing matrix ---------------------------------------------------------
  // Set up the additional smearing matrix Ac
  // All the MC predictions need to be multiplied by Ac to move to the regularized phase space

  TFile* inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());
  TH2D* hsmear = (TH2D*)inputRootFile->Get(("SmearingMatrix_"+ObjSuffix).c_str());
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
  if (fDist == kElecEnergy) {
    if (event->NumFSParticle(11) == 0) return;
    fXVar = event->GetHMFSParticle(11)->fP.E() / 1000;
  }
}


void MicroBooNE_CC1ENp_XSec_1D_nu::ConvertEventRates() {
  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply Weiner-SVD additional smearing Ac
  int nBins = fMCHist->GetNbinsX();
  TVectorD OriginalMC(nBins);
  for (int iBin=0;iBin<nBins;iBin++) {
    OriginalMC(iBin) = fMCHist->GetBinContent(iBin+1);
  }
  TVectorD SmearedMC = (*fSmearingMatrix) * OriginalMC;

  for (int iBin=0;iBin<nBins;iBin++) {
    fMCHist->SetBinContent(iBin+1, SmearedMC(iBin));
  }
}

