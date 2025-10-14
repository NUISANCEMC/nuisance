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

#include "MicroBooNE_CC1Mu1p_XSec_2D_nu.h"
#include "MicroBooNE_SignalDef.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TPRegexp.h"
#include "TObjString.h"

namespace {

  double kDUMMY = std::numeric_limits<double>::max();

  // Extract slice values using regex
  TPRegexp re("_(\\d+)_(\\d+)To(\\d+)_(\\d+)");

} // namespace

//********************************************************************
MicroBooNE_CC1Mu1p_XSec_2D_nu::MicroBooNE_CC1Mu1p_XSec_2D_nu(
    nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);

  std::string name = fSettings.GetS("name");

  // work out which sample you need, and set axii
  if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaPT_DeltaAlphaT_nu")) {
    fDist = kDeltaPT;
    fSlice = kDeltaAlphaT;
    fSuffix = "DeltaPT_DeltaAlphaT";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d^{2}#sigma/d#deltap_{T}d#delta#alpha_{T} (cm^{2}/(deg)/(GeV/c)/^{40}Ar)");
  } else {
    NUIS_ABORT(
        "MicroBooNE_CC1Mu1p_XSec_2D_nu: Didn't get a valid name: " << name);
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n"
                               "Target: Ar\n"
                               "Flux: BNB FHC numu\n"
                               "Signal: CC1Mu1p\n";
  fSettings.SetDescription(descrip);
  fSettings.SetTitle(name);
  fSettings.SetAllowedTypes("FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
                            "FIX/FULL");
  fSettings.SetEnuRange(0.0, 6.8);
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Load data ---------------------------------------------------------
  SetHistograms();

  // ScaleFactor for DiffXSec/cm2/Nucleus
  // Already multiplied by the Ar mass number
  fScaleFactor = GetEventHistogram()->Integral("width") / fNEvents * 1.0E-38 /
                 TotalIntegratedFlux() * 40;

  // Final setup ------------------------------------------------------
  FinaliseMeasurement();
}

bool MicroBooNE_CC1Mu1p_XSec_2D_nu::isSignal(FitEvent *event) {
  return SignalDef::MicroBooNE::isCC1Mu1p(event, EnuMin, EnuMax);
}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::FillEventVariables(FitEvent *event) {

  if (!isSignal(event)) { // double the work, but it lets us use the below
                          // functions without error checking
    fXVar = -999;
    return;
  }

  auto const &signal_proton =
      *SignalDef::MicroBooNE::GetCC1Mu1pProtonsInPS(event).front();
  TVector3 vpmu = event->GetHMFSParticle(13)->P3();

  // using definitions in
  // https://journals.aps.org/prd/pdf/10.1103/PhysRevD.108.053002

  TVector3 vp = signal_proton.P3();
  TVector3 vSum =
      vpmu +
      vp; // missing momentum in the plane transverse to the beam direction

  TVector3 vpmuT(vpmu.X(), vpmu.Y(), 0.); // transverse muon momentum
  TVector3 vpT(vp.X(), vp.Y(), 0.);       // transverse proton momentum
  TVector3 vSumT(vSum.X(), vSum.Y(), 0.); // transverse missing momentum (eq. 1)

  TVector3 vpmuL(0., 0., vpmu.Z());
  TVector3 vpL(0., 0., vp.Z());

  double DeltaPT = vSum.Pt() / 1000.; // GeV/c
  double DeltaAlphaT =
      TMath::ACos((-vpmuT * vSumT) / (vpmuT.Mag() * vSumT.Mag())) * 180. /
      TMath::Pi(); // deg
  double DeltaPhiT =
      TMath::ACos((-vpmuT * vpT) / (vpmuT.Mag() * vpT.Mag())) * 180. /
      TMath::Pi(); // deg   -////////////changed by Abi 5/12/23 using (eq 3)

  double MuonCosTheta = vpmu.CosTheta();
  double ProtonCosTheta = vp.CosTheta();

  double MuonMomentum = vpmu.Mag() / 1000.; // GeV/c
  double ProtonMomentum = vp.Mag() / 1000.; // GeV/c

  double BE = 0.04;                  // GeV, binding energy
  double NeutronMass_GeV = 0.939565; // GeV

  double ProtonMass_GeV = 0.938272; // GeV
  double MuonMass_GeV = 0.106;      // GeV
  double DeltaM2 = TMath::Power(NeutronMass_GeV, 2.) -
                   TMath::Power(ProtonMass_GeV, 2.);                 // GeV^2
  double MuonEnergy = (event->GetHMFSParticle(13)->fP.E()) / 1000.0; // GeV/c
                                                                     // Abi
  double ProtonEnergy = signal_proton.E() / 1000.0; // GeV/c Abi;
  double ProtonKE = ProtonEnergy - ProtonMass_GeV;

  // ECal energy reconstruction
  double ECal = ((MuonEnergy) + (ProtonKE) + BE); // GeV
  //  QE Energy Reconstruction

  double EQENum = 2 * (NeutronMass_GeV - BE) * MuonEnergy -
                  (BE * BE - 2 * NeutronMass_GeV * BE +
                   MuonMass_GeV * MuonMass_GeV + DeltaM2);
  double EQEDen =
      2 * (NeutronMass_GeV - BE - MuonEnergy + vpmu.Mag() * vpmu.CosTheta());
  double EQE = EQENum / EQEDen;

  // 3D KI variables

  // For the calculation of the masses
  // https://journals.aps.org/prc/pdf/10.1103/PhysRevC.95.065501

  double MA = (22 * NeutronMass_GeV) + (18 * ProtonMass_GeV) - 0.34381; // GeV

  // For the calculation of the excitation energies
  // https://doi.org/10.1140/epjc/s10052-019-6750-3

  double MAPrime =
      MA - NeutronMass_GeV + 0.0309; // GeV, constant obtained from table 7

  // For the calculation of p_n, back to the Minerva PRL
  // https://journals.aps.org/prl/pdf/10.1103/PhysRevLett.121.022504

  double R =
      MA + vpmuL.Mag() + vpL.Mag() - MuonEnergy - ProtonEnergy; // Equation 8

  // Equation 7

  double PL =
      0.5 * R - (MAPrime * MAPrime + vSumT.Mag() * vSumT.Mag()) / (2 * R);

  double DeltaPn =
      TMath::Sqrt((vSumT.Mag() * vSumT.Mag()) + (PL * PL)) / 1000.0;
  //  https://journals.aps.org/prd/pdf/10.1103/PhysRevD.101.092001

  TVector3 UnitZ(0, 0, 1);
  // double Ptx = ( UnitZ.Cross(vpmuT) ).Dot(vSumT) / vpmuT.Mag(); -original in
  // code
  double DeltaPtx = DeltaPT * TMath::Sin(DeltaAlphaT); // changed by Abi
                                                       // 06/12/23
  double Pty = -(vpmuT).Dot(vSumT) / vpmuT.Mag();

  double DeltaPty = DeltaPT * TMath::Sin(DeltaAlphaT); // changed by Abi
                                                       // 06/12/23
  //----------------------------------------//

  if (fDist == kDeltaPT) {
    fXVar = DeltaPT;
  }
  else if (fDist == kDeltaAlphaT) {
    fXVar = DeltaAlphaT;
  }
  else {
    NUIS_ABORT(
        "MicroBooNE_CC1Mu1p_XSec_2D_nu: Didn't get a valid distribution");
  }

  if (fSlice == kDeltaPT) {
    fYVar = DeltaPT;
  }
  else if (fSlice == kDeltaAlphaT) {
    fYVar = DeltaAlphaT;
  }
  else {
    NUIS_ABORT(
        "MicroBooNE_CC1Mu1p_XSec_2D_nu: Didn't get a valid slice");
  }

}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::FillHistograms() {
  Measurement1D::FillHistograms();
  if (Signal) {
    fMCHist_Fine2D->Fill(fXVar, fYVar, Weight);
    FillMCSlice(fXVar, fYVar, Weight);
  }
}

void MicroBooNE_CCInc_XSec_2DPcos_nu::FillMCSlice(double x, double y,
                                                  double w) {
  int slice_id = 0
  for (const auto& edge : fSliceEdges) {
    if (y >= edge.first && y < edge.second) {
      fMCHist_Slices[slice_id]->Fill(x, w);
      break;
    }
    slice_id++;
  }
}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::ConvertEventRates() {

  // Apply Weiner-SVD additional smearing Ac
  int n = fMCHist->GetNbinsX();

  TVectorD v(n), e(n);
  for (int i = 0; i < n; i++) {
    v(i) = fMCHist->GetBinContent(i + 1);
    e(i) = std::pow(fMCHist->GetBinError(i + 1), 2);
  }

  TVectorD vs = (*fSmearingMatrix) * v;
  TVectorD es = (*fSmearingMatrix) * e;

  for (int i = 0; i < n; i++) {
    fMCHist->SetBinContent(i + 1, vs(i));
    fMCHist->SetBinError(i + 1, std::sqrt(es(i)));
  }

  // have to do standard conversion AFTER wSVD smearing
  Measurement1D::ConvertEventRates();
}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::SetHistograms() {

  // Open input file
  std::string inputFile = FitPar::GetDataBase() +
                          "/MicroBooNE/CC1Mu1p/All_XSecs_Combined_v08_00_00_52.root";
  TFile *inputRootFile = TFile::Open(inputFile.c_str());
  assert(inputRootFile && inputRootFile->IsOpen());

  // Get the list of keys (top-level objects)
  TList* inputKeys = inputRootFile->GetListOfKeys();
  assert(inputKeys);

  // Get input collections for different slices
  std::map<std::pair<double, double>, TH1D*> data_map;
  std::map<std::pair<double, double>, TH2D*> cov_map;
  std::map<std::pair<double, double>, TH2D*> smear_map;

  size_t nbins = 0; // keep track of total number of bins

  for (auto* obj : *inputKeys) {
    auto* key = (TKey*)obj;
    TString name = key->GetName();

    if (name.Contains("FullUnc_" + fSuffix, TString::kIgnoreCase)) {
      TH1D* temp_data = (TH1D*)inputRootFile->Get(name);
      nbins += temp_data->GetNbinsX();
      data_map[GetSlice(name)] = temp_data;
    } 
    else if (name.Contains("UnfCov_" + fSuffix, TString::kIgnoreCase)) {
      cov_map[GetSlice(name)] = (TH2D*)inputRootFile->Get(name);
    }
    else if (name.Contains("Ac_" + fSuffix, TString::kIgnoreCase)) {
      smear_map[GetSlice(name)] = (TH2D*)inputRootFile->Get(name);
    }
  } // end loop over keys

  assert(data_map.size() == cov_map.size() && cov_map.size() == smear_map.size());

  std::string sample_name = fSettings.GetS("name");

  // Prepare merged objects (all slices)
  fDataHist = new TH1D(Form("%s_data", sample_name.c_str()),
			                 Form("%s_data%s", sample_name.c_str(), fSettings.GetFullTitles().c_str()),
                       nbins, 0, nbins);
  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  fSmearingMatrix = new TMatrixD(fDataHist->GetNbinsX(), fDataHist->GetNbinsX());

  // Set 2D fine-grained 2D hist
  fMCHist_Fine2D = new TH2D(Form("%s_MC_FINE_2D", sample_name.c_str()),
			                      Form("%s_MC_FINE_2D; p_{#mu}^{reco} (GeV); cos#theta_{#mu}^{reco};%s", 
				                         sample_name.c_str(), fSettings.GetYTitle().c_str()), 
			                      400, 0.0, 2.5, 100, -1.0, 1.0);
  SetAutoProcessTH1(fMCHist_Fine2D);

  // Keep track of slice index and bin offset
  int slice_id = 0;
  int offset = 0;

  // Loop over slices (map keeps keys ordered)
  for (const auto& [slice, temp_data] : data_map) {

    // Store slice edge values
    fSliceEdges.push_back(slice);

    // Get number of bins in slice
    int nbins_slice = temp_data->GetNbinsX();

    // Merge data histograms
    for (int i = 1; i <= nbins_slice; ++i) {
        double content = temp_data->GetBinContent(i);
        double error   = temp_data->GetBinError(i);
        fDataHist->SetBinContent(offset + i, content);
        fDataHist->SetBinError(offset + i, error);
    }

    // Add data histogram to slice
    TString name  = Form("%s_data_Slice%lu", sample_name.c_str(), slice_id);
    TString title = Form("%s_data_Slice%lu; p_{#mu}^{reco} (GeV);%s", 
                          sample_name.c_str(), slice_id, fSettings.GetYTitle().c_str());
    temp_data->Sumw2();
    temp_data->SetNameTitle(name, title);
    fDataHist_Slices.push_back(temp_data);

    // Add MC histogram to slice
    fMCHist_Slices.push_back((TH1D *)temp_data->Clone());
    name  = Form("%s_MC_Slice%lu", sample_name.c_str(), slice_id);
    title = Form("%s_MC_Slice%lu; p_{#mu}^{reco} (GeV);%s",
                  sample_name.c_str(), slice_id, fSettings.GetYTitle().c_str());
    fMCHist_Slices[slice_id]->SetNameTitle(name, title);
    fMCHist_Slices[slice_id]->Reset();

    // Fill 2D objects, guaranteed same key
    auto temp_cov = cov_map.at(slice);
    auto temp_smear = smear_map.at(slice);
    for (int i = 0; i <= nbins_slice; ++i) {
        for (int j = 0; j <= nbins_slice; ++j) {
            int i_local = i + offset;
            int j_local = j + offset;
            (*fFullCovar)(i_local, j_local) = temp_cov->GetBinContent(i + 1, j + 1);
            (*fSmearingMatrix)(i_local, j_local) = temp_smear->GetBinContent(i + 1, j + 1);
        }
    }

    SetAutoProcessTH1(fDataHist_Slices[slice_id], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[slice_id]);

    slice_id++;
    offset += nbins_slice;

  }

  // Few last details...
  fDataHist->Scale(1e-38);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

}

std::pair<double, double> MicroBooNE_CC1Mu1p_XSec_2D_nu::GetSlice(TString name) {

    double low = kDUMMY;
    double high = kDUMMY;

    TObjArray* matches = re.MatchS(name);
    if (matches && matches->GetEntries() >= 5) {
        int a1 = ((TObjString*)matches->At(1))->GetString().Atoi();
        int a2 = ((TObjString*)matches->At(2))->GetString().Atoi();
        int b1 = ((TObjString*)matches->At(3))->GetString().Atoi();
        int b2 = ((TObjString*)matches->At(4))->GetString().Atoi();
        
        low  = a1 + a2 / 100.0;
        high = b1 + b2 / 100.0;
    }

    return std::make_pair(low, high);

}