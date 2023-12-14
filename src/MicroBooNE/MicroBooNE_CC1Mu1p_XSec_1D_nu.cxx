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

#include "MicroBooNE_CC1Mu1p_XSec_1D_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"

//********************************************************************
MicroBooNE_CC1Mu1p_XSec_1D_nu::MicroBooNE_CC1Mu1p_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");
  std::string objSuffix;

  if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaPT_nu")) {
    fDist = kDeltaPT;
    objSuffix = "DeltaPT";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d#sigma/d#deltap_{T} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaAlphaT_nu")) {
    fDist = kDeltaAlphaT;
    objSuffix = "DeltaAlphaT";
    fSettings.SetXTitle("#delta#alpha_{T} (deg)");
    fSettings.SetYTitle("d#sigma/d#delta#alpha_{T} (cm^{2}/(deg)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaPhiT_nu")) {
    fDist = kDeltaPhiT;
    objSuffix = "DeltaPhiT";
    fSettings.SetXTitle("#delta#phi_{T} (deg)");
    fSettings.SetYTitle("d#sigma/d#delta#phi_{T} (cm^{2}/(deg)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DMuonCosTheta_nu")) {
    fDist = kMuonCosTheta;
    objSuffix = "MuonCosTheta";
    fSettings.SetXTitle("cos#theta_{#mu} (deg)");
    fSettings.SetYTitle("d#sigma/dcos#theta_{#mu} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DProtonCosTheta_nu")) {
    fDist = kProtonCosTheta;
    objSuffix = "ProtonCosTheta";
    fSettings.SetXTitle("cos#theta_{p} (deg)");
    fSettings.SetYTitle("d#sigma/dcos#theta_{p} (cm^{2}/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DMuonMomentum_nu")) {
    fDist = kMuonMomentum;
    objSuffix = "MuonMomentum";
    fSettings.SetXTitle("p_{#mu} (GeV/c)");
    fSettings.SetYTitle("d#sigma/dp_{#mu} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaPn_nu")) {
    fDist = kDeltaPn;
    objSuffix = "DeltaPn";
    fSettings.SetXTitle("p_{n,proxy} (GeV/c)");
    fSettings.SetYTitle("d#sigma/dp_{n,proxy} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaPtx_nu")) {
    fDist = kDeltaPtx;
    objSuffix = "DeltaPtx";
    fSettings.SetXTitle("#deltap_{T,x} (GeV/c)");
    fSettings.SetYTitle("d#sigma/d#deltap_{T,x} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DDeltaPty_nu")) {
    fDist = kDeltaPty;
    objSuffix = "DeltaPty";
    fSettings.SetXTitle("#deltap_{T,y} (GeV/c)");
    fSettings.SetYTitle("d#sigma/d#deltap_{T,y} (cm^{2}/(GeV/c)/^{40}Ar)");
  }
  else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_1DECal_nu")) {
    fDist = kECal;
    objSuffix = "ECal";
    fSettings.SetXTitle("E^{Cal} (GeV)");
    fSettings.SetYTitle("d#sigma/dE^{Cal} (cm^{2}/(GeV)/^{40}Ar)");
  }


  else {
    assert(false);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "Signal: CC1Mu1p\n";

  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.8);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------
  std::string inputFile = FitPar::GetDataBase() + "/MicroBooNE/CC1Mu1p/All_XSecs_Combined_v08_00_00_52.root";
  SetDataFromRootFile(inputFile, "FullUnc_" + objSuffix + "Plot");
  ScaleData(1E-38);

  // ScaleFactor for DiffXSec/cm2/Nucleus // Already multiplied by the Ar mass number
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1E-38 / TotalIntegratedFlux() * 40;

  SetCovarFromRootFile(inputFile, "UnfCov_" + objSuffix + "Plot");

  // Set up the additional smearing matrix Ac
  // All the MC predictions need to be multiplied by Ac to move to the regularized phase space

  TFile* inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());
  TH2D* hsmear = (TH2D*) inputRootFile->Get(("Ac_" + objSuffix + "Plot").c_str());
  assert(hsmear);

  int nrows = hsmear->GetNbinsX();
  int ncols = hsmear->GetNbinsY();
  fSmearingMatrix = new TMatrixD(nrows, ncols);
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<ncols; j++) {
      fSmearingMatrix->operator()(i,j) = hsmear->GetBinContent(i+1, j+1);
    }
  }

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
};


bool MicroBooNE_CC1Mu1p_XSec_1D_nu::isSignal(FitEvent* event) {

  return SignalDef::MicroBooNE::isCC1Mu1p(event, EnuMin, EnuMax);

};


void MicroBooNE_CC1Mu1p_XSec_1D_nu::FillEventVariables(FitEvent* event) {

  if (event->NumFSParticle(13) == 0) return;
  if (event->NumFSParticle(2212) < 1) return;

  if ( !(SignalDef::MicroBooNE::isCC1Mu1p(event, EnuMin, EnuMax) ) ) { return; }

  TVector3 vpmu = event->GetHMFSParticle(13)->fP.Vect();
  if (vpmu.Mag() < 100 || vpmu.Mag() > 1200) { return; }

  int ProtonCounter = 0;
  std::vector<int> ProtonIndices = event->GetAllFSProtonIndices();
  std::vector<int> SignalProtonIndices;

  for (int i = 0; i < (int)(ProtonIndices.size()); i++) {

    double mom = event->GetParticleMom( ProtonIndices.at(i) );
    if (mom > 300 && mom < 1000) { 
      
      ProtonCounter++; 
      SignalProtonIndices.push_back( ProtonIndices.at(i) );

    }

  }

  if (ProtonCounter != 1) { return; }

  TVector3 vp = event->GetParticleP3(SignalProtonIndices.at(0) );
  TVector3 vSum = vpmu + vp;

  TVector3 vpmuT(vpmu.X(),vpmu.Y(),0.);
  TVector3 vpT(vp.X(),vp.Y(),0.);
  TVector3 vSumT(vSum.X(),vSum.Y(),0.);

  TVector3 vpmuL(0.,0.,vpmu.Z());
  TVector3 vpL(0.,0.,vp.Z());

  double DeltaPT = vSum.Pt() / 1000.; // GeV/c
  double DeltaAlphaT = TMath::ACos( (- vpmuT * vSumT) / ( vpmuT.Mag() * vSumT.Mag() ) ) * 180./TMath::Pi(); // deg
  double DeltaPhiT =  TMath::ACos( (- vpmuT * vSumT) / ( vpmuT.Mag() * vSumT.Mag() ) ) * 180./TMath::Pi(); // deg

  double MuonCosTheta = vpmu.CosTheta();
  double ProtonCosTheta = vp.CosTheta();

  double MuonMomentum = vpmu.Mag()/1000.; // GeV/c
  double ProtonMomentum = vp.Mag()/1000.; // GeV/c


  double BE = 0.04; // GeV, binding energy
  double NeutronMass_GeV = 0.939565; // GeV
  double ProtonMass_GeV = 0.938272; // GeV 
  double MuonMass_GeV = 0.106; // GeV 
  double DeltaM2 =  TMath::Power(NeutronMass_GeV,2.) - TMath::Power(ProtonMass_GeV,2.); // GeV^2
  double MuonEnergy = event->GetHMFSParticle(13)->fP.E();
  double ProtonEnergy = event->GetParticleP4(SignalProtonIndices.at(0) ).E();
  double ProtonKE = ProtonEnergy - ProtonMass_GeV;

  // ECal energy reconstruction
  double ECal =  MuonEnergy + ProtonKE + BE; // GeV

  // QE Energy Reconstruction

  double EQENum = 2 * (NeutronMass_GeV - BE) * MuonEnergy - (BE*BE - 2 * NeutronMass_GeV *BE + MuonMass_GeV * MuonMass_GeV + DeltaM2);
  double EQEDen = 2 * ( NeutronMass_GeV - BE - MuonEnergy + vpmu.Mag() * vpmu.CosTheta() );
  double EQE = EQENum / EQEDen;

  // 3D KI variables

  // For the calculation of the masses
  //https://journals.aps.org/prc/pdf/10.1103/PhysRevC.95.065501

  double MA = 22 * NeutronMass_GeV + 18 * ProtonMass_GeV - 0.34381; // GeV

  // For the calculation of the excitation energies
  // https://doi.org/10.1140/epjc/s10052-019-6750-3

  double MAPrime = MA - NeutronMass_GeV + 0.0309; // GeV, constant obtained from table 7 

  // For the calculation of p_n, back to the Minerva PRL
  // https://journals.aps.org/prl/pdf/10.1103/PhysRevLett.121.022504

  double R = MA + vpmuL.Mag() + vpL.Mag() - MuonEnergy - ProtonEnergy; // Equation 8

  // Equation 7

  double PL = 0.5 * R - (MAPrime * MAPrime + vSumT.Mag() * vSumT.Mag()) / (2 * R);

  double DeltaPn = TMath::Sqrt( vSumT.Mag() * vSumT.Mag() + PL * PL );

  // https://journals.aps.org/prd/pdf/10.1103/PhysRevD.101.092001

  TVector3 UnitZ(0,0,1);
  double Ptx = ( UnitZ.Cross(vpmuT) ).Dot(vSumT) / vpmuT.Mag();
  double Pty = - (vpmuT).Dot(vSumT) / vpmuT.Mag();

  //----------------------------------------//

  if (fDist == kDeltaPT) {
    fXVar = DeltaPT;
  }

  else if (fDist == kDeltaAlphaT) {
    fXVar = DeltaAlphaT;
  }

  else if (fDist == kDeltaPhiT) {
    fXVar = DeltaPhiT;
  }

  else if (fDist == kMuonCosTheta) {
    fXVar = MuonCosTheta;
  }

  else if (fDist == kProtonCosTheta) {
    fXVar = ProtonCosTheta;
  }

  else if (fDist == kMuonMomentum) {
    fXVar = MuonMomentum;
  }

  else if (fDist == kProtonMomentum) {
    fXVar = ProtonMomentum;
  }

  else if (fDist == kDeltaPn) {
    fXVar = kDeltaPn;
  }

  else if (fDist == kDeltaPtx) {
    fXVar = kDeltaPtx;
  }

  else if (fDist == kDeltaPty) {
    fXVar = kDeltaPty;
  }

  else if (fDist == kECal) {
    fXVar = ECal;
  }

  else if (fDist == kEQE) {
    fXVar = EQE;
  }

}

void MicroBooNE_CC1Mu1p_XSec_1D_nu::ConvertEventRates() {
  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply Weiner-SVD additional smearing Ac
  int n = fMCHist->GetNbinsX();
  TVectorD v(n);
  for (int i=0; i<n; i++) {
    v(i) = fMCHist->GetBinContent(i+1);
  }

  TVectorD vs = (*fSmearingMatrix) * v;

  for (int i=0; i<n; i++) {
    fMCHist->SetBinContent(i+1, vs(i));
  }
}
