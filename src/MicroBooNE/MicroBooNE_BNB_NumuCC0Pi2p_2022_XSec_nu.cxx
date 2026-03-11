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

#include "MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"
#include <cmath>

//********************************************************************
MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu::MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");

  std::string DataFileName = "/MicroBooNE/CC1Mu2p/MicroBooNE_CC1Mu2p.root";

  std::string ObjPrefix;

  if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DDeltaPT_nu")) {
    fDist = kDeltaPT;
    ObjPrefix = "reco_delta_p_t";
    fSettings.SetXTitle("#delta P_{T} [GeV]");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/GeV/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DCosPlPr_nu")) {
    fDist = kCosPlPr;
    ObjPrefix = "reco_cos(P_L,P_R)";
    fSettings.SetXTitle("cos(#gamma_{#vec{P_{L}},#vec{P_{R}}})");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DCosMuPsum_nu")) {
    fDist = kCosMuPsum;
    ObjPrefix = "reco_cos(Mu,P_sum)";
    fSettings.SetXTitle("cos(#gamma_{#vec{P_{#mu}},#vec{P_{sum}}})");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DDeltaAlphaT_nu")) {
    fDist = kDeltaAlphaT;
    ObjPrefix = "reco_delta_alpha_t";
    fSettings.SetXTitle("#delta #alpha_{T}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DDeltaPhiT_nu")) {
    fDist = kDeltaPhiT;
    ObjPrefix = "reco_delta_phi_t";
    fSettings.SetXTitle("#delta #phi_{T}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DMuonMomentum_nu")) {  
    fDist = kMuonMomentum;
    ObjPrefix = "reco_muon_momentum";
    fSettings.SetXTitle("P_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/GeV/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DMuonCosTheta_nu")) {  
    fDist = kMuonCosTheta;
    ObjPrefix = "reco_muon_costheta";
    fSettings.SetXTitle("cos(#theta_{#mu})");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DMuonPhi_nu")) {  
    fDist = kMuonPhi;
    ObjPrefix = "reco_muon_phi";
    fSettings.SetXTitle("#phi_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DLeadingProtonMomentum_nu")) {  
    fDist = kLeadingProtonMomentum;
    ObjPrefix = "reco_leading_proton_momentum";
    fSettings.SetXTitle("P_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/GeV/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DLeadingProtonCosTheta_nu")) {  
    fDist = kLeadingProtonCosTheta;
    ObjPrefix = "reco_leading_proton_costheta";
    fSettings.SetXTitle("cos(#theta_{#mu})");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DLeadingProtonPhi_nu")) {  
    fDist = kLeadingProtonPhi;
    ObjPrefix = "reco_leading_proton_phi";
    fSettings.SetXTitle("#phi_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DRecoilProtonMomentum_nu")) {  
    fDist = kRecoilProtonMomentum;
    ObjPrefix = "reco_recoil_proton_momentum";
    fSettings.SetXTitle("P_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/GeV/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DRecoilProtonCosTheta_nu")) {  
    fDist = kRecoilProtonCosTheta;
    ObjPrefix = "reco_recoil_proton_costheta";
    fSettings.SetXTitle("cos(#theta_{#mu})");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else if (!name.compare("MicroBooNE_CC1mu2p0pi_XSec_1DRecoilProtonPhi_nu")) {  
    fDist = kRecoilProtonPhi;
    ObjPrefix = "reco_recoil_proton_phi";
    fSettings.SetXTitle("#phi_{#mu}");
    fSettings.SetYTitle("Differential Cross Section [cm^{2}/Ar]");
  } else {
      assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB numu\n" \
                        "Signal: CC1mu2p0pi\n"
                        "Reference: Phys. Lett. B 872, 140052 (2026) \n"
                        "DOI: https://doi.org/10.1016/j.physletb.2025.140052 \n";

  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  std::string DataHistName = ObjPrefix+"/"+ObjPrefix+"_DataHist";
  std::string CovMatName = ObjPrefix+"/"+ObjPrefix+"_CovMat";
  std::string ACMatName = ObjPrefix+"/"+ObjPrefix+"_AC";

  // Load data --------------------------------------------------------- 
  std::string inputFile = FitPar::GetDataBase()+DataFileName;
  SetDataFromRootFile(inputFile, DataHistName);
  ScaleData(1E-38);

  fScaleFactor = 40.0*GetEventHistogram()->Integral("width") / (double(fNEvents) * TotalIntegratedFlux()); // Standard differential cross section per nucleon 
  fScaleFactor *= 1E-38; // Convert units

  SetCovarFromRootFile(inputFile, CovMatName);

  // Load smearing matrix ---------------------------------------------------------
  // Set up the additional smearing matrix Ac
  // All the MC predictions need to be multiplied by Ac to move to the regularized phase space

  TFile* inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());
  TH2D* hSmearMat_TH2 = (TH2D*)inputRootFile->Get(ACMatName.c_str());
  assert(hSmearMat_TH2);

  int nrows = hSmearMat_TH2->GetNbinsX();
  int ncols = hSmearMat_TH2->GetNbinsY();
  fSmearingMatrix = new TMatrixD(nrows, ncols);
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<ncols; j++) {
      (*fSmearingMatrix)(i,j) = hSmearMat_TH2->GetBinContent(i+1, j+1);
    }
  }
  inputRootFile->Close();

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
};


bool MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu::isSignal(FitEvent* event) {
  return SignalDef::MicroBooNE::isCC1mu2p0pi(event, EnuMin, EnuMax);
};


void MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu::FillEventVariables(FitEvent* event) {
  fXVar = -999.;
  if (!isSignal(event)) {return;}

  // Deal with Muon ------------------------------------------------------

  std::vector<FitParticle*> MuonParticles = event->GetAllFSMuon();
  if (MuonParticles.size() != 1) {
    std::cerr << "Number of muons returned not equal to 1 despite explicitly requesting 1 muon" << std::endl;
    throw;
  }
  FitParticle* Muon = MuonParticles[0];

  // Deal with Protons ------------------------------------------------------
  
  std::vector<FitParticle*> ProtonParticles = event->GetAllFSProton();

  std::vector<int> ProtonIndices;
  std::vector<double> ProtonMomentum;
  for (int iPart=0;iPart<ProtonParticles.size();iPart++) {
    if (ProtonParticles[iPart]->P3().Mag() > 300. && ProtonParticles[iPart]->P3().Mag() < 1000.) {
      ProtonIndices.push_back(iPart);
      ProtonMomentum.push_back(ProtonParticles[iPart]->P3().Mag());
    }
  }
  if (ProtonMomentum.size() != 2) {
    std::cerr << "Didn't find two protons with momentum in threshold!" << std::endl;
    std::cerr << "ProtonMomentum.size():" << ProtonMomentum.size() << std::endl;
    std::cerr << "event->NumFSProton():" << event->NumFSProton() << std::endl;
    throw;
  }

  int LeadingProtonIndex = -1;
  int RecoilProtonIndex = -1;
  if (ProtonMomentum[0] > ProtonMomentum[1]) {
    LeadingProtonIndex = ProtonIndices[0];
    RecoilProtonIndex = ProtonIndices[1];
  } else {
    LeadingProtonIndex = ProtonIndices[1];
    RecoilProtonIndex = ProtonIndices[0];
  }

  TVector3 ProtonMomentumSum = ProtonParticles[LeadingProtonIndex]->P3()+ProtonParticles[RecoilProtonIndex]->P3();

  // Calculate some TKI variables ------------------------------------------------------

  TVector3 MuonVectorTrans;
  MuonVectorTrans.SetXYZ(Muon->P3().X(),Muon->P3().Y(),0.);
  double MuonVectorTransMag = MuonVectorTrans.Mag();
  
  TVector3 ProtonVectorTrans;
  ProtonVectorTrans.SetXYZ(ProtonMomentumSum.X(),ProtonMomentumSum.Y(),0.);
  double ProtonVectorTransMag = ProtonVectorTrans.Mag();
  
  TVector3 PtVector = MuonVectorTrans + ProtonVectorTrans;
  double fPt = PtVector.Mag();
  
  // Calculate the variables ------------------------------------------------------

  if (fDist == kDeltaPT) {
    fXVar = fPt/1000.;
  } else if (fDist == kDeltaAlphaT) {
    fXVar = TMath::ACos( (- MuonVectorTrans * PtVector) / ( MuonVectorTransMag * fPt ) ) * 180./TMath::Pi();
    if (fXVar > 180.) { fXVar -= 180.; }
    if (fXVar < 0.) { fXVar += 180.; }
  } else if (fDist == kDeltaPhiT) {
    fXVar = TMath::ACos( (- MuonVectorTrans * ProtonVectorTrans) / ( MuonVectorTransMag * ProtonVectorTransMag ) ) * 180./TMath::Pi();
    if (fXVar > 180.) { fXVar -= 180.; }
    if (fXVar < 0.) { fXVar += 180.; }
  } else if (fDist == kCosPlPr) {
    fXVar = std::cos(ProtonParticles[LeadingProtonIndex]->P3().Angle(ProtonParticles[RecoilProtonIndex]->P3()));
  } else if (fDist == kCosMuPsum) {
    fXVar = std::cos(Muon->P3().Angle(ProtonMomentumSum));
  } else if (fDist == kMuonMomentum) {
    fXVar = Muon->P3().Mag()/1000.;
  } else if (fDist == kMuonCosTheta) {
    fXVar = std::cos(Muon->P3().Theta());
  } else if (fDist == kMuonPhi) {
    fXVar = Muon->P3().Phi();
  } else if (fDist == kLeadingProtonMomentum) {
    fXVar = ProtonParticles[LeadingProtonIndex]->P3().Mag()/1000.;
  } else if (fDist == kLeadingProtonCosTheta) {
    fXVar = std::cos(ProtonParticles[LeadingProtonIndex]->P3().Theta());
  } else if (fDist == kLeadingProtonPhi) {
    fXVar = ProtonParticles[LeadingProtonIndex]->P3().Phi();
  } else if (fDist == kRecoilProtonMomentum) {
    fXVar = ProtonParticles[RecoilProtonIndex]->P3().Mag()/1000.;
  } else if (fDist == kRecoilProtonCosTheta) {
    fXVar = std::cos(ProtonParticles[RecoilProtonIndex]->P3().Theta());
  } else if (fDist == kRecoilProtonPhi) {
    fXVar = ProtonParticles[RecoilProtonIndex]->P3().Phi();
  }
  
}

void MicroBooNE_BNB_NumuCC0Pi2p_2022_XSec_nu::ConvertEventRates() {
  // Do standard conversion
  Measurement1D::ConvertEventRates();

  int nBins = fMCHist->GetNbinsX();

  // First convert to TVectorD
  TVectorD MC_PreSmear(nBins);
  for (int iBin=0;iBin<nBins;iBin++) {
    MC_PreSmear(iBin) = fMCHist->GetBinContent(iBin+1)*fMCHist->GetBinWidth(iBin+1);
  }

  // Apply smearing
  TVectorD MC_PostSmear = (*fSmearingMatrix) * MC_PreSmear;

  // Then copy results back to histogram
  for (int iBin=0;iBin<nBins;iBin++) {
    fMCHist->SetBinContent(iBin+1, MC_PostSmear(iBin)/fMCHist->GetBinWidth(iBin+1));
  }
}

