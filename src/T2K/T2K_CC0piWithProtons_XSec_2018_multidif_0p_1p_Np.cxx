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

#include "T2K_SignalDef.h"

#include "T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np.h"

//********************************************************************
T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::
    T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np sample. \n"
                        "Target: CH \n"
                        "Flux: T2K 2.5 degree off-axis (ND280)  \n"
                        "Signal: CC0piNp (N>=0) with p_p>500MeV \n"
                        "https://doi.org/10.1103/PhysRevD.98.032003 \n"
                        "Data release: https://t2k-experiment.org/results/2018-transverse-cc0pi/ \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Bin number");
  // fSettings.SetYTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fAnalysis = 1;

  // CCQELike plot information
  fSettings.SetTitle(fName);
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Set useCC0pi0p, useCC0pi1p, and useCC0piNp, first initialize to false;
  useCC0pi0p = false;
  useCC0pi1p = false;
  useCC0piNp = false;

  if (fName == "T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np"){
    useCC0pi0p = true;
    useCC0pi1p = true;
    useCC0piNp = true;
  }
  else if (fName == "T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p"){
    useCC0pi0p = true;
    useCC0pi1p = true;
  }
  else if (fName == "T2K_CC0piWithProtons_XSec_2018_multidif_0p"){
    useCC0pi0p = true;
  }
  else if (fName == "T2K_CC0piWithProtons_XSec_2018_multidif_1p"){
    useCC0pi1p = true;
  }


  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  // fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) *
  // 1E-38 / (TotalIntegratedFlux()));
  fScaleFactor = ((GetEventHistogram()->Integral("width") / (fNEvents + 0.)) *
                  10 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

bool T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::isSignal(FitEvent *event) {
  // If looking at all subsamples, only requirement is that this is a CC0pi event
  if (useCC0pi0p && useCC0pi1p && useCC0piNp)
    return SignalDef::isCC0pi(event, 14, EnuMin, EnuMax);

  // Otherwise, evaluate each relevant signal definition separately
  if (useCC0pi0p && SignalDef::isT2K_CC0pi0p(event, EnuMin, EnuMax))
    return true;
  if (useCC0pi1p && SignalDef::isT2K_CC0pi1p(event, EnuMin, EnuMax))
    return true;
  if (useCC0piNp && SignalDef::isT2K_CC0piNp(event, EnuMin, EnuMax))
    return true;

  // If you get here, the event has failed and is not signal
  return false;
};

void T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::FillEventVariables(
    FitEvent *event) {

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  // Dummy proton variables if we don't have a proton
  double pp = -999;
  double CosThetaP = -999;
  // Check if we do have a proton and fill variables
  if (event->NumFSParticle(2212) > 0){
    TLorentzVector Pp = event->GetHMFSParticle(2212)->fP;
    pp = Pp.Vect().Mag() / 1000.;
    CosThetaP = cos(Pnu.Vect().Angle(Pp.Vect()));
  }

  // How many protons above threshold?
  std::vector<FitParticle *> protons = event->GetAllFSProton();
  int nProtonsAboveThresh = 0;
  for (size_t i = 0; i < protons.size(); i++) {
    if (protons[i]->p() > 500)
      nProtonsAboveThresh++;
  }

  // Get bin number in total 1D histogram
  int binnumber = Get1DBin(nProtonsAboveThresh, pmu, CosThetaMu, pp, CosThetaP);

  // I'm hacking this to fit in the Measurement1D framework, but it's going to be super ugly - apologies...
  // The 1D histogram handled by NUISANCE is defined in terms of bin number, so that has to be fXVar
  // Actually needs to be binnumber-0.5 because it's treating it as a variable
  fXVar = binnumber-0.5;

  // Then fill variables so I can use them to fill the slice histograms in FillHistograms()
  fPP = pp;
  fPMu = pmu;
  fCosThetaP = CosThetaP;
  fCosThetaMu = CosThetaMu;
  fNp = nProtonsAboveThresh;

  // Also set mode so the mode histogram works
  Mode = event->Mode;

  return;
};

void T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::FillHistograms() {

  Measurement1D::FillHistograms();
  if (Signal) {
    // fMCHist_Fine2D->Fill(fXVar, fYVar, Weight);
    if (useCC0pi0p && fNp == 0){
      fMCHist_CC0pi0pCosTheta->Fill(fCosThetaMu, Weight);
    }
    else if (useCC0pi1p && fNp == 1){
      fMCHist_CC0pi1pCosTheta->Fill(fCosThetaMu, Weight);
    }
    FillMCSlice(fNp, fPMu, fCosThetaMu, fPP, fCosThetaP, Weight);
  }
}

void T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::FillMCSlice(int nProtonsAboveThresh, double pmu, double CosThetaMu, double pp, double CosThetaP, double w) {
// Get slice number for 1D CosThetaMu slice
  int CosThetaMuSliceNo = GetCosThetaMuSlice(nProtonsAboveThresh, CosThetaMu);
  // If sliceno is valid (not negative), fill the relevant slice
  if (CosThetaMuSliceNo < 0) return;
  // CC0pi0p slices: fill with pmu
  if (useCC0pi0p && nProtonsAboveThresh == 0 && CosThetaMuSliceNo < 10){
    fMCHist_Slices[CosThetaMuSliceNo]->Fill(pmu, w);
  }
  // CC0pi1p slices: fill with CosThetaP
  if (useCC0pi1p && nProtonsAboveThresh == 1){
    fMCHist_Slices[CosThetaMuSliceNo]->Fill(CosThetaP, w);

    // If we're looking at CC0pi1p, also fill the CosThetaMu-CosThetaP slices with PP
    int CC0pi1p2DSliceNo = GetCC0pi1p2DSlice(nProtonsAboveThresh, CosThetaMu, CosThetaP);
    if (CC0pi1p2DSliceNo < 0) return;
    fMCHist_Slices[CC0pi1p2DSliceNo]->Fill(pp, w);
  }
}

void T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::SetHistograms() {

  // Read in 1D Data Histograms
  fInputFile = new TFile(
      (FitPar::GetDataBase() + "/T2K/CC0pi/STV/multidif_results.root")
          .c_str(),
      "READ");
  // fInputFile->ls();

  // Read in 1D Data
  TH1D *tempDataHist = (TH1D *)fInputFile->Get("Result");

  // Read in covariance matrix
  TH2D *tempcov = (TH2D *)fInputFile->Get("CovarianceMatrix");

  // The input data and covariance matrix include bins for CC0pi0p, CC0pi1p, and CC0piNp. We may not want them all if we only want to look at one or two of the sub-samples, so go through and only keep the bins we want
  // CC0pi0p: bins 1-60 -> 60 bins
  // CC0pi1p: bins 61-92 -> 32 bins
  // CC0piNp: bin 93 -> 1 bin
  int n_binskeep = 0;
  if (useCC0pi0p) n_binskeep += 60;
  if (useCC0pi1p) n_binskeep += 32;
  if (useCC0piNp) n_binskeep += 1;

  fDataHist = new TH1D("DataHist", tempDataHist->GetTitle(),n_binskeep,0,n_binskeep);
  fFullCovar = new TMatrixDSym(n_binskeep);

  int i_binskeep = 1;
  for (int i_allbins=1; i_allbins<tempDataHist->GetNbinsX()+1; i_allbins++){
    if ((i_allbins >=1 && i_allbins <=60) && !useCC0pi0p) continue;
    if ((i_allbins >= 61 && i_allbins <=92) && !useCC0pi1p) continue;
    if ((i_allbins == 93) && !useCC0piNp) continue;

    fDataHist->SetBinContent(i_binskeep,tempDataHist->GetBinContent(i_allbins));
    fDataHist->SetBinError(i_binskeep,tempDataHist->GetBinError(i_allbins));

    int j_binskeep = 1;
    for (int j_allbins = 1; j_allbins < tempDataHist->GetNbinsX()+1; j_allbins++){
      if ((j_allbins >=1 && j_allbins <=60) && !useCC0pi0p) continue;
      if ((j_allbins >= 61 && j_allbins <=92) && !useCC0pi1p) continue;
      if ((j_allbins == 93) && !useCC0piNp) continue;

      // std::cout << i_allbins << ", " << j_allbins << " -- " << i_binskeep-1 << ", " << j_binskeep-1 << " -- " << tempcov->GetBinContent(i_allbins, j_allbins) << std::endl;

      (*fFullCovar)(i_binskeep-1,j_binskeep-1) = tempcov->GetBinContent(i_allbins, j_allbins)*1e38*1e38;
      j_binskeep++;
    } // end loop over j_allbins

    i_binskeep++;
  }

  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

  // Make 1D data histogram
  TH1D *linearResult = new TH1D(*fDataHist);
  // Set name based on what subsamples we are looking at
  if (useCC0pi0p && useCC0pi1p && useCC0piNp){
    linearResult->SetName("T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np_data");
  }
  else if (useCC0pi0p && useCC0pi1p && !useCC0piNp){
    linearResult->SetName("T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_data");
  }
  else if (useCC0pi0p && !useCC0pi1p && !useCC0piNp){
    linearResult->SetName("T2K_CC0piWithProtons_XSec_2018_multidif_0p_data");
  }
  else if (!useCC0pi0p && useCC0pi1p && !useCC0piNp){
    linearResult->SetName("T2K_CC0piWithProtons_XSec_2018_multidif_1p_data");
  }
  else{
    linearResult->SetName("T2K_CC0piWithProtons_XSec_2018_multidif_data");
  }
  SetAutoProcessTH1(linearResult, kCMD_Write);


  // Fine histograms - don't implement for now (this is copied from T2K_CC0pi1p_XSec_3DPcoscos_nu)
  // fMCHist_Fine2D = new TH2D("T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np_Fine2D",
  //                           "T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np_Fine2D",
  //                           400, 0.0, 30.0, 100, -1.0, 1.0);
  // SetAutoProcessTH1(fMCHist_Fine2D);


  // The code below converts a relative covariance matrix to an absolute one. I think the input is absolute so we don't need it, but come back to this if the results look weird
  // for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
  //   for (int j = 0; j < fDataHist->GetNbinsX(); j++) {
  //     //(*fFullCovar)(i,j) = tempcov->GetBinContent(i+1, j+1);
  //     (*fFullCovar)(i, j) = tempcov->GetBinContent(i + 1, j + 1) *
  //                           fDataHist->GetBinContent(i + 1) *
  //                           fDataHist->GetBinContent(j + 1);
  //     if (i == j)
  //       fDataHist->SetBinError(i + 1, sqrt((*fFullCovar)(i, j)));
  //     // if(i==j) std::cout << "For bin " << i+1 << ", relative covariance was "
  //     // << tempcov->GetBinContent(i+1,j+1); if(i==j) std::cout << ". Absolute
  //     // covariance is now " << (*fFullCovar)(i,j) << ", linear xsec is: " <<
  //     // fDataHist->GetBinContent(i+1) << std::endl;
  //   }
  // }


  // Read in data slice histograms and make MC slices
  // Slices are stored in slightly different ways for 0p and 1p samples
  // CC0pi0p: folder NoProtonsAbove500MeV
  //         -> TH1D ResultInMuonCosTheta
  //         -> TH1D MuonCosThetaSlice_i where i = 0 to 9
  // CC0pi1p: folder OneProtonAbove500MeV
  //         -> TH1D ResultInMuonCosTheta
  //         -> TH1D MuonCosThetaSlice_1D_i where i = 0 to 3
  //         -> TH1D MuCThSlice_1_PCthSlice_0
  //         -> TH1D MuCThSlice_2_PCthSlice_0
  //         -> TH1D MuCThSlice_2_PCthSlice_1
  //         -> TH1D MuCThSlice_3_PCthSlice_0
  // We also have proton multiplicity, in folder ProtonMultiplicity
  //         -> TH1D Result
  //         -> TH2D CovarianceMatrix
  // TODO add this as a separate sample? Don't implement here

  // CC0pi0p slices
  if (useCC0pi0p){
    fDataHist_CC0pi0pCosTheta = (TH1D*)fInputFile->Get("NoProtonsAbove500MeV/ResultInMuonCosTheta")->Clone("T2K_CC0pi0p_XSec_2018_MuonCosTheta_data");
    fMCHist_CC0pi0pCosTheta = (TH1D*)fDataHist_CC0pi0pCosTheta->Clone("T2K_CC0pi0p_XSec_2018_MuonCosTheta_MC");
    fMCHist_CC0pi0pCosTheta->Reset();
    SetAutoProcessTH1(fDataHist_CC0pi0pCosTheta, kCMD_Write);
    SetAutoProcessTH1(fMCHist_CC0pi0pCosTheta, kCMD_Reset, kCMD_Scale, kCMD_Write);

    for (int i=0; i<=9; i++){
      fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("NoProtonsAbove500MeV/MuonCosThetaSlice_%i", i))->Clone(Form("T2K_CC0pi0p_XSec_2018_Data_Slice%i", i)));
      fMCHist_Slices.push_back((TH1D*)fInputFile->Get(Form("NoProtonsAbove500MeV/MuonCosThetaSlice_%i", i))->Clone(Form("T2K_CC0pi0p_XSec_2018_MC_Slice%i", i)));
    } // end loop over i
  }

  // CC0pi1p slices
  if (useCC0pi1p){
    fDataHist_CC0pi1pCosTheta = (TH1D*)fInputFile->Get("OneProtonAbove500MeV/ResultInMuonCosTheta")->Clone("T2K_CC0pi1p_XSec_2018_MuonCosTheta_data");
    fMCHist_CC0pi1pCosTheta = (TH1D*)fDataHist_CC0pi1pCosTheta->Clone("T2K_CC0pi1p_XSec_2018_MuonCosTheta_MC");
    fMCHist_CC0pi1pCosTheta->Reset();
    SetAutoProcessTH1(fDataHist_CC0pi1pCosTheta, kCMD_Write);
    SetAutoProcessTH1(fMCHist_CC0pi1pCosTheta, kCMD_Reset, kCMD_Scale, kCMD_Write);

    for (int i=0; i<=3; i++){
      fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("OneProtonAbove500MeV/MuonCosThetaSlice_1D_%i", i))->Clone(Form("T2K_CC0pi1p_XSec_2018_Data_MuonCosTh1DSlice%i", i)));
      fMCHist_Slices.push_back((TH1D*)fInputFile->Get(Form("OneProtonAbove500MeV/MuonCosThetaSlice_1D_%i", i))->Clone(Form("T2K_CC0pi1p_XSec_2018_MC_MuonCosTh1DSlice%i", i)));
    }
    // Add in the muon costh-p costh slices (which aren't as nicely numbered)
    //         -> TH1D MuCThSlice_1_PCthSlice_0
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_1_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_1_PCthSlice_0"));
    fMCHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_1_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_1_PCthSlice_0"));
    //         -> TH1D MuCThSlice_2_PCthSlice_0
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_2_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_2_PCthSlice_0"));
    fMCHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_2_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_2_PCthSlice_0"));
    //         -> TH1D MuCThSlice_2_PCthSlice_1
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_2_PCthSlice_1")->Clone("T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_2_PCthSlice_1"));
    fMCHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_2_PCthSlice_1")->Clone("T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_2_PCthSlice_1"));
    //         -> TH1D MuCThSlice_3_PCthSlice_0
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_3_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_Data_MuCThSlice_3_PCthSlice_0"));
    fMCHist_Slices.push_back((TH1D*)fInputFile->Get("OneProtonAbove500MeV/MuCThSlice_3_PCthSlice_0")->Clone("T2K_CC0pi1p_XSec_2018_MC_MuCThSlice_3_PCthSlice_0"));
  }


  // Set all slice histograms to auto-process and reset MC histograms
  for (size_t i=0; i<fDataHist_Slices.size(); i++){
    fMCHist_Slices[i]->Reset();
    SetAutoProcessTH1(fDataHist_Slices[i], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[i], kCMD_Reset, kCMD_Scale, kCMD_Write);
  }

  return;
};

// Yay hardcoding
// Taken from multidif_binMap.txt in data release
int T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::Get1DBin(int nProtonsAboveThresh, double pmu, double CosThetaMu, double pp, double CosThetaP) {

  int binnumber = -999;

  // If you're looking at a sample you don't want to look at, return -999
  if (nProtonsAboveThresh == 0 && !useCC0pi0p){return binnumber;}
  if (nProtonsAboveThresh == 1 && !useCC0pi1p){return binnumber;}
  if (nProtonsAboveThresh >= 2 && !useCC0piNp){return binnumber;}

  // Calculate bin number (check that we want to use this sample before looking for the correct bin to save computation -- probably redundant because of the checks above but I like to be careful)
  if (nProtonsAboveThresh == 0 && useCC0pi0p){ //CC0pi0p: 2D binning in CosThetaMu--pmu
    if (CosThetaMu >= -1 && CosThetaMu <= -0.3) {binnumber = 1;}
    else if (CosThetaMu > -0.3 && CosThetaMu <= 0.3){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.3) {binnumber = 2;}
      else if (pmu > 0.3 && pmu <= 0.4){binnumber = 3;}
      else if (pmu > 0.4 && pmu <= 30){binnumber = 4;}
    } // end if (CosThetaMu > -0.3 && CosThetaMu <= 0.3)
    else if (CosThetaMu > 0.3 && CosThetaMu <= 0.6){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.3){binnumber = 5;}
      else if (pmu > 0.3 && pmu <= 0.4){binnumber = 6;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 7;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 8;}
      else if (pmu > 0.6 && pmu <= 30){binnumber = 9;}
    } // end if (CosThetaMu > 0.3 && CosThetaMu <= 0.6)
    else if (CosThetaMu > 0.6 && CosThetaMu <= 0.7){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.3){binnumber = 10;}
      else if (pmu > 0.3 && pmu <= 0.4){binnumber = 11;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 12;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 13;}
      else if (pmu > 0.6 && pmu <= 30){binnumber = 14;}
    } // end if (CosThetaMu > 0.6 && CosThetaMu <= 0.7)
    else if (CosThetaMu > 0.7 && CosThetaMu <= 0.8){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.3){binnumber = 15;}
      else if (pmu > 0.3 && pmu <= 0.4){binnumber = 16;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 17;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 18;}
      else if (pmu > 0.6 && pmu <= 0.7){binnumber = 19;}
      else if (pmu > 0.7 && pmu <= 0.8){binnumber = 20;}
      else if (pmu > 0.8 && pmu <= 30){binnumber = 21;}
    } // end if (CosThetaMu > 0.7 && CosThetaMu <= 0.8)
    else if (CosThetaMu > 0.8 && CosThetaMu <= 0.85){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.4){binnumber = 22;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 23;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 24;}
      else if (pmu > 0.6 && pmu <= 0.7){binnumber = 25;}
      else if (pmu > 0.7 && pmu <= 0.8){binnumber = 26;}
      else if (pmu > 0.8 && pmu <= 30){binnumber = 27;}
    } // end if (CosThetaMu > 0.8 && CosThetaMu <= 0.85)
    else if (CosThetaMu > 0.85 && CosThetaMu <= 0.9){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.3){binnumber = 28;}
      else if (pmu > 0.3 && pmu <= 0.4){binnumber = 29;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 30;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 31;}
      else if (pmu > 0.6 && pmu <= 0.7){binnumber = 32;}
      else if (pmu > 0.7 && pmu <= 0.8){binnumber = 33;}
      else if (pmu > 0.8 && pmu <= 1){binnumber = 34;}
      else if (pmu > 1 && pmu <= 30){binnumber = 35;}
    } // end if (CosThetaMu > 0.85 && CosThetaMu <= 0.9)
    else if (CosThetaMu > 0.9 && CosThetaMu <= 0.94){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.4){binnumber = 36;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 37;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 38;}
      else if (pmu > 0.6 && pmu <= 0.7){binnumber = 39;}
      else if (pmu > 0.7 && pmu <= 0.8){binnumber = 40;}
      else if (pmu > 0.8 && pmu <= 1.25){binnumber = 41;}
      else if (pmu > 1.25 && pmu <= 30){binnumber = 42;}
    } // end if (CosThetaMu > 0.9 && CosThetaMu <= 0.94)
    else if (CosThetaMu > 0.94 && CosThetaMu <= 0.98){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.4){binnumber = 43;}
      else if (pmu > 0.4 && pmu <= 0.5){binnumber = 44;}
      else if (pmu > 0.5 && pmu <= 0.6){binnumber = 45;}
      else if (pmu > 0.6 && pmu <= 0.7){binnumber = 46;}
      else if (pmu > 0.7 && pmu <= 0.8){binnumber = 47;}
      else if (pmu > 0.8 && pmu <= 1){binnumber = 48;}
      else if (pmu > 1 && pmu <= 1.25){binnumber = 49;}
      else if (pmu > 1.25 && pmu <= 1.5){binnumber = 50;}
      else if (pmu > 1.5 && pmu <= 2){binnumber = 51;}
      else if (pmu > 2 && pmu <= 30){binnumber = 52;}
    } // end if (CosThetaMu > 0.94 && CosThetaMu <= 0.98)
    else if (CosThetaMu > 0.98 && CosThetaMu <= 1){
      // Now find bin in pmu
      if (pmu >= 0 && pmu <= 0.5){binnumber = 53;}
      else if (pmu > 0.5 && pmu <= 0.65){binnumber = 54;}
      else if (pmu > 0.65 && pmu <= 0.8){binnumber = 55;}
      else if (pmu > 0.8 && pmu <= 1.25){binnumber = 56;}
      else if (pmu > 1.25 && pmu <= 2){binnumber = 57;}
      else if (pmu > 2 && pmu <= 3){binnumber = 58;}
      else if (pmu > 3 && pmu <= 5){binnumber = 59;}
      else if (pmu > 5 && pmu <= 30){binnumber = 60;}
    } // end if (CosThetaMu > 0.98 && CosThetaMu <= 1)
  } // end (nProtonsAboveThresh == 0)
  else if (nProtonsAboveThresh == 1 && useCC0pi1p){
    if (CosThetaMu >= -1 && CosThetaMu <= -0.3){
      // Find bin in CosThetaP
      if (CosThetaP >= -1 && CosThetaP <= 0.87){binnumber = 61;}
      else if (CosThetaP > 0.87 && CosThetaP <= 0.94){binnumber = 62;}
      else if (CosThetaP > 0.94 && CosThetaP <= 0.97){binnumber = 63;}
      else if (CosThetaP > 0.97 && CosThetaP <= 1){binnumber = 64;}
    } // end (CosThetaMu >= -1 && CosThetaMu <= -0.3)
    else if (CosThetaMu > -0.3 && CosThetaMu <= 0.3){
      // Find bin in CosThetaP
      if (CosThetaP >= -1 && CosThetaP <= 0.75){binnumber = 65;}
      else if (CosThetaP > 0.75 && CosThetaP <= 0.85){binnumber = 66;}
      else if (CosThetaP > 0.85 && CosThetaP <= 0.94){
         // Find bin in pp
         if (pp >= 0.5 && pp <= 0.68){binnumber = 67;}
         else if (pp > 0.68 && pp <= 0.78){binnumber = 68;}
         else if (pp > 0.78 && pp <= 0.9){binnumber = 69;}
         else if (pp > 0.9 && pp <= 30){binnumber = 70;}
      }  // end if (CosThetaP > 0.85 && CosThetaP <= 0.94)
      else if (CosThetaP > 0.94 && CosThetaP <= 1){binnumber = 71;}
    } // end if (CosThetaMu > -0.3 && CosThetaMu <= 0.3)
    else if (CosThetaMu > 0.3 && CosThetaMu <= 0.8){
      // Find bin in CosThetaP
      if (CosThetaP >= -1 && CosThetaP <= 0.3){binnumber = 72;}
      else if (CosThetaP > 0.3 && CosThetaP <= 0.5){binnumber = 73;}
      else if (CosThetaP > 0.5 && CosThetaP <= 0.8){
        // find bin in pp
         if (pp >= 0.5 && pp <= 0.6){binnumber = 74;}
         else if (pp > 0.6 && pp <= 0.7){binnumber = 75;}
         else if (pp > 0.7 && pp <= 0.8){binnumber = 76;}
         else if (pp > 0.8 && pp <= 0.9){binnumber = 77;}
         else if (pp > 0.9 && pp <= 30){binnumber = 78;}
      } // end if (CosThetaP > 0.5 && CosThetaP <= 0.8)
      else if (CosThetaP > 0.8 && CosThetaP <= 1){
        // find bin in pp
         if (pp >= 0.5 && pp <= 0.6){binnumber = 79;}
         else if (pp > 0.6 && pp <= 0.7){binnumber = 80;}
         else if (pp > 0.7 && pp <= 0.8){binnumber = 81;}
         else if (pp > 0.8 && pp <= 1){binnumber = 82;}
         else if (pp > 1 && pp <= 30){binnumber = 83;}
      } // end if (CosThetaP > 0.8 && CosThetaP <= 1)
    } // end if (CosThetaMu > 0.3 && CosThetaMu <= 0.8)
    else if (CosThetaMu > 0.8 && CosThetaMu <= 1){
      // Find bin in CosThetaP
      if (CosThetaP >= -1 && CosThetaP <= 0){binnumber = 84;}
      else if (CosThetaP > 0 && CosThetaP <= 0.3){binnumber = 85;}
      else if (CosThetaP > 0.3 && CosThetaP <= 0.8){
         // find bin in pp
         if (pp >= 0.5 && pp <= 0.6){binnumber = 86;}
         else if (pp > 0.6 && pp <= 0.7){binnumber = 87;}
         else if (pp > 0.7 && pp <= 0.8){binnumber = 88;}
         else if (pp > 0.8 && pp <= 0.9){binnumber = 89;}
         else if (pp > 0.9 && pp <= 1.1){binnumber = 90;}
         else if (pp > 1.1 && pp <= 30){binnumber = 91;}
       } // end if (CosThetaP > 0.3 && CosThetaP <= 0.8)
      else if (CosThetaP > 0.8 && CosThetaP <= 1){binnumber = 92;}
    } // end if (CosThetaMu > 0.8 && CosThetaMu <= 1)
  } // end if (nProtonsAboveThresh == 1)
  else if (nProtonsAboveThresh > 1 && useCC0piNp){
    binnumber = 93;
  }

  // If binnumber is still -999, something has gone wrong
  if (binnumber == -999){
    std::cout << "ERROR did not find correct 1D bin for an event with nProtonsAboveThresh = " << nProtonsAboveThresh << ", pmu = " << pmu << ", CosThetaMu = " << CosThetaMu << ", pp = " << pp << ", CosThetaP = " << CosThetaP << std::endl;
    return -999;
  }

  // Now need to work out adjustments for if we don't want to use one or more of the samples
  // If all samples are wanted, no adjustments required - the binning stands
  // If only CC0pi0p sample is wanted, no adjustments required - CC0pi0p binning is correct and the other bins won't be assigned
  // If CC0pi0p and CC0pi1p samples are wanted, no adjustments required

  // If only CC0pi1p sample is wanted or CC0pi1p and CC0piNp samples are wanted, need to subtract off total number of CC0pi0p bins
  if (useCC0pi1p && !useCC0pi0p){
    binnumber -= 60;
  }
  // If only CC0piNp sample is wanted, need to subtract off total number of CC0pi0p and CC0pi1p bins
  if (useCC0piNp && !useCC0pi0p && !useCC0pi1p){
    binnumber =- 92;
  }
  // If CC0pi0p and CC0piNp samples are wanted, need to subtract off CC0pi1p bins from CC0piNp bin number
  if (useCC0piNp && useCC0pi0p && !useCC0pi1p && binnumber == 93){
    binnumber = 61;
  }

  return binnumber;
};

// Yay hardcoding again!
// Taken from multidif_binMap.txt in data release
int T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::GetCosThetaMuSlice(int nProtonsAboveThresh, double CosThetaMu) {

  int slicenumber = -999;

  // if (useCC0pi0p), the first 10 slices will be CC0pi0p slices
  //         -> TH1D MuonCosThetaSlice_i where i = 0 to 9
  //         -> slices 0 to 9
  // if (useCC0pi1p), the next 8 slices will be CC0pi1p slices
  //         -> TH1D MuonCosThetaSlice_1D_i where i = 0 to 3
  //         -> TH1D MuCThSlice_1_PCthSlice_0
  //         -> TH1D MuCThSlice_2_PCthSlice_0
  //         -> TH1D MuCThSlice_2_PCthSlice_1
  //         -> TH1D MuCThSlice_3_PCthSlice_0
  //         -> slices 10 to 17 if (useCC0pi0p)
  //         -> slices 0 to 7 if (!useCC0pi0p)
  // In this code, we just find the costhetamu slices

  // Calculate bin number (check that we want to use this sample before looking for the correct bin to save computation)
  if (nProtonsAboveThresh == 0 && useCC0pi0p){ //CC0pi0p: 10 slices in CosThetaMu
    if (CosThetaMu >= -1 && CosThetaMu <= -0.3) {slicenumber = 0;}
    else if (CosThetaMu > -0.3 && CosThetaMu <= 0.3){slicenumber = 1;}
    else if (CosThetaMu > 0.3 && CosThetaMu <= 0.6){slicenumber = 2;}
    else if (CosThetaMu > 0.6 && CosThetaMu <= 0.7){slicenumber = 3;}
    else if (CosThetaMu > 0.7 && CosThetaMu <= 0.8){slicenumber = 4;}
    else if (CosThetaMu > 0.8 && CosThetaMu <= 0.85){slicenumber = 5;}
    else if (CosThetaMu > 0.85 && CosThetaMu <= 0.9){slicenumber = 6;}
    else if (CosThetaMu > 0.9 && CosThetaMu <= 0.94){slicenumber = 7;}
    else if (CosThetaMu > 0.94 && CosThetaMu <= 0.98){slicenumber = 8;}
    else if (CosThetaMu > 0.98 && CosThetaMu <= 1){slicenumber = 9;}
  } // end (nProtonsAboveThresh == 0)
  else if (nProtonsAboveThresh == 1 && useCC0pi1p){ // CC0pi1p: 8 slices, either in CosThetaMu or CosThetaMu-CosThetaP, depending on the slice
    if (CosThetaMu >= -1 && CosThetaMu <= -0.3){slicenumber = 10;}
    else if (CosThetaMu > -0.3 && CosThetaMu <= 0.3){slicenumber = 11;}
    else if (CosThetaMu > 0.3 && CosThetaMu <= 0.8){slicenumber = 12;}
    else if (CosThetaMu > 0.8 && CosThetaMu <= 1){slicenumber = 13;}
  } // end if (nProtonsAboveThresh == 1)

  // slicenumber may be -999 if nProtonsAboveThresh > 1
  // otherwise, something has gone wrong
  if (slicenumber == -999 && nProtonsAboveThresh <= 1){
    std::cout << "ERROR did not find correct 1D CosThetaMu slice for an event with nProtonsAboveThresh = " << nProtonsAboveThresh << ", CosThetaMu = " << CosThetaMu << std::endl;
    return -999;
  }

  // If useCC0pi0p is false, adjust slice numbers for CC0pi1p
  if (useCC0pi1p && !useCC0pi0p){
    slicenumber -= 10;
  }

  return slicenumber;
};

// Hardcoding one more time
// Taken from multidif_binMap.txt in data release
int T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::GetCC0pi1p2DSlice(int nProtonsAboveThresh, double CosThetaMu, double CosThetaP) {

  int slicenumber = -999;

  // Calculate slice number (note: only 2D CC0pi1p slices in CosThetaMu-CosThetaP are handled here)
  // These slices exist for:
  //   CosThetaMu -0.3 - 0.3, CosThetaP 0.85 - 0.94
  //   CosThetaMu 0.3 - 0.8, CosThetaP 0.5 - 0.8
  //   CosThetaMu 0.3 - 0.8, CosThetaP 0.8 - 1.0
  //   CosThetaMu 0.8 - 1.0, CosThetaP 0.3 - 0.8
  // If useCC0pi0p is true, these will be slices 14-17
  // If useCC0pi0p is false, these will be slices 4-7
  if (nProtonsAboveThresh == 1 && useCC0pi1p){
    if (CosThetaMu > -0.3 && CosThetaMu <= 0.3 && CosThetaP > 0.85 && CosThetaP <= 0.94){slicenumber = 14;}
    else if (CosThetaMu > 0.3 && CosThetaMu <= 0.8){
      // Find bin in CosThetaP
      if (CosThetaP > 0.5 && CosThetaP <= 0.8){slicenumber = 15;}
      else if (CosThetaP > 0.8 && CosThetaP <= 1){slicenumber = 16;}
    } // end if (CosThetaMu > 0.3 && CosThetaMu <= 0.8)
    else if (CosThetaMu > 0.8 && CosThetaMu <= 1 && CosThetaP > 0.3 && CosThetaP <= 0.8){slicenumber = 17;}
  } // end if (nProtonsAboveThresh == 1)

  // No check on binnumber = -999 here, because many events won't fall into one of these slices

  // If useCC0pi0p is false, adjust slice numbers for CC0pi1p
  if (useCC0pi1p && !useCC0pi0p){
    slicenumber -= 10;
  }

  return slicenumber;
};

// // Reimplementation of Measurement1D::Write (calling the original) to also set the slice histograms to have the chi2 of the total 1D histogram -- makes plotting easier
// void T2K_CC0piWithProtons_XSec_2018_multidif_0p_1p_Np::Write(std::string drawOpt){
//   // call Measurement1D::Write
//   Measurement1D::Write(drawOpt);
//
//   // Now also set slice histogram titles to be equal to the overall chi2
//   std::ostringstream chi2;
//   chi2 << std::setprecision(5) << GetLikelihood();
//   for (size_t i=0; i<fDataHist_Slices.size(); i++){
//       fMCHist_Slices[i]->SetTitle(chi2.str().c_str());
//     }
// };
