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

//********************************************************************
/// @brief Class to perform smearceptance MC Studies on a custom measurement
Smearceptance_Tester::Smearceptance_Tester(std::string name,
                                           std::string inputfile, FitWeight *rw,
                                           std::string type,
                                           std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  std::vector<std::string> splitName = GeneralUtils::ParseToStr(name, "_");
  size_t firstUS = name.find_first_of("_");

  std::string smearceptorName = name.substr(firstUS + 1);

  fName = name.substr(0, firstUS);
  eventVariables = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 100.;  // Arbritrarily high energy limit

  // Set default fitter flags
  fIsDiag = true;
  fIsShape = false;
  fIsRawEvents = false;

  // This function will sort out the input files automatically and parse all the
  // inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually
  // this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  eventVariables = NULL;

  // Setup fDataHist as a placeholder
  this->fDataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);
  this->SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  this->fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  LOG(SAM) << "Smearceptance Flux Scaling Factor = " << fScaleFactor
           << std::endl;

  if (fScaleFactor <= 0.0) {
    ERR(WRN) << "SCALE FACTOR TOO LOW " << std::endl;
    sleep(20);
  }

  // Setup our TTrees
  this->AddEventVariablesToTree();

  smearceptor = &Smearcepterton::Get().GetSmearcepter(smearceptorName);

  Int_t RecNBins = 20, TrueNBins = 20;
  double RecBinL = 0, TrueBinL = 0, RecBinH = 10, TrueBinH = 10;

  if (Config::Get().GetConfigNode("smear.reconstructed.binning")) {
    std::vector<std::string> args = GeneralUtils::ParseToStr(
        Config::Get().ConfS("smear.reconstructed.binning"), ",");
    RecNBins = GeneralUtils::StrToInt(args[0]);
    RecBinL = GeneralUtils::StrToDbl(args[1]);
    RecBinH = GeneralUtils::StrToDbl(args[2]);
    TrueNBins = RecNBins;
    TrueBinL = RecBinL;
    TrueBinH = RecBinH;
  }

  if (Config::Get().GetConfigNode("smear.true.binning")) {
    std::vector<std::string> args = GeneralUtils::ParseToStr(
        Config::Get().ConfS("smear.true.binning"), ",");
    TrueNBins = GeneralUtils::StrToInt(args[0]);
    TrueBinL = GeneralUtils::StrToDbl(args[1]);
    TrueBinH = GeneralUtils::StrToDbl(args[2]);
  }
  SVDTruncation = 0;
  if (Config::Get().GetConfigNode("smear.true.binning")) {
    SVDTruncation = Config::Get().ConfI("smear.SVD.truncation");
    QLOG(SAM, "Applying SVD truncation of: " << SVDTruncation)
  }

  QLOG(SAM, "Using binning True: " << TrueNBins << ", [" << TrueBinL << " -- "
                                   << TrueBinH << "], Rec: " << RecNBins
                                   << ", [" << RecBinL << " -- " << RecBinH
                                   << "]");

  ETrueDistrib = new TH1D("ELep_rate", ";True E_{#nu};Count", TrueNBins,
                          TrueBinL, TrueBinH);
  ERecDistrib = new TH1D("ELepRec_rate", ";Rec E_{#nu};Count", RecNBins,
                         RecBinL, RecBinH);
  ETrueDistrib->Sumw2();
  ERecDistrib->Sumw2();

  RecoSmear =
      new TH2D("ELepHadVis_Recon", ";Recon. E_{#nu};True E_{#nu}", RecNBins,
               RecBinL, RecBinH, TrueNBins, TrueBinL, TrueBinH);
}

void Smearceptance_Tester::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    FitPar::Config().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  LOG(SAM) << "Adding Event Variables" << std::endl;

  eventVariables->Branch("Omega_true", &Omega_true, "Omega_true/F");
  eventVariables->Branch("Q2_true", &Q2_true, "Q2_true/F");
  eventVariables->Branch("Mode_true", &Mode_true, "Mode_true/I");

  eventVariables->Branch("EISLep_true", &EISLep_true, "EISLep_true/F");

  eventVariables->Branch("KEFSHad_cpip_true", &KEFSHad_cpip_true,
                         "KEFSHad_cpip_true/F");
  eventVariables->Branch("KEFSHad_cpim_true", &KEFSHad_cpim_true,
                         "KEFSHad_cpim_true/F");
  eventVariables->Branch("KEFSHad_cpi_true", &KEFSHad_cpi_true,
                         "KEFSHad_cpi_true/F");
  eventVariables->Branch("TEFSHad_pi0_true", &TEFSHad_pi0_true,
                         "TEFSHad_pi0_true/F");
  eventVariables->Branch("KEFSHad_p_true", &KEFSHad_p_true, "KEFSHad_p_true/F");
  eventVariables->Branch("KEFSHad_n_true", &KEFSHad_n_true, "KEFSHad_n_true/F");

  eventVariables->Branch("EFSHad_true", &EFSHad_true, "EFSHad_true/F");
  eventVariables->Branch("EFSChargedEMHad_true", &EFSChargedEMHad_true,
                         "EFSChargedEMHad_true/F");

  eventVariables->Branch("EFSLep_true", &EFSLep_true, "EFSLep_true/F");
  eventVariables->Branch("EFSgamma_true", &EFSgamma_true, "EFSgamma_true/F");

  eventVariables->Branch("PDGISLep_true", &PDGISLep_true, "PDGISLep_true/I");
  eventVariables->Branch("PDGFSLep_true", &PDGFSLep_true, "PDGFSLep_true/I");

  eventVariables->Branch("Nprotons_true", &Nprotons_true, "Nprotons_true/I");
  eventVariables->Branch("Nneutrons_true", &Nneutrons_true, "Nneutrons_true/I");
  eventVariables->Branch("Ncpiplus_true", &Ncpiplus_true, "Ncpiplus_true/I");
  eventVariables->Branch("Ncpiminus_true", &Ncpiminus_true, "Ncpiminus_true/I");
  eventVariables->Branch("Ncpi_true", &Ncpi_true, "Ncpi_true/I");
  eventVariables->Branch("Npi0_true", &Npi0_true, "Npi0_true/I");

  eventVariables->Branch("KEFSHad_cpip_rec", &KEFSHad_cpip_rec,
                         "KEFSHad_cpip_rec/F");
  eventVariables->Branch("KEFSHad_cpim_rec", &KEFSHad_cpim_rec,
                         "KEFSHad_cpim_rec/F");
  eventVariables->Branch("KEFSHad_cpi_rec", &KEFSHad_cpi_rec,
                         "KEFSHad_cpi_rec/F");
  eventVariables->Branch("TEFSHad_pi0_rec", &TEFSHad_pi0_rec,
                         "TEFSHad_pi0_rec/F");
  eventVariables->Branch("KEFSHad_p_rec", &KEFSHad_p_rec, "KEFSHad_p_rec/F");
  eventVariables->Branch("KEFSHad_n_rec", &KEFSHad_n_rec, "KEFSHad_n_rec/F");

  eventVariables->Branch("EFSHad_rec", &EFSHad_rec, "EFSHad_rec/F");
  eventVariables->Branch("EFSLep_rec", &EFSLep_rec, "EFSLep_rec/F");

  eventVariables->Branch("EFSVis_cpip", &EFSVis_cpip, "EFSVis_cpip/F");
  eventVariables->Branch("EFSVis_cpim", &EFSVis_cpim, "EFSVis_cpim/F");
  eventVariables->Branch("EFSVis_cpi", &EFSVis_cpi, "EFSVis_cpi/F");
  eventVariables->Branch("EFSVis_pi0", &EFSVis_pi0, "EFSVis_pi0/F");
  eventVariables->Branch("EFSVis_p", &EFSVis_p, "EFSVis_p/F");
  eventVariables->Branch("EFSVis_n", &EFSVis_n, "EFSVis_n/F");
  eventVariables->Branch("EFSVis_gamma", &EFSVis_gamma, "EFSVis_gamma/F");
  eventVariables->Branch("EFSVis_other", &EFSVis_other, "EFSVis_other/F");
  eventVariables->Branch("EFSVis", &EFSVis, "EFSVis/F");

  eventVariables->Branch("FSCLep_seen", &FSCLep_seen, "FSCLep_seen/I");
  eventVariables->Branch("Nprotons_seen", &Nprotons_seen, "Nprotons_seen/I");
  eventVariables->Branch("Nneutrons_seen", &Nneutrons_seen, "Nneutrons_seen/I");
  eventVariables->Branch("Ncpip_seen", &Ncpip_seen, "Ncpip_seen/I");
  eventVariables->Branch("Ncpim_seen", &Ncpim_seen, "Ncpim_seen/I");
  eventVariables->Branch("Ncpi_seen", &Ncpi_seen, "Ncpi_seen/I");
  eventVariables->Branch("Npi0_seen", &Npi0_seen, "Npi0_seen/I");
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
  eventVariables->Branch("Ngamma_contributed", &Ngamma_contributed,
                         "Ngamma_contributed/I");
  eventVariables->Branch("Nothers_contibuted", &Nothers_contibuted,
                         "Nothers_contibuted/I");

  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/F");
  eventVariables->Branch("EffWeight", &EffWeight, "EffWeight/F");

  eventVariables->Branch("xsecScaling", &xsecScaling, "xsecScaling/F");

  eventVariables->Branch("flagCCINC_true", &flagCCINC_true, "flagCCINC_true/O");
  eventVariables->Branch("flagCC0Pi_true", &flagCC0Pi_true, "flagCC0Pi_true/O");

  eventVariables->Branch("flagCCINC_rec", &flagCCINC_rec, "flagCCINC_rec/O");
  eventVariables->Branch("flagCC0Pi_rec", &flagCC0Pi_rec, "flagCC0Pi_rec/O");

#ifdef DEBUG_SMEARTESTER
  eventVariables->Branch("FSMuon_True", &FSMuon_True);
  eventVariables->Branch("FSMuon_Smeared", &FSMuon_Smeared);
#endif
}

template <size_t N>
int CountNPdgsSeen(RecoInfo ri, int (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum +=
        std::count(ri.RecObjClass.begin(), ri.RecObjClass.end(), pdgs[pdg_it]);
  }
  return sum;
}

template <size_t N>
int CountNNotPdgsSeen(RecoInfo ri, int (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum +=
        (std::count(ri.RecObjClass.begin(), ri.RecObjClass.end(), pdgs[pdg_it])
             ? 0
             : 1);
  }
  return sum;
}

template <size_t N>
int CountNPdgsContributed(RecoInfo ri, int (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum += std::count(ri.TrueContribPDGs.begin(), ri.TrueContribPDGs.end(),
                      pdgs[pdg_it]);
  }
  return sum;
}

template <size_t N>
int CountNNotPdgsContributed(RecoInfo ri, int (&pdgs)[N]) {
  int sum = 0;
  for (size_t pdg_it = 0; pdg_it < N; ++pdg_it) {
    sum += (std::count(ri.TrueContribPDGs.begin(), ri.TrueContribPDGs.end(),
                       pdgs[pdg_it])
                ? 0
                : 1);
  }
  return sum;
}

TVector3 GetHMFSRecParticles(RecoInfo ri, int pdg) {
  TVector3 mom(0, 0, 0);
  for (size_t p_it = 0; p_it < ri.RecObjMom.size(); ++p_it) {
    if ((ri.RecObjClass[p_it] == pdg) &&
        (mom.Mag() < ri.RecObjMom[p_it].Mag())) {
      mom = ri.RecObjMom[p_it];
    }
  }
  return mom;
}

template <size_t N>
double SumKE_RecoInfo(RecoInfo ri, int (&pdgs)[N], double mass) {
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
double SumTE_RecoInfo(RecoInfo ri, int (&pdgs)[N], double mass) {
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
double SumVisE_RecoInfo(RecoInfo ri, int (&pdgs)[N]) {
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
double SumVisE_RecoInfo_NotPdgs(RecoInfo ri, int (&pdgs)[N]) {
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

  int cpipPDG[] = {211};
  int cpimPDG[] = {-211};
  int pi0PDG[] = {111};
  int ProtonPDG[] = {2212};
  int NeutronPDG[] = {2112};
  int GammaPDG[] = {22};
  int CLeptonPDGs[] = {11, 13, 15};
  int ExplicitPDGs[] = {211, -211, 11, 2212, 2112, 22, 11, 13, 15, 12, 14, 16};

  RecoInfo *ri = smearceptor->Smearcept(event);

#ifdef DEBUG_SMEARTESTER

  FSMuon_True = TVector3(0, 0, 0);
  FSMuon_Smeared = TVector3(0, 0, 0);
  FitParticle *fsMu = event->GetHMFSMuon();
  if (fsMu) {
    FSMuon_True = fsMu->P3();
    FSMuon_Smeared = GetHMFSRecParticles(*ri, 13);
  }
#endif

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
  KEFSHad_p_true = FitUtils::SumKE_PartVect(event->GetAllFSProton());
  KEFSHad_n_true = FitUtils::SumKE_PartVect(event->GetAllFSNeutron());
  EFSHad_true =
      KEFSHad_cpi_true + TEFSHad_pi0_true + KEFSHad_p_true + KEFSHad_n_true;
  EFSChargedEMHad_true = KEFSHad_cpi_true + TEFSHad_pi0_true + KEFSHad_p_true;

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

  KEFSHad_cpip_rec =
      SumKE_RecoInfo(*ri, cpipPDG, PhysConst::mass_cpi * PhysConst::mass_MeV);
  KEFSHad_cpim_rec =
      SumKE_RecoInfo(*ri, cpimPDG, PhysConst::mass_cpi * PhysConst::mass_MeV);
  KEFSHad_cpi_rec = KEFSHad_cpip_rec + KEFSHad_cpim_rec;
  TEFSHad_pi0_rec =
      SumTE_RecoInfo(*ri, pi0PDG, PhysConst::mass_pi0 * PhysConst::mass_MeV);
  KEFSHad_p_rec = SumKE_RecoInfo(*ri, ProtonPDG,
                                 PhysConst::mass_proton * PhysConst::mass_MeV);
  KEFSHad_n_rec = SumKE_RecoInfo(*ri, NeutronPDG,
                                 PhysConst::mass_neutron * PhysConst::mass_MeV);
  EFSHad_rec =
      KEFSHad_cpi_rec + TEFSHad_pi0_rec + KEFSHad_p_rec + KEFSHad_n_rec;

  TVector3 FSLepMom_rec(0, 0, 0);
  if (event->GetHMFSAnyLeptons()) {
    double massLep = event->GetHMFSAnyLeptons()->M();
    FSLepMom_rec = GetHMFSRecParticles(*ri, event->GetHMFSAnyLeptons()->PDG());
    EFSLep_rec = (FSLepMom_rec.Mag() > 1E-5)
                     ? sqrt(FSLepMom_rec * FSLepMom_rec + massLep * massLep)
                     : 0;
  } else {
    EFSLep_rec = 0;
  }

  EFSVis_cpip = SumVisE_RecoInfo(*ri, cpipPDG);
  EFSVis_cpim = SumVisE_RecoInfo(*ri, cpimPDG);
  EFSVis_cpi = EFSVis_cpip + EFSVis_cpim;
  EFSVis_pi0 = SumVisE_RecoInfo(*ri, pi0PDG);
  EFSVis_p = SumVisE_RecoInfo(*ri, ProtonPDG);
  EFSVis_n = SumVisE_RecoInfo(*ri, NeutronPDG);
  EFSVis_gamma = SumVisE_RecoInfo(*ri, GammaPDG);
  EFSVis_other = SumVisE_RecoInfo_NotPdgs(*ri, ExplicitPDGs);
  EFSVis = EFSVis_cpip + EFSVis_cpim + EFSVis_cpi + EFSVis_pi0 + EFSVis_p +
           EFSVis_n + EFSVis_gamma;

  FSCLep_seen = CountNPdgsSeen(*ri, CLeptonPDGs);
  Nprotons_seen = CountNPdgsSeen(*ri, ProtonPDG);
  Nneutrons_seen = CountNPdgsSeen(*ri, NeutronPDG);
  Ncpip_seen = CountNPdgsSeen(*ri, cpipPDG);
  Ncpim_seen = CountNPdgsSeen(*ri, cpimPDG);
  Ncpi_seen = Ncpip_seen + Ncpim_seen;
  Npi0_seen = CountNPdgsSeen(*ri, pi0PDG);
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
  Ngamma_contributed = CountNPdgsContributed(*ri, GammaPDG);
  Nothers_contibuted = CountNNotPdgsContributed(*ri, ExplicitPDGs);

  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;
  FluxWeight = GetFluxHistogram()->GetBinContent(
                   GetFluxHistogram()->FindBin(EISLep_true)) /
               GetFluxHistogram()->Integral();
  EffWeight = ri->Weight;

  xsecScaling = fScaleFactor;

  flagCCINC_true = PDGFSLep_true & 1;
  flagCC0Pi_true = (Ncpi_true + Npi0_true) == 0;

  flagCCINC_rec = FSCLep_seen && PDGFSLep_true & 1;
  flagCC0Pi_rec = ((Ncpi_seen + Npi0_seen) == 0) && flagCCINC_rec;

  // Fill the eventVariables Tree
  eventVariables->Fill();

  RecoSmear->Fill(flagCCINC_rec ? EISLep_LepHadVis_rec / 1000.0 : -1,
                  EISLep_true / 1000.0, Weight);
  ETrueDistrib->Fill(EISLep_true / 1000.0, flagCCINC_true ? Weight : 0);

  ERecDistrib->Fill(EISLep_LepHadVis_rec / 1000.0, flagCCINC_rec ? Weight : 0);

  return;
};

//********************************************************************
void Smearceptance_Tester::Write(std::string drawOpt) {
  //********************************************************************

  // First save the TTree
  eventVariables->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  TH2D *SmearMatrix_ev =
      static_cast<TH2D *>(RecoSmear->Clone("ELepHadVis_Smear_ev"));

  for (Int_t trueAxis_it = 1;
       trueAxis_it < RecoSmear->GetYaxis()->GetNbins() + 1; ++trueAxis_it) {
    double NEISLep = ETrueDistrib->GetBinContent(trueAxis_it);

    for (Int_t recoAxis_it = 1;
         recoAxis_it < RecoSmear->GetXaxis()->GetNbins() + 1; ++recoAxis_it) {
      if (NEISLep > std::numeric_limits<double>::epsilon()) {
        SmearMatrix_ev->SetBinContent(
            recoAxis_it, trueAxis_it,
            SmearMatrix_ev->GetBinContent(recoAxis_it, trueAxis_it) / NEISLep);
      }
    }
  }

  ETrueDistrib->Write();
  ERecDistrib->Write();

  RecoSmear->Write();

  SmearMatrix_ev->Write();

  TH2D *ResponseMatrix_ev =
      SmearceptanceUtils::SVDGetInverse(SmearMatrix_ev, SVDTruncation);
  ResponseMatrix_ev = SmearceptanceUtils::SwapXYTH2D(ResponseMatrix_ev);
  ResponseMatrix_ev->SetName("ResponseMatrix_ev");
  ResponseMatrix_ev->Write();

#ifdef DEBUG_SMEARTESTER

  TMatrixD SmearMatrix_ev_md = SmearceptanceUtils::GetMatrix(
      SmearceptanceUtils::SwapXYTH2D(SmearMatrix_ev));

  TH1D *SmearedEvt = static_cast<TH1D *>(ERecDistrib->Clone());
  SmearedEvt->SetNameTitle("SmearedEvt", ";Rec E_{#nu}; count");

  SmearceptanceUtils::PushTH1ThroughMatrixWithErrors(
      ETrueDistrib, SmearedEvt, SmearMatrix_ev_md, 5000, false);

  SmearedEvt->Write();

  SmearedEvt->Scale(1, "width");
  SmearedEvt->SetName("SmearedEvt_bw");
  SmearedEvt->Write();

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
  this->fCurrentNorm = norm;
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
  eventVariables->Reset();
  return;
}

//********************************************************************
float Smearceptance_Tester::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
