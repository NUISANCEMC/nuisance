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

namespace {

// Extract numbers from string (ignore units and text)
std::vector<double> extractNumbers(const std::string &text) {
  std::vector<double> nums;
  std::regex numRegex(R"([-+]?\d*\.?\d+)");
  for (std::sregex_iterator it(text.begin(), text.end(), numRegex), end; it != end; ++it) {
    nums.push_back(std::stod(it->str()));
  }
  return nums;
}

// Parse the slice name string
std::pair<double, double> parseSliceEdges(const std::string &slice,
                                          const double default_low=0.0, const double default_high=1.0) {
  double low = -std::numeric_limits<double>::infinity();
  double high = std::numeric_limits<double>::infinity();

  // Identify patterns of comparisons
  if (slice.find('<') != std::string::npos || slice.find('>') != std::string::npos) {
    std::vector<double> nums = extractNumbers(slice);

    if (nums.size() == 2 && slice.find('<') != std::string::npos) {
      low = std::min(nums[0], nums[1]);
      high = std::max(nums[0], nums[1]);
    } else if (nums.size() == 1) {
      if (slice.find('>') != std::string::npos)
        low = nums[0];
      else if (slice.find('<') != std::string::npos)
        high = nums[0];
    }
  }

  // Apply defaults if missing
  if (std::isinf(low)) low = default_low;
  if (std::isinf(high)) high = default_high;

  return {low, high};
}

void DivideBinWidth(TH1D* h) {

  int NBins = h->GetXaxis()->GetNbins();

  for (int i = 0; i < NBins; i++) {

    double CurrentEntry = h->GetBinContent(i+1);
    double NewEntry = CurrentEntry / h->GetBinWidth(i+1);

    double CurrentError = h->GetBinError(i+1);
    double NewError = CurrentError / h->GetBinWidth(i+1);

    h->SetBinContent(i+1,NewEntry); 
    h->SetBinError(i+1,NewError); 

  }

}

} // namespace

Slice::Slice(std::string name, int id) : slice_name(name), slice_id(id) { 
  slice_edges = parseSliceEdges(slice_name);
}

Slice::~Slice() {}

void Slice::AddBin(int global, double low, double high) {
  // TODO: error handling
  global_bins.push_back(global);
  bin_edges.emplace_back(low, high);
}

double* Slice::GetBinsForTH1() const {
  int n_bins = global_bins.size();
  double *bin_edges_arr = new double[n_bins + 1]; // there's always 1 more edge than bins
  for (int i = 0; i < n_bins; ++i) {
    bin_edges_arr[i] = bin_edges[i].first;
  }
  bin_edges_arr[n_bins] = bin_edges[n_bins-1].second;
  return bin_edges_arr;
}

BinScheme::BinScheme() {}

BinScheme::~BinScheme() {}

void BinScheme::AddBin(std::vector<std::string> bin_config) {
  // Cast values as parsed by LoadBinScheme
  std::string slice_name = bin_config[1];
  int global_bin = std::stoi(bin_config[0]);
  double low_edge = std::stod(bin_config[2]);
  double high_edge = std::stod(bin_config[3]);
  return AddBin(slice_name, global_bin, low_edge, high_edge);
}

void BinScheme::AddBin(std::string slice_name,
                       int global, double low, double high) {
  for (auto& slice : slice_vec) {
    // Add bin to slice if already exists in collection
    if (slice.GetSliceName() == slice_name) {
      slice.AddBin(global, low, high);
      return;
    }
  }
  // Create new slice if not found before
  Slice new_slice(slice_name, current_slice);
  current_slice++;
  new_slice.AddBin(global, low, high);
  slice_vec.push_back(new_slice);
}

int BinScheme::GetNumberBins() {
  int nbins = 0;
  for (auto& slice : slice_vec) {
    nbins += slice.GetNumberBins();
  }
  return nbins;
}

Slice BinScheme::GetSliceFromGlobal(int global) {
  for (auto& slice : slice_vec) {
    std::vector<int> global_in_slice = slice.GetGlobalBins();
    if (std::find(global_in_slice.begin(), global_in_slice.end(), global) != global_in_slice.end()) {
      return slice;
    }
  }
  NUIS_ABORT("MicroBooNE_CC1Mu1p_XSec_2D_nu: Invalid bin number in scheme!");
}

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
    fSuffix = "SerialDeltaPT_DeltaAlphaT";
    fSliceTitle = "#delta#alpha_{T} (deg)";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d^{2}#sigma/d#delta#alpha_{T}d#deltap_{T} (cm^{2}/(deg)/(GeV/c)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaPT_MuonCosTheta_nu")) {
    fDist = kDeltaPT;
    fSlice = kMuonCosTheta;
    fSuffix = "SerialDeltaPT_MuonCosTheta";
    fSliceTitle = "cos#theta_{#mu}";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d^{2}#sigma/dcos#theta_{#mu}d#deltap_{T} (cm^{2}/(GeV/c)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaPT_ProtonCosTheta_nu")) {
    fDist = kDeltaPT;
    fSlice = kProtonCosTheta;
    fSuffix = "SerialDeltaPT_ProtonCosTheta";
    fSliceTitle = "cos#theta_{p}";
    fSettings.SetXTitle("#deltap_{T} (GeV/c)");
    fSettings.SetYTitle("d^{2}#sigma/dcos#theta_{p}d#deltap_{T} (cm^{2}/(GeV/c)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaAlphaT_DeltaPT_nu")) {
    fDist = kDeltaAlphaT;
    fSlice = kDeltaPT;
    fSuffix = "SerialDeltaAlphaT_DeltaPT";
    fSliceTitle = "#deltap_{T} (GeV/c)";
    fSettings.SetXTitle("#delta#alpha_{T} (deg)");
    fSettings.SetYTitle("d^{2}#sigma/d#deltap_{T}d#delta#alpha_{T} (cm^{2}/(GeV/c)/(deg)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaAlphaT_MuonCosTheta_nu")) {
    fDist = kDeltaAlphaT;
    fSlice = kMuonCosTheta;
    fSuffix = "SerialDeltaAlphaT_MuonCosTheta";
    fSliceTitle = "cos#theta_{#mu}";
    fSettings.SetXTitle("#delta#alpha_{T} (deg)");
    fSettings.SetYTitle("d^{2}#sigma/dcos#theta_{#mu}d#delta#alpha_{T} (cm^{2}/(deg)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaAlphaT_ProtonCosTheta_nu")) {
    fDist = kDeltaAlphaT;
    fSlice = kProtonCosTheta;
    fSuffix = "SerialDeltaAlphaT_ProtonCosTheta";
    fSliceTitle = "cos#theta_{p}";
    fSettings.SetXTitle("#delta#alpha_{T} (deg)");
    fSettings.SetYTitle("d^{2}#sigma/dcos#theta_{p}d#delta#alpha_{T} (cm^{2}/(deg)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaPhiT_DeltaPT_nu")) {
    fDist = kDeltaPhiT;
    fSlice = kDeltaPT;
    fSuffix = "SerialDeltaPhiT_DeltaPT";
    fSliceTitle = "#deltap_{T} (GeV/c)";
    fSettings.SetXTitle("#delta#phi_{T} (deg)");
    fSettings.SetYTitle("d^{2}#sigma/dcos#theta_{p}d#delta#phi_{T} (cm^{2}/(deg)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DDeltaPtx_DeltaPty_nu")) {
    fDist = kDeltaPtx;
    fSlice = kDeltaPty;
    fSuffix = "SerialDeltaPtx_DeltaPty";
    fSliceTitle = "#deltap_{T,y} (GeV/c)";
    fSettings.SetXTitle("#deltap_{T,x} (GeV/c)");
    fSettings.SetYTitle("d^{2}#sigma/d#deltap_{T,y}d#deltap_{T,x} (cm^{2}/(GeV/c)^{2}/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DECal_DeltaPT_nu")) {
    fDist = kECal;
    fSlice = kDeltaPT;
    fSuffix = "SerialECal_DeltaPT";
    fSliceTitle = "#deltap_{T} (GeV/c)";
    fSettings.SetXTitle("E^{cal} (GeV)");
    fSettings.SetYTitle("d^{2}#sigma/d#deltap_{T}dE^{ecal} (cm^{2}/(GeV/c)/(GeV)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DECal_DeltaAlphaT_nu")) {
    fDist = kECal;
    fSlice = kDeltaAlphaT;
    fSuffix = "SerialECal_DeltaAlphaT";
    fSliceTitle = "#delta#alpha_{T} (deg)";
    fSettings.SetXTitle("E^{cal} (GeV)");
    fSettings.SetYTitle("d^{2}#sigma/d#delta#alpha_{T}dE^{ecal} (cm^{2}/(deg)/(GeV)/^{40}Ar)");
  } else if (!name.compare("MicroBooNE_CC1Mu1p_XSec_2DECal_DeltaPty_nu")) {
    fDist = kECal;
    fSlice = kDeltaPty;
    fSuffix = "SerialECal_DeltaPty";
    fSliceTitle = "#deltap_{T,y} (GeV/c)";
    fSettings.SetXTitle("E^{cal} (GeV)");
    fSettings.SetYTitle("d^{2}#sigma/d#deltap_{T,y}dE^{ecal} (cm^{2}/(GeV/c)/(GeV)/^{40}Ar)");
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
  double MuonEnergy = (event->GetHMFSParticle(13)->fP.E()) / 1000.0; // GeV
                                                                     // Abi
  double ProtonEnergy = signal_proton.E() / 1000.0; // GeV Abi;
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
  double DeltaPtx = ( UnitZ.Cross(vpmuT) ).Dot(vSumT) / vpmuT.Mag() / 1000.0; // GeV/c
  double DeltaPty = -(vpmuT).Dot(vSumT) / vpmuT.Mag() / 1000.0; // GeV/c

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
    fXVar = DeltaPn;
  }
  else if (fDist == kDeltaPtx) {
    fXVar = DeltaPtx;
  }
  else if (fDist == kDeltaPty) {
    fXVar = DeltaPty;
  }
  else if (fDist == kECal) {
    fXVar = ECal;
  }
  else if (fDist == kEQE) {
    fXVar = EQE;
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
  else if (fSlice == kDeltaPhiT) {
    fYVar = DeltaPhiT;
  }
  else if (fSlice == kMuonCosTheta) {
    fYVar = MuonCosTheta;
  }
  else if (fSlice == kProtonCosTheta) {
    fYVar = ProtonCosTheta;
  }
  else if (fSlice == kMuonMomentum) {
    fYVar = MuonMomentum;
  }
  else if (fSlice == kProtonMomentum) {
    fYVar = ProtonMomentum;
  }
  else if (fSlice == kDeltaPn) {
    fYVar = DeltaPn;
  }
  else if (fSlice == kDeltaPtx) {
    fYVar = DeltaPtx;
  }
  else if (fSlice == kDeltaPty) {
    fYVar = DeltaPty;
  }
  else if (fSlice == kECal) {
    fYVar = ECal;
  }
  else if (fSlice == kEQE) {
    fYVar = EQE;
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

void MicroBooNE_CC1Mu1p_XSec_2D_nu::FillMCSlice(double x, double y,
                                                  double w) {
  // Fill corresponding MC slice histogram
  int slice_id = 0;
  for (const auto& slice : fBinScheme.GetSlices()) {
    std::pair<double, double> edges = slice.GetSliceEdges();
    if (y >= edges.first && y < edges.second) {
      fMCHist_Slices[slice_id]->Fill(x, w);
      break;
    }
    slice_id++;
  }
}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::ConvertEventRates() {

  // Do standard conversion
  Measurement1D::ConvertEventRates();

  // Apply MC truth -> reco smearing
  std::vector<TH1D *> slices_true;
  for (size_t i = 0; i < fMCHist_Slices.size(); i++) {
    TH1D *h = (TH1D *)fMCHist_Slices[i]->Clone(TString(fMCHist_Slices[i]->GetName()) + "_true");
    slices_true.push_back(h);
  }

  for (int ireco = 1; ireco < fMCHist->GetNbinsX() + 1; ireco++) {
    double total = 0.0;
    for (int itrue = 1; itrue < fMCHist->GetNbinsX() + 1; itrue++) {
      Slice slice = fBinScheme.GetSliceFromGlobal(itrue);
      int slice_id = slice.GetSliceId();
      int itrue_local = itrue - slice.GetGlobalBins()[0] + 1;
      TH1D *h = slices_true[slice_id];
      total += h->GetBinContent(itrue_local) *
               h->GetBinWidth(itrue_local) *
               fSmearingMatrix->operator()(ireco - 1, itrue - 1);
    }
    Slice slice = fBinScheme.GetSliceFromGlobal(ireco);
    int slice_id = slice.GetSliceId();
    int ireco_local = ireco - slice.GetGlobalBins()[0] + 1;
    TH1D *h = fMCHist_Slices[slice_id];
    h->SetBinContent(ireco_local, total / h->GetBinWidth(ireco_local));
  }

  for (size_t i = 0; i < slices_true.size(); i++) {
    delete slices_true[i];
  }

  // Scale MC slices also by their width in Y
  std::vector<Slice> slices = fBinScheme.GetSlices();
  for (size_t i = 0; i < fMCHist_Slices.size(); i++) {
    std::pair<double, double> edges = slices[i].GetSliceEdges();
    float w = edges.second - edges.first;
    fMCHist_Slices[i]->Scale(1.0 / w);
  }

  // Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (size_t i = 0; i < fDataHist_Slices.size(); i++) {
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++) {
      fMCHist->SetBinContent(bincount + 1, fMCHist_Slices[i]->GetBinContent(j + 1));
      fMCHist->SetBinError(bincount + 1, fMCHist_Slices[i]->GetBinError(j + 1));
      bincount++;
    }
  }

}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::SetHistograms() {

  // Open input file
  std::string inputFileName = FitPar::GetDataBase() +
                          "/MicroBooNE/CC1Mu1p/CC1Mu1p_DataRelease.root";
  TFile *inputFile = TFile::Open(inputFileName.c_str());
  assert(inputFile && inputFile->IsOpen());

  // Read bin configuration from file
  LoadBinScheme();

  std::string sample_name = fSettings.GetS("name");

  // Get data histogram
  fDataHist = (TH1D*)inputFile->Get(("TotalUnc_" + fSuffix).c_str());
  fDataHist->SetNameTitle(Form("%s_data", sample_name.c_str()),
			                    Form("%s_data%s", sample_name.c_str(), fSettings.GetFullTitles().c_str()));
  fDataHist->Scale(1e-38);
  int nbins = fDataHist->GetNbinsX();

  // Get covariance and smearing matrices
  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  fSmearingMatrix = new TMatrixD(fDataHist->GetNbinsX(), fDataHist->GetNbinsX());
  // Convert from TH2D to TMatrixD
  TH2D* temp_cov = (TH2D*)inputFile->Get(("Cov_" + fSuffix).c_str());
  TH2D* temp_smr = (TH2D*)inputFile->Get(("Ac_" + fSuffix).c_str());
  for (int i = 0; i < nbins; ++i) {
    for (int j = 0; j < nbins; ++j) {
      (*fFullCovar)(i, j) = temp_cov->GetBinContent(i + 1, j + 1);
      (*fSmearingMatrix)(i, j) = temp_smr->GetBinContent(i + 1, j + 1);
    }
  }

  // Set fine-grained MC 2D hist
  double slicesMin = fBinScheme.GetSlices().front().GetSliceEdges().first;
  double slicesMax = fBinScheme.GetSlices().back().GetSliceEdges().second;
  double binsMin = DBL_MAX, binsMax = -DBL_MAX;
  for (auto& s : fBinScheme.GetSlices()) {
      binsMin = std::min(binsMin, s.GetBinsForTH1()[0]);
      binsMax = std::max(binsMax, s.GetBinsForTH1()[s.GetNumberBins()]);
  }
  fMCHist_Fine2D = new TH2D(Form("%s_MC_FINE_2D", sample_name.c_str()),
			                      Form("%s_MC_FINE_2D; %s; %s; %s", 
				                         sample_name.c_str(),
                                 fSettings.GetXTitle().c_str(), fSliceTitle.c_str(), fSettings.GetYTitle().c_str()), 
			                      100, binsMin, binsMax, 100, slicesMin, slicesMax);
  SetAutoProcessTH1(fMCHist_Fine2D);

  // Divide data in slices
  int slice_id = 0;
  for (const auto& slice : fBinScheme.GetSlices()) {

    // Add data histogram to slice
    TString name  = Form("%s_data_Slice%lu", sample_name.c_str(), slice_id);
    TString title = Form("%s_data_Slice%lu; %s; %s", 
                          sample_name.c_str(), slice_id,
                          fSettings.GetXTitle().c_str(), fSettings.GetYTitle().c_str());
    TH1D* temp_data = new TH1D(name, title, slice.GetNumberBins(), slice.GetBinsForTH1());

    std::vector<int> temp_global_bins = slice.GetGlobalBins();
    double slice_width = slice.GetSliceWidth();
    for (int i = 1; i <= temp_global_bins.size(); ++i) {
      // Get bin properties
      int global_bin = temp_global_bins[i-1];
      double content = fDataHist->GetBinContent(global_bin);
      double error = fDataHist->GetBinError(global_bin);
      double width = temp_data->GetBinWidth(i); // width in slice hist only!
      // Compute new value
      double scale_factor = 1.0 / width / slice_width;
      double new_content = content * scale_factor;
      double new_error = error * scale_factor;
      // Fill slice histogram
      temp_data->SetBinContent(i, new_content);
      temp_data->SetBinError(i, new_error);
      // Update global data histogram
      fDataHist->SetBinContent(global_bin, new_content);
      fDataHist->SetBinError(global_bin, new_error);
    }

    temp_data->Sumw2();
    fDataHist_Slices.push_back(temp_data);

    // Add MC histogram to slice
    fMCHist_Slices.push_back((TH1D *)temp_data->Clone());
    name  = Form("%s_MC_Slice%lu", sample_name.c_str(), slice_id);
    title = Form("%s_MC_Slice%lu; %s; %s",
                  sample_name.c_str(), slice_id,
                  fSettings.GetXTitle().c_str(), fSettings.GetYTitle().c_str());
    fMCHist_Slices[slice_id]->SetNameTitle(name, title);
    fMCHist_Slices[slice_id]->Reset();

    SetAutoProcessTH1(fDataHist_Slices[slice_id], kCMD_Write);
    SetAutoProcessTH1(fMCHist_Slices[slice_id]);

    slice_id++;

  }

  // Update covariance matrix
  for (int i = 0; i < fBinScheme.GetNumberBins(); ++i) {
    Slice iSlice = fBinScheme.GetSliceFromGlobal(i+1);
    int iLocal = i + 1 - iSlice.GetGlobalBins()[0] + 1;
    double iBinWidth = fDataHist_Slices[iSlice.GetSliceId()]->GetBinWidth(iLocal);
    double iSliceWidth = iSlice.GetSliceWidth();
    for (int j = 0; j < fBinScheme.GetNumberBins(); ++j) {
      Slice jSlice = fBinScheme.GetSliceFromGlobal(j+1);
      int jLocal = j + 1 - jSlice.GetGlobalBins()[0] + 1;
      double jBinWidth = fDataHist_Slices[jSlice.GetSliceId()]->GetBinWidth(jLocal);
      double jSliceWidth = jSlice.GetSliceWidth();
      double cov_scale_factor = 1.0 / iBinWidth / jBinWidth / iSliceWidth / jSliceWidth;
      (*fFullCovar)(i, j) = (*fFullCovar)(i, j) * cov_scale_factor;
    }
  }

  fDecomp = StatUtils::GetDecomp(fFullCovar);

}

void MicroBooNE_CC1Mu1p_XSec_2D_nu::LoadBinScheme() {

  // Open input file
  std::string binFileName = FitPar::GetDataBase() +
                            "/MicroBooNE/CC1Mu1p/CC1Mu1p_BinScheme.txt";
  std::ifstream binFile(binFileName);

  std::string line;
  bool inTargetBlock = false;

  while (std::getline(binFile, line)) {

    // Trim leading/trailing spaces
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t\r\n") + 1);

    // Detect start of block
    if (line.find(fSuffix+"Plot") != std::string::npos) {
      inTargetBlock = true;
      continue;
    }

    // Stop when next block begins
    if (inTargetBlock && line.rfind("Serial", 0) == 0 && line != fSuffix+"Plot") {
      break;
    }

    // Skip header or empty lines
    if (!inTargetBlock || line.empty() || line[0] == 'B') continue;

    // Split line by '&'
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> parts;
    while (std::getline(ss, token, '&')) {
      // Trim whitespace around each part
      token.erase(0, token.find_first_not_of(" \t"));
      token.erase(token.find_last_not_of(" \t\r\n") + 1);
      parts.push_back(token);
    }

    // Add bin to bin scheme
    fBinScheme.AddBin(parts);

  }

  binFile.close();

}