// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "GenericFlux_Tester.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
GenericFlux_Tester::GenericFlux_Tester(std::string name, std::string inputfile,
                                       FitWeight *rw, std::string type,
                                       std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  measurementName = name;
  eventVariables = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 100000.;  // Arbritrarily high energy limit

  // Set default fitter flags
  isDiag = true;
  isShape = false;
  isRawEvents = false;

  nu_4mom = 0;
  pmu = 0;
  ppip = 0;
  ppim = 0;
  ppi0 = 0;
  pprot = 0;
  pneut = 0;

  // This function will sort out the input files automatically and parse all the
  // inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually
  // this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  eventVariables = NULL;

  // Setup dataHist as a placeholder
  this->dataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);
  this->SetupDefaultHist();
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  this->scaleFactor = (this->eventHist->Integral() * 1E-38 / (nevents + 0.)) /
                      this->TotalIntegratedFlux();

  // Setup our TTrees
  this->AddEventVariablesToTree();
  this->AddSignalFlagsToTree();
}

void GenericFlux_Tester::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    FitPar::Config().out->cd();
    eventVariables = new TTree((this->measurementName + "_VARS").c_str(),
                               (this->measurementName + "_VARS").c_str());
  }

  eventVariables->Branch("Mode", &Mode, "Mode/I");

  eventVariables->Branch("PDGnu", &PDGnu, "PDGnu/I");
  eventVariables->Branch("Enu_true", &Enu_true, "Enu_true/F");

  eventVariables->Branch("Nleptons", &Nleptons, "Nleptons/I");
  eventVariables->Branch("MLep", &MLep, "MLep/F");
  eventVariables->Branch("ELep", &ELep, "ELep/F");
  eventVariables->Branch("TLep", &TLep, "TLep/F");
  eventVariables->Branch("CosLep", &CosLep, "CosLep/F");
  eventVariables->Branch("CosPmuPpip", &CosPmuPpip, "CosPmuPpip/F");
  eventVariables->Branch("CosPmuPpim", &CosPmuPpim, "CosPmuPpim/F");
  eventVariables->Branch("CosPmuPpi0", &CosPmuPpi0, "CosPmuPpi0/F");
  eventVariables->Branch("CosPmuPprot", &CosPmuPprot, "CosPmuPprot/F");
  eventVariables->Branch("CosPmuPneut", &CosPmuPneut, "CosPmuPneut/F");

  eventVariables->Branch("Nprotons", &Nprotons, "Nprotons/I");
  eventVariables->Branch("MPr", &MPr, "MPr/F");
  eventVariables->Branch("EPr", &EPr, "EPr/F");
  eventVariables->Branch("TPr", &TPr, "TPr/F");
  eventVariables->Branch("CosPr", &CosPr, "CosPr/F");
  eventVariables->Branch("CosPprotPneut", &CosPprotPneut, "CosPprotPneut/F");

  eventVariables->Branch("Nneutrons", &Nneutrons, "Nneutrons/I");
  eventVariables->Branch("MNe", &MNe, "MNe/F");
  eventVariables->Branch("ENe", &ENe, "ENe/F");
  eventVariables->Branch("TNe", &TNe, "TNe/F");
  eventVariables->Branch("CosNe", &CosNe, "CosNe/F");

  eventVariables->Branch("Npiplus", &Npiplus, "Npiplus/I");
  eventVariables->Branch("MPiP", &MPiP, "MPiP/F");
  eventVariables->Branch("EPiP", &EPiP, "EPiP/F");
  eventVariables->Branch("TPiP", &TPiP, "TPiP/F");
  eventVariables->Branch("CosPiP", &CosPiP, "CosPiP/F");
  eventVariables->Branch("CosPpipPprot", &CosPpipPprot, "CosPpipProt/F");
  eventVariables->Branch("CosPpipPneut", &CosPpipPneut, "CosPpipPneut/F");
  eventVariables->Branch("CosPpipPpim", &CosPpipPpim, "CosPpipPpim/F");
  eventVariables->Branch("CosPpipPpi0", &CosPpipPpi0, "CosPpipPpi0/F");

  eventVariables->Branch("Npineg", &Npineg, "Npineg/I");
  eventVariables->Branch("MPiN", &MPiN, "MPiN/F");
  eventVariables->Branch("EPiN", &EPiN, "EPiN/F");
  eventVariables->Branch("TPiN", &TPiN, "TPiN/F");
  eventVariables->Branch("CosPiN", &CosPiN, "CosPiN/F");
  eventVariables->Branch("CosPpimPprot", &CosPpimPprot, "CosPpimPprot/F");
  eventVariables->Branch("CosPpimPneut", &CosPpimPneut, "CosPpimPneut/F");
  eventVariables->Branch("CosPpimPpi0", &CosPpimPpi0, "CosPpimPpi0/F");

  eventVariables->Branch("Npi0", &Npi0, "Npi0/I");
  eventVariables->Branch("MPi0", &MPi0, "MPi0/F");
  eventVariables->Branch("EPi0", &EPi0, "EPi0/F");
  eventVariables->Branch("TPi0", &TPi0, "TPi0/F");
  eventVariables->Branch("CosPi0", &CosPi0, "CosPi0/F");
  eventVariables->Branch("CosPi0Pprot", &CosPi0Pprot, "CosPi0Pprot/F");
  eventVariables->Branch("CosPi0Pneut", &CosPi0Pneut, "CosPi0Pneut/F");

  eventVariables->Branch("Q2_true", &Q2_true, "Q2_true/F");
  eventVariables->Branch("q0_true", &q0_true, "q0_true/F");
  eventVariables->Branch("q3_true", &q3_true, "q3_true/F");

  eventVariables->Branch("Enu_QE", &Enu_QE, "Enu_QE/F");
  eventVariables->Branch("Q2_QE", &Q2_QE, "Q2_QE/F");

  eventVariables->Branch("W_nuc_rest", &W_nuc_rest, "W_nuc_rest/F");
  eventVariables->Branch("bjorken_x", &bjorken_x, "bjorken_x/F");
  eventVariables->Branch("bjorken_y", &bjorken_y, "bjorken_y/F");

  eventVariables->Branch("Erecoil_true", &Erecoil_true, "Erecoil_true/F");
  eventVariables->Branch("Erecoil_charged", &Erecoil_charged,
                         "Erecoil_charged/F");
  eventVariables->Branch("Erecoil_minerva", &Erecoil_minerva,
                         "Erecoil_minerva/F");

  eventVariables->Branch("nu_4mom", &nu_4mom);
  eventVariables->Branch("pmu_4mom", &pmu);
  eventVariables->Branch("hm_ppip_4mom", &ppip);
  eventVariables->Branch("hm_ppim_4mom", &ppim);
  eventVariables->Branch("hm_ppi0_4mom", &ppi0);
  eventVariables->Branch("hm_pprot_4mom", &pprot);
  eventVariables->Branch("hm_pneut_4mom", &pneut);

  // Event Scaling Information
  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/F");
  eventVariables->Branch("scaleFactor", &scaleFactor, "scaleFactor/F");

  return;
}

void GenericFlux_Tester::AddSignalFlagsToTree() {
  if (!eventVariables) {
    FitPar::Config().out->cd();
    eventVariables = new TTree((this->measurementName + "_VARS").c_str(),
                               (this->measurementName + "_VARS").c_str());
  }

  // Signal Definitions from SignalDef.cxx
  eventVariables->Branch("flagCCQE_full", &flagCCQE_full, "flagCCQE_full/O");
  eventVariables->Branch("flagCCQE_rest", &flagCCQE_rest, "flagCCQE_rest/O");
  eventVariables->Branch("flagCCQEBar_full", &flagCCQEBar_full,
                         "flagCCQEBar_full/O");
  eventVariables->Branch("flagCCQEBar_rest", &flagCCQEBar_rest,
                         "flagCCQEBar_rest/O");

  eventVariables->Branch("flagCC1pip_MiniBooNE", &flagCC1pip_MiniBooNE,
                         "flagCC1pip_MiniBooNE/O");

  eventVariables->Branch("flagCC1pip_MINERvA_full", &flagCC1pip_MINERvA_full,
                         "flagCC1pip_MINERvA_full/O");
  eventVariables->Branch("flagCC1pip_MINERvA_rest", &flagCC1pip_MINERvA_rest,
                         "flagCC1pip_MINERvA_rest/O");
  eventVariables->Branch("flagCCNpip_MINERvA_full", &flagCCNpip_MINERvA_full,
                         "flagCCNpip_MINERvA_full/O");
  eventVariables->Branch("flagCCNpip_MINERvA_rest", &flagCCNpip_MINERvA_rest,
                         "flagCCNpip_MINERvA_rest/O");

  eventVariables->Branch("flagCC1pip_T2K_Michel", &flagCC1pip_T2K_Michel,
                         "flagCC1pip_T2K_Michel/O");
  eventVariables->Branch("flagCC1pip_T2K", &flagCC1pip_T2K, "flagCC1pip_T2K/O");

  eventVariables->Branch("flagCC1pi0_MiniBooNE", &flagCC1pi0_MiniBooNE,
                         "flagCC1pi0_MiniBooNE/O");

  eventVariables->Branch("flagCC1pi0Bar_MINERvA", &flagCC1pi0Bar_MINERvA,
                         "flagCC1pi0Bar_MINERvA/O");

  eventVariables->Branch("flagNC1pi0_MiniBooNE", &flagNC1pi0_MiniBooNE,
                         "flagNC1pi0_MiniBooNE/O");
  eventVariables->Branch("flagNC1pi0Bar_MiniBooNE", &flagNC1pi0Bar_MiniBooNE,
                         "flagNC1pi0Bar_MiniBooNE/O");

  eventVariables->Branch("flagCCcoh_MINERvA", &flagCCcoh_MINERvA,
                         "flagCCcoh_MINERvA/O");
  eventVariables->Branch("flagCCcohBar_MINERvA", &flagCCcohBar_MINERvA,
                         "flagCCcohBar_MINERvA/O");

  eventVariables->Branch("flagCCQEnumu_MINERvA_full",
                         &flagCCQEnumu_MINERvA_full,
                         "flagCCQEnumu_MINERvA_full/O");
  eventVariables->Branch("flagCCQEnumubar_MINERvA_full",
                         &flagCCQEnumubar_MINERvA_full,
                         "flagCCQEnumubar_MINERvA_full/O");
  eventVariables->Branch("flagCCQEnumu_MINERvA_rest",
                         &flagCCQEnumu_MINERvA_rest,
                         "flagCCQEnumu_MINERvA_rest/O");
  eventVariables->Branch("flagCCQEnumubar_MINERvA_rest",
                         &flagCCQEnumubar_MINERvA_rest,
                         "flagCCQEnumubar_MINERvA_rest/O");

  eventVariables->Branch("flagCCincLowRecoil_MINERvA",
                         &flagCCincLowRecoil_MINERvA,
                         "flagCCincLowRecoil_MINERvA/O");
  eventVariables->Branch("flagCCincLowRecoil_MINERvA_reqhad",
                         &flagCCincLowRecoil_MINERvA_reqhad,
                         "flagCCincLowRecoil_MINERvA_reqhad/O");

  eventVariables->Branch("flagCCQELike_MiniBooNE", &flagCCQELike_MiniBooNE,
                         "flagCCQELike_MiniBooNE/O");
  eventVariables->Branch("flagCCQE_MiniBooNE", &flagCCQE_MiniBooNE,
                         "flagCCQE_MiniBooNE/O");
  eventVariables->Branch("flagCCQEBar_MiniBooNE", &flagCCQEBar_MiniBooNE,
                         "flagCCQEBar_MiniBooNE/O");
};

//********************************************************************
void GenericFlux_Tester::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Fill Signal Variables
  FillSignalFlags(event);
  //  std::cout<<"Filling signal"<<std::endl;
  // Function used to extract any variables of interest to the event
  Mode = event->Mode;
  Nleptons = 0;
  Nparticles = 0;
  PDGnu = 0;
  PDGLep = 0;

  Enu_true = Enu_QE = Q2_true = Q2_QE = TLep = TPr = TNe = TPiP = TPiN = TPi0 =
      -999.9;

  Nprotons = 0;
  PPr = EPr = MPr = CosPr = -999.9;

  Nneutrons = 0;
  PNe = ENe = MNe = CosNe = -999.9;

  Npiplus = 0;
  PPiP = EPiP = MPiP = CosPiP = -999.9;

  Npineg = 0;
  PPiN = EPiN = MPiN = CosPiN = -999.9;

  Npi0 = 0;
  PPi0 = EPi0 = MPi0 = CosPi0 = -999.9;

  // All of the angles Clarence added
  CosPmuPpip = CosPmuPpim = CosPmuPpi0 = CosPmuPprot = CosPmuPneut =
      CosPpipPprot = CosPpipPneut = CosPpipPpim = CosPpipPpi0 = CosPpimPprot =
          CosPpimPneut = CosPpimPpi0 = CosPi0Pprot = CosPi0Pneut =
              CosPprotPneut = -999.9;

  float proton_highmom = -999.9;
  float neutron_highmom = -999.9;
  float piplus_highmom = -999.9;
  float pineg_highmom = -999.9;
  float pi0_highmom = -999.9;

  (*nu_4mom) = event->PartInfo(0)->fP;
  (*pmu) = TLorentzVector(0, 0, 0, 0);
  (*ppip) = TLorentzVector(0, 0, 0, 0);
  (*ppim) = TLorentzVector(0, 0, 0, 0);
  (*ppi0) = TLorentzVector(0, 0, 0, 0);
  (*pprot) = TLorentzVector(0, 0, 0, 0);
  (*pneut) = TLorentzVector(0, 0, 0, 0);

  Enu_true = nu_4mom->E();
  PDGnu = event->PartInfo(0)->fPID;

  bool cc = (abs(event->Mode) < 30);

  // Add all pion distributions for the event.

  // Add classifier for CC0pi or CC1pi or CCOther
  // Save Modes Properly
  // Save low recoil measurements

  // Start Particle Loop
  UInt_t npart = event->Npart();
  for (UInt_t i = 0; i < npart; i++) {
    // Skip particles that weren't in the final state
    bool part_alive = event->PartInfo(i)->fIsAlive;
    if (!part_alive) continue;

    // PDG Particle
    int PDGpart = event->PartInfo(i)->fPID;
    TLorentzVector part_4mom = event->PartInfo(i)->fP;

    Nparticles++;

    // Get Charged Lepton
    if (abs(PDGpart) == abs(PDGnu) - 1) {
      Nleptons++;

      PDGLep = PDGpart;

      TLep = FitUtils::T(part_4mom) * 1000.0;
      PLep = (part_4mom.Vect().Mag());
      ELep = (part_4mom.E());
      MLep = (part_4mom.Mag());
      CosLep = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));
      (*pmu) = part_4mom;

      Q2_true = -1 * (part_4mom - (*nu_4mom)).Mag2();

      float ThetaLep = (event->PartInfo(0))
                           ->fP.Vect()
                           .Angle((event->PartInfo(i))->fP.Vect());

      q0_true = (part_4mom - (*nu_4mom)).E();
      q3_true = (part_4mom - (*nu_4mom)).Vect().Mag();

      // Get W_true with assumption of initial state nucleon at rest
      float m_n = 938.27208;
      W_nuc_rest = sqrt(-Q2_true + 2 * m_n * (Enu_true - ELep) + m_n * m_n);

      // Get the Bjorken x and y variables
      // Assume that E_had = Enu - Emu as in MINERvA
      bjorken_x = Q2_true / (2 * m_n * (Enu_true - ELep));
      bjorken_y = 1 - ELep / Enu_true;

      // Quasi-elastic ----------------------
      // ------------------------------------

      // Q2 QE Assuming Carbon Input. Should change this to be dynamic soon.
      Q2_QE =
          FitUtils::Q2QErec(part_4mom, cos(ThetaLep), 34., true) * 1000000.0;
      Enu_QE = FitUtils::EnuQErec(part_4mom, cos(ThetaLep), 34., true) * 1000.0;

      // Pion Production ----------------------
      // --------------------------------------

    } else if (PDGpart == 2212) {
      Nprotons++;
      if (part_4mom.Vect().Mag() > proton_highmom) {
        proton_highmom = part_4mom.Vect().Mag();

        PPr = (part_4mom.Vect().Mag());
        EPr = (part_4mom.E());
        TPr = FitUtils::T(part_4mom) * 1000.;
        MPr = (part_4mom.Mag());
        CosPr = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));

        (*pprot) = part_4mom;
      }
    } else if (PDGpart == 2112) {
      Nneutrons++;
      if (part_4mom.Vect().Mag() > neutron_highmom) {
        neutron_highmom = part_4mom.Vect().Mag();

        PNe = (part_4mom.Vect().Mag());
        ENe = (part_4mom.E());
        TNe = FitUtils::T(part_4mom) * 1000.;
        MNe = (part_4mom.Mag());
        CosNe = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));

        (*pneut) = part_4mom;
      }
    } else if (PDGpart == 211) {
      Npiplus++;
      if (part_4mom.Vect().Mag() > piplus_highmom) {
        piplus_highmom = part_4mom.Vect().Mag();

        PPiP = (part_4mom.Vect().Mag());
        EPiP = (part_4mom.E());
        TPiP = FitUtils::T(part_4mom) * 1000.;
        MPiP = (part_4mom.Mag());
        CosPiP = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));

        (*ppip) = part_4mom;
      }
    } else if (PDGpart == -211) {
      Npineg++;
      if (part_4mom.Vect().Mag() > pineg_highmom) {
        pineg_highmom = part_4mom.Vect().Mag();

        PPiN = (part_4mom.Vect().Mag());
        EPiN = (part_4mom.E());
        TPiN = FitUtils::T(part_4mom) * 1000.;
        MPiN = (part_4mom.Mag());
        CosPiN = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));

        (*ppim) = part_4mom;
      }
    } else if (PDGpart == 111) {
      Npi0++;
      if (part_4mom.Vect().Mag() > pi0_highmom) {
        pi0_highmom = part_4mom.Vect().Mag();

        PPi0 = (part_4mom.Vect().Mag());
        EPi0 = (part_4mom.E());
        TPi0 = FitUtils::T(part_4mom) * 1000.;
        MPi0 = (part_4mom.Mag());
        CosPi0 = cos(part_4mom.Vect().Angle(nu_4mom->Vect()));

        (*ppi0) = part_4mom;
      }
    }
  }

  // Get Recoil Definitions ------
  // -----------------------------
  Erecoil_true = FitUtils::GetErecoil_TRUE(event);
  Erecoil_charged = FitUtils::GetErecoil_CHARGED(event);
  Erecoil_minerva = FitUtils::GetErecoil_MINERvA_LowRecoil(event);

  // Do the angles between final state particles
  if (Nleptons > 0 && Npiplus > 0)
    CosPmuPpip = cos(pmu->Vect().Angle(ppip->Vect()));
  if (Nleptons > 0 && Npineg > 0)
    CosPmuPpim = cos(pmu->Vect().Angle(ppim->Vect()));
  if (Nleptons > 0 && Npi0 > 0) CosPmuPpi0 = cos(pmu->Vect().Angle(ppi0->Vect()));
  if (Nleptons > 0 && Nprotons > 0)
    CosPmuPprot = cos(pmu->Vect().Angle(pprot->Vect()));
  if (Nleptons > 0 && Nneutrons > 0)
    CosPmuPneut = cos(pmu->Vect().Angle(pneut->Vect()));

  if (Npiplus > 0 && Nprotons > 0)
    CosPpipPprot = cos(ppip->Vect().Angle(pprot->Vect()));
  if (Npiplus > 0 && Nneutrons > 0)
    CosPpipPneut = cos(ppip->Vect().Angle(pneut->Vect()));
  if (Npiplus > 0 && Npineg > 0)
    CosPpipPpim = cos(ppip->Vect().Angle(ppim->Vect()));
  if (Npiplus > 0 && Npi0 > 0)
    CosPpipPpi0 = cos(ppip->Vect().Angle(ppi0->Vect()));

  if (Npineg > 0 && Nprotons > 0)
    CosPpimPprot = cos(ppim->Vect().Angle(pprot->Vect()));
  if (Npineg > 0 && Nneutrons > 0)
    CosPpimPneut = cos(ppim->Vect().Angle(pneut->Vect()));
  if (Npineg > 0 && Npi0 > 0) CosPpimPpi0 = cos(ppim->Vect().Angle(ppi0->Vect()));

  if (Npi0 > 0 && Nprotons > 0)
    CosPi0Pprot = cos(ppi0->Vect().Angle(pprot->Vect()));
  if (Npi0 > 0 && Nneutrons > 0)
    CosPi0Pneut = cos(ppi0->Vect().Angle(pneut->Vect()));

  if (Nprotons > 0 && Nneutrons > 0)
    CosPprotPneut = cos(pprot->Vect().Angle(pneut->Vect()));

  // Event Weights ----
  // ------------------
  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;
  FluxWeight =
      fluxHist->GetBinContent(fluxHist->FindBin(Enu)) / fluxHist->Integral();

  // Fill the eventVariables Tree
  eventVariables->Fill();
  return;
};

//********************************************************************
void GenericFlux_Tester::Write(std::string drawOpt) {
  //********************************************************************

  // First save the TTree
  eventVariables->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  return;
}

//********************************************************************
void GenericFlux_Tester::FillSignalFlags(FitEvent *event) {
  //********************************************************************

  // Some example flags are given from SignalDef.
  // See src/Utils/SignalDef.cxx for more.
  flagCCQE_full = SignalDef::isCCQE(event, EnuMin, EnuMax, false);
  flagCCQE_rest = SignalDef::isCCQE(event, EnuMin, EnuMax, true);
  flagCCQEBar_full = SignalDef::isCCQEBar(event, EnuMin, EnuMax, false);
  flagCCQEBar_rest = SignalDef::isCCQEBar(event, EnuMin, EnuMax, true);

  flagCC1pip_MiniBooNE = SignalDef::isCC1pip_MiniBooNE(event, EnuMin, EnuMax);
  flagCC1pip_MINERvA_full = SignalDef::isCC1pip_MINERvA(event, EnuMin, EnuMax);
  flagCC1pip_MINERvA_rest =
      SignalDef::isCC1pip_MINERvA(event, EnuMin, EnuMax, true);

  int dummy;
  flagCCNpip_MINERvA_full =
      SignalDef::isCCNpip_MINERvA(event, dummy, EnuMin, EnuMax);
  flagCCNpip_MINERvA_rest =
      SignalDef::isCCNpip_MINERvA(event, dummy, EnuMin, EnuMax, true);

  // Include Michel e sample so no phase space cuts on pion, only angle
  flagCC1pip_T2K_Michel =
      SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
  flagCC1pip_T2K = SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, false);

  flagCC1pi0_MiniBooNE = SignalDef::isCC1pi0_MiniBooNE(event, EnuMin, EnuMax);
  flagCC1pi0Bar_MINERvA = SignalDef::isCC1pi0Bar_MINERvA(event, EnuMin, EnuMax);
  flagNC1pi0_MiniBooNE = SignalDef::isNC1pi0_MiniBooNE(event, EnuMin, EnuMax);
  flagNC1pi0Bar_MiniBooNE =
      SignalDef::isNC1pi0Bar_MiniBooNE(event, EnuMin, EnuMax);

  flagCCcoh_MINERvA = SignalDef::isCCcoh_MINERvA(event, EnuMin, EnuMax);
  flagCCcohBar_MINERvA = SignalDef::isCCcohBar_MINERvA(event, EnuMin, EnuMax);

  flagCCQEnumu_MINERvA_full =
      SignalDef::isCCQEnumu_MINERvA(event, 1.5, 10.0, true);
  flagCCQEnumubar_MINERvA_full =
      SignalDef::isCCQEnumubar_MINERvA(event, 1.5, 10.0, true);
  flagCCQEnumu_MINERvA_rest =
      SignalDef::isCCQEnumu_MINERvA(event, 1.5, 10.0, false);
  flagCCQEnumubar_MINERvA_rest =
      SignalDef::isCCQEnumubar_MINERvA(event, 1.5, 10.0, false);

  flagCCincLowRecoil_MINERvA =
      SignalDef::isCCincLowRecoil_MINERvA(event, 2.0, 6.0, false);
  flagCCincLowRecoil_MINERvA_reqhad =
      SignalDef::isCCincLowRecoil_MINERvA(event, 2.0, 6.0, true);

  flagCCQELike_MiniBooNE = SignalDef::isMiniBooNE_CCQELike(event, 0.0, 3.0);
  flagCCQE_MiniBooNE = SignalDef::isMiniBooNE_CCQE(event, 0.0, 3.0);
  flagCCQEBar_MiniBooNE = SignalDef::isMiniBooNE_CCQEBar(event, 0.0, 3.0);
}

// -------------------------------------------------------------------
// Purely MC Plot
// Following functions are just overrides to handle this
// -------------------------------------------------------------------
//********************************************************************
/// Everything is classed as signal...
bool GenericFlux_Tester::isSignal(FitEvent *event) {
  //********************************************************************
  (void)event;
  return true;
};

//********************************************************************
void GenericFlux_Tester::ScaleEvents() {
  //********************************************************************
  // Saving everything to a TTree so no scaling required
  return;
}

//********************************************************************
void GenericFlux_Tester::ApplyNormScale(float norm) {
  //********************************************************************

  // Saving everything to a TTree so no scaling required
  this->currentNorm = norm;
  return;
}

//********************************************************************
void GenericFlux_Tester::FillHistograms() {
  //********************************************************************
  // No Histograms need filling........
  return;
}

//********************************************************************
void GenericFlux_Tester::ResetAll() {
  //********************************************************************
  eventVariables->Reset();
  return;
}

//********************************************************************
float GenericFlux_Tester::GetChi2() {
  //********************************************************************
  // No Likelihood to test, purely MC
  return 0.0;
}
