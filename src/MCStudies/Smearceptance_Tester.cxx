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

#include "Smearceptance_Tester.h"

#include "SmearceptanceUtils.h"

#include "Smearcepterton.h"

//#define DEBUG_SMEARTESTER

//********************************************************************
/// @brief Class to perform smearceptance MC Studies on a custom measurement
Smearceptance_Tester::Smearceptance_Tester(nuiskey samplekey) {
  //********************************************************************

  samplekey.Print();

  // Sample overview ---------------------------------------------------
  std::string descrip =
      "Simple measurement class for producing an event summary tree of smeared "
      "events.\n";

  if (Config::HasPar("NPOT")) {
    samplekey.SetS("NPOT", Config::GetParS("NPOT"));
  }
  if (Config::HasPar("FluxIntegralOverride")) {
    samplekey.SetS("FluxIntegralOverride",
                   Config::GetParS("FluxIntegralOverride"));
  }
  if (Config::HasPar("TargetVolume")) {
    samplekey.SetS("TargetVolume", Config::GetParS("TargetVolume"));
  }
  if (Config::HasPar("TargetMaterialDensity")) {
    samplekey.SetS("TargetMaterialDensity",
                   Config::GetParS("TargetMaterialDensity"));
  }

  OutputSummaryTree = true;
  if (Config::HasPar("smear.OutputSummaryTree")) {
    OutputSummaryTree = Config::GetParI("smear.OutputSummaryTree");
  }

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  fSettings.SetTitle("Smearceptance Studies");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("XXX");
  fSettings.SetYTitle("Number of events");
  fSettings.SetEnuRange(0.0, 1E5);
  fSettings.SetAllowedTypes("EVT/SHAPE/DIAG", "EVT/SHAPE/DIAG");
  fSettings.DefineAllowedTargets("*");
  fSettings.DefineAllowedSpecies("*");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      TotalIntegratedFlux();

  // Measurement Details
  std::vector<std::string> splitName = GeneralUtils::ParseToStr(fName, "_");
  //size_t firstUS = fName.find_first_of("_");

  std::string smearceptorName = samplekey.GetS("smearceptor");
  QLOG(SAM, "Using smearceptor: " << smearceptorName
                                  << " (parsed from: " << fName << ").");

  fDataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);
  SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  eventVariables = NULL;

  QLOG(SAM, "Smearceptance Flux Scaling Factor = " << fScaleFactor);

  if (fScaleFactor <= 0.0) {
    ERROR(WRN, "SCALE FACTOR TOO LOW ");
    sleep(20);
  }

  // Setup our TTrees
  AddEventVariablesToTree();

  smearceptor = &Smearcepterton::Get().GetSmearcepter(smearceptorName);

  Int_t RecNBins = 20, TrueNBins = 20;
  double RecBinL = 0xdeadbeef, TrueBinL = 0, RecBinH = 10, TrueBinH = 10;

  if (Config::HasPar("smear.reconstructed.binning")) {
    std::vector<std::string> args = GeneralUtils::ParseToStr(
        Config::GetParS("smear.reconstructed.binning"), ",");
    RecNBins = GeneralUtils::StrToInt(args[0]);
    RecBinL = GeneralUtils::StrToDbl(args[1]);
    RecBinH = GeneralUtils::StrToDbl(args[2]);
    TrueNBins = RecNBins;
    TrueBinL = RecBinL;
    TrueBinH = RecBinH;
  }

  if (Config::HasPar("smear.true.binning")) {
    std::vector<std::string> args =
        GeneralUtils::ParseToStr(Config::GetParS("smear.true.binning"), ",");
    TrueNBins = GeneralUtils::StrToInt(args[0]);
    TrueBinL = GeneralUtils::StrToDbl(args[1]);
    TrueBinH = GeneralUtils::StrToDbl(args[2]);
  }
  SVDTruncation = 0;
  if (Config::HasPar("smear.true.binning")) {
    SVDTruncation = Config::GetParI("smear.SVD.truncation");
    QLOG(SAM, "Applying SVD truncation of: " << SVDTruncation)
  }

  ETrueDistrib = NULL;
  ETrueDistrib_noweight = NULL;
  ERecDistrib = NULL;
  RecoSmear = NULL;
  if (RecBinL != 0xdeadbeef) {
    QLOG(SAM, "Using binning True: " << TrueNBins << ", [" << TrueBinL << " -- "
                                     << TrueBinH << "], Rec: " << RecNBins
                                     << ", [" << RecBinL << " -- " << RecBinH
                                     << "]");

    ETrueDistrib = new TH1D("ELep_rate", ";True E_{#nu};Count", TrueNBins,
                            TrueBinL, TrueBinH);
    ETrueDistrib_noweight =
        new TH1D("ELep_rate_noweight", ";True E_{#nu};Count", TrueNBins,
                 TrueBinL, TrueBinH);
    ERecDistrib = new TH1D("ELepRec_rate", ";Rec E_{#nu};Count", RecNBins,
                           RecBinL, RecBinH);
    ETrueDistrib->Sumw2();
    ERecDistrib->Sumw2();

    RecoSmear =
        new TH2D("ELepHadVis_Recon", ";True E_{#nu};Recon. E_{#nu}", RecNBins,
                 RecBinL, RecBinH, TrueNBins, TrueBinL, TrueBinH);
    RecoSmear->Sumw2();
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
}

void Smearceptance_Tester::AddEventVariablesToTree() {
  if (OutputSummaryTree) {
    // Setup the TTree to save everything
    if (!eventVariables) {
      Config::Get().out->cd();
      eventVariables =
          new TTree((fName + "_VARS").c_str(), (fName + "_VARS").c_str());
    }

    LOG(SAM) << "Adding Event Variables" << std::endl;

    eventVariables->Branch("Omega_true", &Omega_true, "Omega_true/F");
    eventVariables->Branch("Q2_true", &Q2_true, "Q2_true/F");
    eventVariables->Branch("Mode_true", &Mode_true, "Mode_true/I");

    eventVariables->Branch("EISLep_true", &EISLep_true, "EISLep_true/F");

    eventVariables->Branch("HMFS_clep_true", &HMFS_clep_true);
    eventVariables->Branch("HMFS_pip_true", &HMFS_pip_true);
    eventVariables->Branch("HMFS_pim_true", &HMFS_pim_true);
    eventVariables->Branch("HMFS_cpi_true", &HMFS_cpi_true);
    eventVariables->Branch("HMFS_pi0_true", &HMFS_pi0_true);
    eventVariables->Branch("HMFS_cK_true", &HMFS_cK_true);
    eventVariables->Branch("HMFS_K0_true", &HMFS_K0_true);
    eventVariables->Branch("HMFS_p_true", &HMFS_p_true);

    eventVariables->Branch("KEFSHad_cpip_true", &KEFSHad_cpip_true,
                           "KEFSHad_cpip_true/F");
    eventVariables->Branch("KEFSHad_cpim_true", &KEFSHad_cpim_true,
                           "KEFSHad_cpim_true/F");
    eventVariables->Branch("KEFSHad_cpi_true", &KEFSHad_cpi_true,
                           "KEFSHad_cpi_true/F");
    eventVariables->Branch("TEFSHad_pi0_true", &TEFSHad_pi0_true,
                           "TEFSHad_pi0_true/F");
    eventVariables->Branch("KEFSHad_cK_true", &KEFSHad_cK_true,
                           "KEFSHad_cK_true/F");
    eventVariables->Branch("KEFSHad_K0_true", &KEFSHad_K0_true,
                           "KEFSHad_K0_true/F");
    eventVariables->Branch("KEFSHad_p_true", &KEFSHad_p_true,
                           "KEFSHad_p_true/F");
    eventVariables->Branch("KEFSHad_n_true", &KEFSHad_n_true,
                           "KEFSHad_n_true/F");

    eventVariables->Branch("EFSHad_true", &EFSHad_true, "EFSHad_true/F");
    eventVariables->Branch("EFSChargedEMHad_true", &EFSChargedEMHad_true,
                           "EFSChargedEMHad_true/F");

    eventVariables->Branch("EFSLep_true", &EFSLep_true, "EFSLep_true/F");
    eventVariables->Branch("EFSgamma_true", &EFSgamma_true, "EFSgamma_true/F");

    eventVariables->Branch("PDGISLep_true", &PDGISLep_true, "PDGISLep_true/I");
    eventVariables->Branch("PDGFSLep_true", &PDGFSLep_true, "PDGFSLep_true/I");

    eventVariables->Branch("Nprotons_true", &Nprotons_true, "Nprotons_true/I");
    eventVariables->Branch("Nneutrons_true", &Nneutrons_true,
                           "Nneutrons_true/I");
    eventVariables->Branch("Ncpiplus_true", &Ncpiplus_true, "Ncpiplus_true/I");
    eventVariables->Branch("Ncpiminus_true", &Ncpiminus_true,
                           "Ncpiminus_true/I");
    eventVariables->Branch("Ncpi_true", &Ncpi_true, "Ncpi_true/I");
    eventVariables->Branch("Npi0_true", &Npi0_true, "Npi0_true/I");
    eventVariables->Branch("NcK_true", &NcK_true, "NcK_true/I");
    eventVariables->Branch("NK0_true", &NK0_true, "NK0_true/I");

    eventVariables->Branch("HMFS_clep_rec", &HMFS_clep_rec);
    eventVariables->Branch("HMFS_pip_rec", &HMFS_pip_rec);
    eventVariables->Branch("HMFS_pim_rec", &HMFS_pim_rec);
    eventVariables->Branch("HMFS_cpi_rec", &HMFS_cpi_rec);
    eventVariables->Branch("HMFS_pi0_rec", &HMFS_pi0_rec);
    eventVariables->Branch("HMFS_cK_rec", &HMFS_cK_rec);
    eventVariables->Branch("HMFS_K0_rec", &HMFS_K0_rec);
    eventVariables->Branch("HMFS_p_rec", &HMFS_p_rec);

    eventVariables->Branch("KEFSHad_cpip_rec", &KEFSHad_cpip_rec,
                           "KEFSHad_cpip_rec/F");
    eventVariables->Branch("KEFSHad_cpim_rec", &KEFSHad_cpim_rec,
                           "KEFSHad_cpim_rec/F");
    eventVariables->Branch("KEFSHad_cpi_rec", &KEFSHad_cpi_rec,
                           "KEFSHad_cpi_rec/F");
    eventVariables->Branch("TEFSHad_pi0_rec", &TEFSHad_pi0_rec,
                           "TEFSHad_pi0_rec/F");
    eventVariables->Branch("KEFSHad_cK_rec", &KEFSHad_cK_rec,
                           "KEFSHad_cK_rec/F");
    eventVariables->Branch("KEFSHad_K0_rec", &KEFSHad_K0_rec,
                           "KEFSHad_K0_rec/F");
    eventVariables->Branch("KEFSHad_p_rec", &KEFSHad_p_rec, "KEFSHad_p_rec/F");
    eventVariables->Branch("KEFSHad_n_rec", &KEFSHad_n_rec, "KEFSHad_n_rec/F");

    eventVariables->Branch("EFSHad_rec", &EFSHad_rec, "EFSHad_rec/F");
    eventVariables->Branch("EFSLep_rec", &EFSLep_rec, "EFSLep_rec/F");

    eventVariables->Branch("EFSVis_cpip", &EFSVis_cpip, "EFSVis_cpip/F");
    eventVariables->Branch("EFSVis_cpim", &EFSVis_cpim, "EFSVis_cpim/F");
    eventVariables->Branch("EFSVis_cpi", &EFSVis_cpi, "EFSVis_cpi/F");
    eventVariables->Branch("EFSVis_pi0", &EFSVis_pi0, "EFSVis_pi0/F");
    eventVariables->Branch("EFSVis_cK", &EFSVis_cK, "EFSVis_cK/F");
    eventVariables->Branch("EFSVis_K0", &EFSVis_K0, "EFSVis_K0/F");
    eventVariables->Branch("EFSVis_p", &EFSVis_p, "EFSVis_p/F");
    eventVariables->Branch("EFSVis_n", &EFSVis_n, "EFSVis_n/F");
    eventVariables->Branch("EFSVis_gamma", &EFSVis_gamma, "EFSVis_gamma/F");
    eventVariables->Branch("EFSVis_other", &EFSVis_other, "EFSVis_other/F");
    eventVariables->Branch("EFSVis", &EFSVis, "EFSVis/F");

    eventVariables->Branch("FSCLep_seen", &FSCLep_seen, "FSCLep_seen/I");
    eventVariables->Branch("Nprotons_seen", &Nprotons_seen, "Nprotons_seen/I");
    eventVariables->Branch("Nneutrons_seen", &Nneutrons_seen,
                           "Nneutrons_seen/I");
    eventVariables->Branch("Ncpip_seen", &Ncpip_seen, "Ncpip_seen/I");
    eventVariables->Branch("Ncpim_seen", &Ncpim_seen, "Ncpim_seen/I");
    eventVariables->Branch("Ncpi_seen", &Ncpi_seen, "Ncpi_seen/I");
    eventVariables->Branch("Npi0_seen", &Npi0_seen, "Npi0_seen/I");
    eventVariables->Branch("NcK_seen", &NcK_seen, "NcK_seen/I");
    eventVariables->Branch("NK0_seen", &NK0_seen, "NK0_seen/I");
    eventVariables->Branch("Nothers_seen", &Nothers_seen, "Nothers_seen/I");

    eventVariables->Branch("EISLep_QE_rec", &EISLep_QE_rec, "EISLep_QE_rec/F");
    eventVariables->Branch("EISLep_LepHad_rec", &EISLep_LepHad_rec,
                           "EISLep_LepHad_rec/F");
    eventVariables->Branch("EISLep_LepHadVis_rec", &EISLep_LepHadVis_rec,
                           "EISLep_LepHadVis_rec/F");

    eventVariables->Branch("Nprotons_contributed", &Nprotons_contributed,
                           "Nprotons_contributed/I");
    eventVariables->Branch("Nneutrons_contributed", &Nneutrons_contributed,
                           "Nneutrons_contributed/I");
    eventVariables->Branch("Ncpip_contributed", &Ncpip_contributed,
                           "Ncpip_contributed/I");
    eventVariables->Branch("Ncpim_contributed", &Ncpim_contributed,
                           "Ncpim_contributed/I");
    eventVariables->Branch("Ncpi_contributed", &Ncpi_contributed,
                           "Ncpi_contributed/I");
    eventVariables->Branch("Npi0_contributed", &Npi0_contributed,
                           "Npi0_contributed/I");
    eventVariables->Branch("NcK_contributed", &NcK_contributed,
                           "NcK_contributed/I");
    eventVariables->Branch("NK0_contributed", &NK0_contributed,
                           "NK0_contributed/I");
    eventVariables->Branch("Ngamma_contributed", &Ngamma_contributed,
                           "Ngamma_contributed/I");
    eventVariables->Branch("Nothers_contibuted", &Nothers_contibuted,
                           "Nothers_contibuted/I");

    eventVariables->Branch("Weight", &Weight, "Weight/F");
    eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
    eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
    eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/F");
    eventVariables->Branch("EffWeight", &EffWeight, "EffWeight/F");

    xsecScaling = fScaleFactor;
    eventVariables->Branch("xsecScaling", &xsecScaling, "xsecScaling/F");

    eventVariables->Branch("flagCCINC_true", &flagCCINC_true,
                           "flagCCINC_true/O");
    eventVariables->Branch("flagCC0K_true", &flagCC0K_true, "flagCC0K_true/O");
    eventVariables->Branch("flagCC0Pi_true", &flagCC0Pi_true,
                           "flagCC0Pi_true/O");
    eventVariables->Branch("flagCC1Pi_true", &flagCC1Pi_true,
                           "flagCC1Pi_true/O");

    eventVariables->Branch("flagCCINC_rec", &flagCCINC_rec, "flagCCINC_rec/O");
    eventVariables->Branch("flagCC0K_rec", &flagCC0K_rec, "flagCC0K_rec/O");
    eventVariables->Branch("flagCC0Pi_rec", &flagCC0Pi_rec, "flagCC0Pi_rec/O");
    eventVariables->Branch("flagCC1Pi_rec", &flagCC1Pi_rec, "flagCC1Pi_rec/O");
  }

  PredEvtRateWeight = 1;
  if (fEvtRateScaleFactor != 0xdeadbeef) {
    if (OutputSummaryTree) {
      eventVariables->Branch("PredEvtRateWeight", &PredEvtRateWeight,
                             "PredEvtRateWeight/F");
    }
    PredEvtRateWeight = fScaleFactor * fEvtRateScaleFactor;
  }
}

template <size_t N>
int CountNPdgsSeen(RecoInfo ri, int const (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum +=
        std::count(ri.RecObjClass.begin(), ri.RecObjClass.end(), pdgs[pdg_it]);
  }
  return sum;
}

template <size_t N>
int CountNNotPdgsSeen(RecoInfo ri, int const (&pdgs)[N]) {
  int sum = 0;
  for (size_t p_it = 0; p_it < ri.RecObjClass.size(); ++p_it) {
    if (!std::count(pdgs, pdgs + N, ri.RecObjClass[p_it])) {
      sum++;
    }
  }
  return sum;
}

template <size_t N>
int CountNPdgsContributed(RecoInfo ri, int const (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum += std::count(ri.TrueContribPDGs.begin(), ri.TrueContribPDGs.end(),
                      pdgs[pdg_it]);
  }
  return sum;
}

template <size_t N>
int CountNNotPdgsContributed(RecoInfo ri, int const (&pdgs)[N]) {
  int sum = 0;
  for (size_t p_it = 0; p_it < ri.TrueContribPDGs.size(); ++p_it) {
    if (!std::count(pdgs, pdgs + N, ri.TrueContribPDGs[p_it])) {
      sum++;
    }
  }
  return sum;
}

TLorentzVector GetHMFSRecParticles(RecoInfo ri, int pdg) {
  TLorentzVector mom(0, 0, 0, 0);
  for (size_t p_it = 0; p_it < ri.RecObjMom.size(); ++p_it) {
    if ((ri.RecObjClass[p_it] == pdg) &&
        (mom.Mag() < ri.RecObjMom[p_it].Mag())) {
      mom.SetXYZM(ri.RecObjMom[p_it].X(), ri.RecObjMom[p_it].Y(),
                  ri.RecObjMom[p_it].Z(),
                  PhysConst::GetMass(ri.RecObjClass[p_it]) * 1.0E3);
    }
  }
  return mom;
}

template <size_t N>
double SumKE_RecoInfo(RecoInfo ri, int const (&pdgs)[N], double mass) {
  double sum = 0;
  for (size_t p_it = 0; p_it < ri.RecObjMom.size(); ++p_it) {
    if (!std::count(pdgs, pdgs + N,
                    ri.RecObjClass[p_it])) {  // If we don't care about this
                                              // particle type.
      continue;
    }
    sum += sqrt(ri.RecObjMom[p_it].Mag2() + mass * mass) - mass;
  }

  return sum;
}

template <size_t N>
double SumTE_RecoInfo(RecoInfo ri, int const (&pdgs)[N], double mass) {
  double sum = 0;
  for (size_t p_it = 0; p_it < ri.RecObjMom.size(); ++p_it) {
    if (!std::count(pdgs, pdgs + N,
                    ri.RecObjClass[p_it])) {  // If we don't care about this
                                              // particle type.
      continue;
    }
    sum += sqrt(ri.RecObjMom[p_it].Mag2() + mass * mass);
  }

  return sum;
}

template <size_t N>
double SumVisE_RecoInfo(RecoInfo ri, int const (&pdgs)[N]) {
  double sum = 0;

  for (size_t p_it = 0; p_it < ri.RecVisibleEnergy.size(); ++p_it) {
    if (!std::count(pdgs, pdgs + N,
                    ri.TrueContribPDGs[p_it])) {  // If we don't care about this
                                                  // particle type.
      continue;
    }
    sum += ri.RecVisibleEnergy[p_it];
  }

  return sum;
}

template <size_t N>
double SumVisE_RecoInfo_NotPdgs(RecoInfo ri, int const (&pdgs)[N]) {
  double sum = 0;

  for (size_t p_it = 0; p_it < ri.RecVisibleEnergy.size(); ++p_it) {
    if (std::count(pdgs, pdgs + N,
                   ri.TrueContribPDGs[p_it])) {  // If we know about this
                                                 // particle type.
      continue;
    }
    sum += ri.RecVisibleEnergy[p_it];
  }

  return sum;
}

//********************************************************************
void Smearceptance_Tester::FillEventVariables(FitEvent *event) {
  //********************************************************************

  static int const cpipPDG[] = {211};
  static int const cpimPDG[] = {-211};
  static int const pi0PDG[] = {111};

  static int const cKPDG[] = {321, -321};
  static int const K0PDG[] = {311, 310, 130};

  static int const ProtonPDG[] = {2212};
  static int const NeutronPDG[] = {2112};
  static int const GammaPDG[] = {22};
  static int const CLeptonPDGs[] = {11, 13, 15, -11, -13, -15};
  static int const ExplicitPDGs[] = {211,  -211, 111, 321, -321, 311, 310, 130,
                                     2212, 2112, 22,  11,  13,   15,  12,  14,
                                     16,   -11,  -13, -15, -12,  -14, -16};

  RecoInfo *ri = smearceptor->Smearcept(event);

  //** START Pions

  HMFS_clep_true = TLorentzVector(0, 0, 0, 0);
  HMFS_clep_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsCLep = event->GetHMFSParticle(CLeptonPDGs);
  if (fsCLep) {
    HMFS_clep_true = fsCLep->P4();
    HMFS_clep_rec = GetHMFSRecParticles(*ri, fsCLep->PDG());
  }

  //** END Charged leptons

  //** START Pions

  HMFS_pip_true = TLorentzVector(0, 0, 0, 0);
  HMFS_pip_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsPip = event->GetHMFSPiPlus();
  if (fsPip) {
    HMFS_pip_true = fsPip->P4();
    HMFS_pip_rec = GetHMFSRecParticles(*ri, fsPip->PDG());
  }

  HMFS_pim_true = TLorentzVector(0, 0, 0, 0);
  HMFS_pim_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsPim = event->GetHMFSPiMinus();
  if (fsPim) {
    HMFS_pim_true = fsPim->P4();
    HMFS_pim_rec = GetHMFSRecParticles(*ri, fsPim->PDG());
  }

  HMFS_cpi_true = TLorentzVector(0, 0, 0, 0);
  HMFS_cpi_rec = TLorentzVector(0, 0, 0, 0);
  if (fsPip || fsPim) {
    if (!fsPip) {
      HMFS_cpi_true = HMFS_pim_true;
      HMFS_cpi_rec = HMFS_pim_rec;
    } else if (!fsPim) {
      HMFS_cpi_true = HMFS_pip_true;
      HMFS_cpi_rec = HMFS_pip_rec;
    } else {
      HMFS_cpi_true =
          (fsPip->p2() > fsPim->p2()) ? HMFS_pip_true : HMFS_pim_true;
      HMFS_cpi_rec = (fsPip->p2() > fsPim->p2()) ? HMFS_pip_rec : HMFS_pim_rec;
    }
  }

  HMFS_pi0_true = TLorentzVector(0, 0, 0, 0);
  HMFS_pi0_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsPi0 = event->GetHMFSPiZero();
  if (fsPi0) {
    HMFS_pi0_true = fsPi0->P4();
    HMFS_pi0_rec = GetHMFSRecParticles(*ri, fsPi0->PDG());
  }

  //** END Pions

  //** START Kaons

  HMFS_cK_true = TLorentzVector(0, 0, 0, 0);
  HMFS_cK_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fscK = event->GetHMFSParticle(cKPDG);
  if (fscK) {
    HMFS_cK_true = fscK->P4();
    HMFS_cK_rec = GetHMFSRecParticles(*ri, fscK->PDG());
  }

  HMFS_K0_true = TLorentzVector(0, 0, 0, 0);
  HMFS_K0_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsK0 = event->GetHMFSParticle(K0PDG);
  if (fsK0) {
    HMFS_K0_true = fsK0->P4();
    HMFS_K0_rec = GetHMFSRecParticles(*ri, fsK0->PDG());
  }

  //** END Kaons

  //** START Nucleons

  HMFS_p_true = TLorentzVector(0, 0, 0, 0);
  HMFS_p_rec = TLorentzVector(0, 0, 0, 0);
  FitParticle *fsP = event->GetHMFSProton();
  if (fsP) {
    HMFS_p_true = fsP->P4();
    HMFS_p_rec = GetHMFSRecParticles(*ri, fsP->PDG());
  }

  TLorentzVector FourMomentumTransfer =
      (event->GetHMISAnyLeptons()->P4() - event->GetHMFSAnyLeptons()->P4());

  Omega_true = FourMomentumTransfer.E();
  Q2_true = -1 * FourMomentumTransfer.Mag2();
  Mode_true = event->Mode;

  EISLep_true = event->GetHMISAnyLeptons()->E();
  KEFSHad_cpip_true = FitUtils::SumTE_PartVect(event->GetAllFSPiPlus());
  KEFSHad_cpim_true = FitUtils::SumTE_PartVect(event->GetAllFSPiMinus());
  KEFSHad_cpi_true = KEFSHad_cpip_true + KEFSHad_cpim_true;
  TEFSHad_pi0_true = FitUtils::SumTE_PartVect(event->GetAllFSPiZero());
  KEFSHad_cK_true = FitUtils::SumTE_PartVect(event->GetAllFSParticle(cKPDG));
  KEFSHad_K0_true = FitUtils::SumTE_PartVect(event->GetAllFSParticle(K0PDG));
  KEFSHad_p_true = FitUtils::SumKE_PartVect(event->GetAllFSProton());
  KEFSHad_n_true = FitUtils::SumKE_PartVect(event->GetAllFSNeutron());
  EFSHad_true =
      KEFSHad_cpi_true + TEFSHad_pi0_true + KEFSHad_p_true + KEFSHad_n_true;
  EFSChargedEMHad_true = KEFSHad_cpi_true + TEFSHad_pi0_true + KEFSHad_p_true +
                         KEFSHad_cK_true + KEFSHad_K0_true;

  EFSLep_true = event->GetHMFSAnyLeptons()->E();
  EFSgamma_true = FitUtils::SumTE_PartVect(event->GetAllFSPhoton());

  PDGISLep_true = event->GetHMISAnyLeptons()->PDG();
  PDGFSLep_true = event->GetHMFSAnyLeptons()->PDG();

  Nprotons_true = event->GetAllFSProton().size();
  Nneutrons_true = event->GetAllFSNeutron().size();
  Ncpiplus_true = event->GetAllFSPiPlus().size();
  Ncpiminus_true = event->GetAllFSPiMinus().size();
  Ncpi_true = Ncpiplus_true + Ncpiminus_true;
  Npi0_true = event->GetAllFSPiZero().size();
  NcK_true = event->GetAllFSParticle(cKPDG).size();
  NK0_true = event->GetAllFSParticle(K0PDG).size();

  KEFSHad_cpip_rec =
      SumKE_RecoInfo(*ri, cpipPDG, PhysConst::mass_cpi * PhysConst::mass_MeV);
  KEFSHad_cpim_rec =
      SumKE_RecoInfo(*ri, cpimPDG, PhysConst::mass_cpi * PhysConst::mass_MeV);
  KEFSHad_cpi_rec = KEFSHad_cpip_rec + KEFSHad_cpim_rec;

  TEFSHad_pi0_rec =
      SumTE_RecoInfo(*ri, pi0PDG, PhysConst::mass_pi0 * PhysConst::mass_MeV);

  KEFSHad_cK_rec =
      SumKE_RecoInfo(*ri, cKPDG, PhysConst::mass_cK * PhysConst::mass_MeV);
  KEFSHad_K0_rec =
      SumKE_RecoInfo(*ri, K0PDG, PhysConst::mass_K0 * PhysConst::mass_MeV);

  KEFSHad_p_rec = SumKE_RecoInfo(*ri, ProtonPDG,
                                 PhysConst::mass_proton * PhysConst::mass_MeV);
  KEFSHad_n_rec = SumKE_RecoInfo(*ri, NeutronPDG,
                                 PhysConst::mass_neutron * PhysConst::mass_MeV);
  EFSHad_rec =
      KEFSHad_cpi_rec + TEFSHad_pi0_rec + KEFSHad_p_rec + KEFSHad_n_rec;

  TLorentzVector FSLepMom_rec(0, 0, 0, 0);
  if (event->GetHMFSAnyLeptons()) {
    FSLepMom_rec = GetHMFSRecParticles(*ri, event->GetHMFSAnyLeptons()->PDG());
    EFSLep_rec = FSLepMom_rec.E();
  } else {
    EFSLep_rec = 0;
  }

  EFSVis_cpip = SumVisE_RecoInfo(*ri, cpipPDG);
  EFSVis_cpim = SumVisE_RecoInfo(*ri, cpimPDG);
  EFSVis_cpi = EFSVis_cpip + EFSVis_cpim;
  EFSVis_pi0 = SumVisE_RecoInfo(*ri, pi0PDG);
  EFSVis_cK = SumVisE_RecoInfo(*ri, cKPDG);
  EFSVis_K0 = SumVisE_RecoInfo(*ri, K0PDG);
  EFSVis_p = SumVisE_RecoInfo(*ri, ProtonPDG);
  EFSVis_n = SumVisE_RecoInfo(*ri, NeutronPDG);
  EFSVis_gamma = SumVisE_RecoInfo(*ri, GammaPDG);
  EFSVis_other = SumVisE_RecoInfo_NotPdgs(*ri, ExplicitPDGs);
  EFSVis = EFSVis_cpi + EFSVis_pi0 + EFSVis_p + EFSVis_n + EFSVis_gamma +
           EFSVis_cK + EFSVis_K0;

  FSCLep_seen = CountNPdgsSeen(*ri, CLeptonPDGs);
  Nprotons_seen = CountNPdgsSeen(*ri, ProtonPDG);
  Nneutrons_seen = CountNPdgsSeen(*ri, NeutronPDG);
  Ncpip_seen = CountNPdgsSeen(*ri, cpipPDG);
  Ncpim_seen = CountNPdgsSeen(*ri, cpimPDG);
  Ncpi_seen = Ncpip_seen + Ncpim_seen;
  Npi0_seen = CountNPdgsSeen(*ri, pi0PDG);
  NcK_seen = CountNPdgsSeen(*ri, cKPDG);
  NK0_seen = CountNPdgsSeen(*ri, K0PDG);
  Nothers_seen = CountNNotPdgsSeen(*ri, ExplicitPDGs);

  if (FSCLep_seen && (FSLepMom_rec.Mag() > 1E-8)) {
    EISLep_QE_rec =
        FitUtils::EnuQErec(FSLepMom_rec.Mag() / 1000.0, FSLepMom_rec.CosTheta(),
                           34, PDGFSLep_true > 0) *
        1000.0;
  } else {
    EISLep_QE_rec = 0;
  }
  EISLep_LepHad_rec = EFSLep_rec + EFSHad_rec;
  EISLep_LepHadVis_rec = EFSLep_rec + EFSHad_rec + EFSVis;

  Nprotons_contributed = CountNPdgsContributed(*ri, ProtonPDG);
  Nneutrons_contributed = CountNPdgsContributed(*ri, NeutronPDG);
  Ncpip_contributed = CountNPdgsContributed(*ri, cpipPDG);
  Ncpim_contributed = CountNPdgsContributed(*ri, cpimPDG);
  Ncpi_contributed = Ncpip_contributed + Ncpim_contributed;
  Npi0_contributed = CountNPdgsContributed(*ri, pi0PDG);
  NcK_contributed = CountNPdgsContributed(*ri, cKPDG);
  NK0_contributed = CountNPdgsContributed(*ri, K0PDG);
  Ngamma_contributed = CountNPdgsContributed(*ri, GammaPDG);
  Nothers_contibuted = CountNNotPdgsContributed(*ri, ExplicitPDGs);

  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;
  FluxWeight = GetFluxHistogram()->GetBinContent(
                   GetFluxHistogram()->FindBin(EISLep_true)) /
               GetFluxHistogram()->Integral();
  EffWeight = ri->Weight;

  flagCCINC_true = PDGFSLep_true & 1;
  flagCC0K_true = (NcK_true + NK0_true) == 0;
  flagCC0Pi_true =
      flagCCINC_true && flagCC0K_true && ((Ncpi_true + Npi0_true) == 0);
  flagCC1Pi_true =
      flagCCINC_true && flagCC0K_true && ((Ncpi_true + Npi0_true) == 1);

  flagCCINC_rec = FSCLep_seen && flagCCINC_true;
  flagCC0K_rec = (NcK_seen + NK0_seen) == 0;
  flagCC0Pi_rec =
      flagCCINC_rec && flagCC0K_rec && ((Ncpi_seen + Npi0_seen) == 0);
  flagCC1Pi_rec =
      flagCCINC_rec && flagCC0K_rec && ((Ncpi_seen + Npi0_seen) == 1);

  if (OutputSummaryTree) {
    // Fill the eventVariables Tree
    eventVariables->Fill();
  }

  if (RecoSmear) {
    RecoSmear->Fill(EISLep_true / 1000.0,
                    flagCCINC_rec ? EISLep_LepHadVis_rec / 1000.0 : -1, Weight);
    ETrueDistrib_noweight->Fill(EISLep_true / 1000.0,
                                flagCCINC_true ? Weight : 0);

    ETrueDistrib->Fill(EISLep_true / 1000.0,
                       flagCCINC_true ? Weight * PredEvtRateWeight : 0);

    ERecDistrib->Fill(EISLep_LepHadVis_rec / 1000.0,
                      flagCCINC_rec ? Weight * PredEvtRateWeight : 0);
  }
};

//********************************************************************
void Smearceptance_Tester::Write(std::string drawOpt) {
  //********************************************************************

  if (OutputSummaryTree) {
    // First save the TTree
    eventVariables->Write();
  }

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  if (!RecoSmear) {
    return;
  }

  TH2D *SmearMatrix_ev =
      static_cast<TH2D *>(RecoSmear->Clone("ELepHadVis_Smear_ev"));

  for (Int_t trueAxis_it = 1;
       trueAxis_it < RecoSmear->GetXaxis()->GetNbins() + 1; ++trueAxis_it) {
    double NEISLep = ETrueDistrib_noweight->GetBinContent(trueAxis_it);

    for (Int_t recoAxis_it = 1;
         recoAxis_it < RecoSmear->GetYaxis()->GetNbins() + 1; ++recoAxis_it) {
      if (NEISLep > std::numeric_limits<double>::epsilon()) {
        SmearMatrix_ev->SetBinContent(
            trueAxis_it, recoAxis_it,
            SmearMatrix_ev->GetBinContent(trueAxis_it, recoAxis_it) / NEISLep);
      }
    }
  }

  ETrueDistrib_noweight->Write();
  ETrueDistrib->Write();
  ERecDistrib->Write();

  RecoSmear->Write();

  SmearMatrix_ev->Write();

  TH2D *ResponseMatrix_ev =
      SmearceptanceUtils::SVDGetInverse(SmearMatrix_ev, SVDTruncation);
  ResponseMatrix_ev->SetName("ResponseMatrix_ev");
  ResponseMatrix_ev->Write();

#ifdef DEBUG_SMEARTESTER

  TMatrixD SmearMatrix_ev_md = SmearceptanceUtils::GetMatrix(SmearMatrix_ev);

  TH1D *SmearedEvt = static_cast<TH1D *>(ERecDistrib->Clone());
  SmearedEvt->SetNameTitle("SmearedEvt", ";Rec E_{#nu}; count");

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      ETrueDistrib, SmearedEvt, SmearMatrix_ev_md, 5000, false);

  SmearedEvt->Write();

  SmearedEvt->Scale(1, "width");
  SmearedEvt->SetName("SmearedEvt_bw");
  SmearedEvt->Write();

#endif

#ifdef __PROB3PP_ENABLED__
  FitWeight *fw = FitBase::GetRW();
  if (fw->HasRWEngine(kOSCILLATION)) {
    OscWeightEngine *oscWE =
        dynamic_cast<OscWeightEngine *>(fw->GetRWEngine(kOSCILLATION));
    TGraph POsc;

    POsc.Set(1E4 - 1);

    double min = ETrueDistrib->GetXaxis()->GetBinLowEdge(1);
    double step = (ETrueDistrib->GetXaxis()->GetBinUpEdge(
                       ETrueDistrib->GetXaxis()->GetNbins()) -
                   ETrueDistrib->GetXaxis()->GetBinLowEdge(1)) /
                  double(1E4);

    for (size_t i = 1; i < 1E4; ++i) {
      double enu = min + i * step;
      double ow = oscWE->CalcWeight(enu, 14);
      if (ow != ow) {
        std::cout << "Bad osc weight for ENu: " << enu << std::endl;
      }
      POsc.SetPoint(i - 1, enu, ow);
    }

    POsc.Write("POsc", TObject::kOverwrite);
  }
#endif

  TMatrixD ResponseMatrix_evt_md =
      SmearceptanceUtils::GetMatrix(ResponseMatrix_ev);

  TH1D *Unfolded_enu_obs = static_cast<TH1D *>(ETrueDistrib->Clone());
  Unfolded_enu_obs->SetNameTitle("UnfoldedENu_evt", ";True E_{#nu};count");

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      ERecDistrib, Unfolded_enu_obs, ResponseMatrix_evt_md, 5000, false);

  Unfolded_enu_obs->Write();

  Unfolded_enu_obs->Scale(1, "width");
  Unfolded_enu_obs->SetName("UnfoldedENu_evt_bw");
  Unfolded_enu_obs->Write();

  ETrueDistrib->Scale(1, "width");
  ETrueDistrib->SetName("ELep_rate_bw");
  ETrueDistrib->Write();

  ERecDistrib->Scale(1, "width");
  ERecDistrib->SetName("ELepRec_rate_bw");
  ERecDistrib->Write();
}

// -------------------------------------------------------------------
// Purely MC Plot
// Following functions are just overrides to handle this
// -------------------------------------------------------------------
//********************************************************************
/// Everything is classed as signal...
bool Smearceptance_Tester::isSignal(FitEvent *event) {
  //********************************************************************
  (void)event;
  return true;
};

//********************************************************************
void Smearceptance_Tester::ScaleEvents() {
  //********************************************************************
  // Saving everything to a TTree so no scaling required
  return;
}

//********************************************************************
void Smearceptance_Tester::ApplyNormScale(float norm) {
  //********************************************************************

  // Saving everything to a TTree so no scaling required
  fCurrentNorm = norm;
  return;
}

//********************************************************************
void Smearceptance_Tester::FillHistograms() {
  //********************************************************************
  // No Histograms need filling........
  return;
}

//********************************************************************
void Smearceptance_Tester::ResetAll() {
  //********************************************************************
  if (OutputSummaryTree) {
    eventVariables->Reset();
  }
  return;
}

//********************************************************************
float Smearceptance_Tester::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
