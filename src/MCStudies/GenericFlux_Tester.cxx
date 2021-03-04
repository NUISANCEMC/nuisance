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

#include "GenericFlux_Tester.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
GenericFlux_Tester::GenericFlux_Tester(std::string name, std::string inputfile,
                                       FitWeight *rw, std::string type,
                                       std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  fName = name;
  eventVariables = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 1E10; // Arbritrarily high energy limit

  // Set default fitter flags
  fIsDiag = true;
  fIsShape = false;
  fIsRawEvents = false;

  nu_4mom = new TLorentzVector(0, 0, 0, 0);
  pmu = new TLorentzVector(0, 0, 0, 0);
  ppip = new TLorentzVector(0, 0, 0, 0);
  ppim = new TLorentzVector(0, 0, 0, 0);
  ppi0 = new TLorentzVector(0, 0, 0, 0);
  pprot = new TLorentzVector(0, 0, 0, 0);
  pprot_sec = new TLorentzVector(0, 0, 0, 0);
  pneut = new TLorentzVector(0, 0, 0, 0);
  prem = new TLorentzVector(0, 0, 0, 0);
  ptgtNuc = new TLorentzVector(0, 0, 0, 0);
  infk_pprot = new TLorentzVector(0, 0, 0, 0);
  pkp = new TLorentzVector(0, 0, 0, 0);
  pkm = new TLorentzVector(0, 0, 0, 0);

  ppip_vect = new TLorentzVector(0, 0, 0, 0);
  ppim_vect = new TLorentzVector(0, 0, 0, 0);
  ppi0_vect = new TLorentzVector(0, 0, 0, 0);
  pprot_vect = new TLorentzVector(0, 0, 0, 0);
  pprot_sec_vect = new TLorentzVector(0, 0, 0, 0);
  pneut_vect = new TLorentzVector(0, 0, 0, 0);
  prem_vect = new TLorentzVector(0, 0, 0, 0);
  ptgtNuc_vect = new TLorentzVector(0, 0, 0, 0);
  infk_pprot_vect = new TLorentzVector(0, 0, 0, 0);
  pkp_vect = new TLorentzVector(0, 0, 0, 0);
  pkm_vect = new TLorentzVector(0, 0, 0, 0);

  // This function will sort out the input files automatically and parse all the
  // inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually
  // this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  eventVariables = NULL;
  liteMode = Config::Get().GetParB("isLiteMode");
  minervaSTVPS = FitPar::Config().GetParB("isMinervaSTVPS");

  NUIS_LOG(SAM, " Generic Flux Config. liteMode=" << liteMode << " minervaSTVPS= " << minervaSTVPS);


  if (Config::HasPar("EnuMin")) {
    EnuMin = Config::GetParD("EnuMin");
  }

  if (Config::HasPar("EnuMax")) {
    EnuMax = Config::GetParD("EnuMax");
  }

  // Setup fDataHist as a placeholder
  this->fDataHist = new TH1D(("empty_data"), ("empty-data"), 1, 0, 1);
  this->SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  // N.B. MeasurementBase::PredictedEventRate includes the 1E-38 factor that is
  // often included here in other classes that directly integrate the event
  // histogram. This method is used here as it now respects EnuMin and EnuMax
  // correctly.
  this->fScaleFactor =
      (this->PredictedEventRate("width", 0, 1000) / double(fNEvents)) /
      this->TotalIntegratedFlux();
  if (fScaleFactor <= 0.0) {
    NUIS_ABORT("SCALE FACTOR TOO LOW ");
  }

  NUIS_LOG(SAM, " Generic Flux Scaling Factor = "
                << fScaleFactor
                << " [= " << (GetEventHistogram()->Integral("width") * 1E-38)
                << "/(" << (fNEvents + 0.) << "*" << this->TotalIntegratedFlux()
                << ")]");

  // Setup our TTrees
  this->AddEventVariablesToTree();
  this->AddSignalFlagsToTree();
}

void GenericFlux_Tester::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  NUIS_LOG(SAM, "Adding Event Variables");
  eventVariables->Branch("Mode", &Mode, "Mode/I");

  eventVariables->Branch("TgtA", &TgtA, "TgtA/I");
  eventVariables->Branch("TgtZ", &TgtZ, "TgtZ/I");

  eventVariables->Branch("PDGnu", &PDGnu, "PDGnu/I");
  eventVariables->Branch("Enu_true", &Enu_true, "Enu_true/F");

  eventVariables->Branch("Nleptons", &Nleptons, "Nleptons/I");
  // all sensible
  eventVariables->Branch("MLep", &MLep, "MLep/F");
  eventVariables->Branch("ELep", &ELep, "ELep/F");
  // negative -999
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


  eventVariables->Branch("Nkplus", &Nkplus, "Nkplus/I");
  eventVariables->Branch("Nkneg", &Nkneg, "Nkneg/I");
  eventVariables->Branch("Nother", &Nother, "Nother/I");

  eventVariables->Branch("Q2_true", &Q2_true, "Q2_true/F");
  eventVariables->Branch("q0_true", &q0_true, "q0_true/F");
  eventVariables->Branch("q3_true", &q3_true, "q3_true/F");

  eventVariables->Branch("stv_dpt", &stv_dpt, "stv_dpt/F");
  eventVariables->Branch("stv_dphit", &stv_dphit, "stv_dphit/F");
  eventVariables->Branch("stv_dat", &stv_dat, "stv_dat/F");
  eventVariables->Branch("pn_rec", &pn_rec, "pn_rec/F");
  eventVariables->Branch("infkModImbal", &infkModImbal, "infkModImbal/F");



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

  eventVariables->Branch("Nleptons_vect", &Nleptons_vect, "Nleptons_vect/I");
  eventVariables->Branch("Nprotons_vect", &Nprotons_vect, "Nprotons_vect/I");
  eventVariables->Branch("Nneutrons_vect", &Nneutrons_vect, "Nneutrons_vect/I");
  eventVariables->Branch("Npiplus_vect", &Npiplus_vect, "Npiplus_vect/I");
  eventVariables->Branch("Npineg_vect", &Npineg_vect, "Npineg_vect/I");
  eventVariables->Branch("Npi0_vect", &Npi0_vect, "Npi0_vect/I");
  eventVariables->Branch("Nkplus_vect", &Nkplus_vect, "Nkplus_vect/I");
  eventVariables->Branch("Nkneg_vect", &Nkneg_vect, "Nkneg_vect/I");
  eventVariables->Branch("Nother_vect", &Nother_vect, "Nother_vect/I");

  if (!liteMode) {
    eventVariables->Branch("nu_4mom", &nu_4mom);
    eventVariables->Branch("pmu_4mom", &pmu);
    eventVariables->Branch("hm_ppip_4mom", &ppip);
    eventVariables->Branch("hm_ppim_4mom", &ppim);
    eventVariables->Branch("hm_ppi0_4mom", &ppi0);
    eventVariables->Branch("hm_pprot_4mom", &pprot);
    eventVariables->Branch("shm_pprot_4mom", &pprot_sec);
    eventVariables->Branch("hm_pneut_4mom", &pneut);
    eventVariables->Branch("prem_4mom", &prem);
    eventVariables->Branch("hm_ISQE_nucl_4mom", &ptgtNuc);
    eventVariables->Branch("infk_pprot_4mom", &infk_pprot);
    eventVariables->Branch("hm_pkp_4mom", &ppip);
    eventVariables->Branch("hm_pkm_4mom", &ppim);

    eventVariables->Branch("hm_ppip_4mom_vect", &ppip_vect);
    eventVariables->Branch("hm_ppim_4mom_vect", &ppim_vect);
    eventVariables->Branch("hm_ppi0_4mom_vect", &ppi0_vect);
    eventVariables->Branch("hm_pprot_4mom_vect", &pprot_vect);
    eventVariables->Branch("shm_pprot_4mom_vect", &pprot_sec_vect);
    eventVariables->Branch("hm_pneut_4mom_vect", &pneut_vect);
    eventVariables->Branch("prem_4mom_vect", &prem_vect);
    eventVariables->Branch("hm_ISQE_nucl_4mom_vect", &ptgtNuc_vect);
    eventVariables->Branch("infk_pprot_4mom_vect", &infk_pprot_vect);
    eventVariables->Branch("hm_pkp_4mom_vect", &ppip_vect);
    eventVariables->Branch("hm_pkm_4mom_vect", &ppim_vect);
  }

  // Event Scaling Information
  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
  eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/F");
  eventVariables->Branch("fScaleFactor", &fScaleFactor, "fScaleFactor/D");

  return;
}

void GenericFlux_Tester::AddSignalFlagsToTree() {
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  NUIS_LOG(SAM, "Adding signal flags");

  // Signal Definitions from SignalDef.cxx
  eventVariables->Branch("flagCCINC", &flagCCINC, "flagCCINC/O");
  eventVariables->Branch("flagNCINC", &flagNCINC, "flagNCINC/O");
  eventVariables->Branch("flagCCQE", &flagCCQE, "flagCCQE/O");
  eventVariables->Branch("flagCC0pi", &flagCC0pi, "flagCC0pi/O");
  eventVariables->Branch("flagCCQELike", &flagCCQELike, "flagCCQELike/O");
  eventVariables->Branch("flagNCEL", &flagNCEL, "flagNCEL/O");
  eventVariables->Branch("flagNC0pi", &flagNC0pi, "flagNC0pi/O");
  eventVariables->Branch("flagCCcoh", &flagCCcoh, "flagCCcoh/O");
  eventVariables->Branch("flagNCcoh", &flagNCcoh, "flagNCcoh/O");
  eventVariables->Branch("flagCC1pip", &flagCC1pip, "flagCC1pip/O");
  eventVariables->Branch("flagNC1pip", &flagNC1pip, "flagNC1pip/O");
  eventVariables->Branch("flagCC1pim", &flagCC1pim, "flagCC1pim/O");
  eventVariables->Branch("flagNC1pim", &flagNC1pim, "flagNC1pim/O");
  eventVariables->Branch("flagCC1pi0", &flagCC1pi0, "flagCC1pi0/O");
  eventVariables->Branch("flagNC1pi0", &flagNC1pi0, "flagNC1pi0/O");
};

//********************************************************************
void GenericFlux_Tester::ResetVariables() {
  //********************************************************************
  // Reset neutrino PDG
  PDGnu = 0;
  // Reset energies
  Enu_true = Enu_QE = __BAD_FLOAT__;

  stv_dpt = stv_dphit = stv_dat = pn_rec = infkModImbal = __BAD_FLOAT__;


  // Reset auxillaries
  Q2_true = Q2_QE = W_nuc_rest = bjorken_x = bjorken_y = q0_true = q3_true =
      Erecoil_true = Erecoil_charged = Erecoil_minerva = __BAD_FLOAT__;

  // Reset particle counters
  Nparticles = Nleptons = Nother = Nprotons = Nneutrons = Npiplus = Npineg =
      Npi0 = Nkneg = Nkplus = 0;
  Nparticles_vect = Nleptons_vect = Nother_vect = Nprotons_vect =
      Nneutrons_vect = Npiplus_vect = Npineg_vect = Npi0_vect = Nkneg_vect = 
      Nkplus_vect = 0;

  // Reset Lepton PDG
  PDGLep = 0;
  // Reset Lepton variables
  TLep = CosLep = ELep = PLep = MLep = __BAD_FLOAT__;

  // Rset proton variables
  PPr = CosPr = EPr = TPr = MPr = __BAD_FLOAT__;

  // Reset neutron variables
  PNe = CosNe = ENe = TNe = MNe = __BAD_FLOAT__;

  // Reset pi+ variables
  PPiP = CosPiP = EPiP = TPiP = MPiP = __BAD_FLOAT__;

  // Reset pi- variables
  PPiN = CosPiN = EPiN = TPiN = MPiN = __BAD_FLOAT__;

  // Reset pi0 variables
  PPi0 = CosPi0 = EPi0 = TPi0 = MPi0 = __BAD_FLOAT__;

  // Reset the cos angles
  CosPmuPpip = CosPmuPpim = CosPmuPpi0 = CosPmuPprot = CosPmuPneut =
      CosPpipPprot = CosPpipPneut = CosPpipPpim = CosPpipPpi0 = CosPpimPprot =
          CosPpimPneut = CosPpimPpi0 = CosPi0Pprot = CosPi0Pneut =
              CosPprotPneut = __BAD_FLOAT__;
}

//********************************************************************
void GenericFlux_Tester::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Fill Signal Variables
  FillSignalFlags(event);
  NUIS_LOG(DEB, "Filling signal");

  // Reset the private variables (see header)
  ResetVariables();

  // Function used to extract any variables of interest to the event
  Mode = event->Mode;

  // Get Tgt info: 
  TgtA =  event->GetTargetA();
  TgtZ =  event->GetTargetZ();

  // Reset the highest momentum variables
  float proton_highmom = __BAD_FLOAT__;
  float proton_secondhighmom = __BAD_FLOAT__;
  float neutron_highmom = __BAD_FLOAT__;
  float piplus_highmom = __BAD_FLOAT__;
  float pineg_highmom = __BAD_FLOAT__;
  float pi0_highmom = __BAD_FLOAT__;
  float kplus_highmom = __BAD_FLOAT__;
  float kneg_highmom = __BAD_FLOAT__;

  (*nu_4mom) = event->PartInfo(0)->fP;

  if (!liteMode) {
    (*pmu) = TLorentzVector(0, 0, 0, 0);
    (*ppip) = TLorentzVector(0, 0, 0, 0);
    (*ppim) = TLorentzVector(0, 0, 0, 0);
    (*ppi0) = TLorentzVector(0, 0, 0, 0);
    (*pprot) = TLorentzVector(0, 0, 0, 0);
    (*pprot_sec) = TLorentzVector(0, 0, 0, 0);
    (*pneut) = TLorentzVector(0, 0, 0, 0);
    (*prem) = TLorentzVector(0, 0, 0, 0);
    (*ptgtNuc) = TLorentzVector(0, 0, 0, 0);
    (*infk_pprot) = TLorentzVector(0, 0, 0, 0);
    (*pkp) = TLorentzVector(0, 0, 0, 0);
    (*pkm) = TLorentzVector(0, 0, 0, 0);

    (*ppip_vect) = TLorentzVector(0, 0, 0, 0);
    (*ppim_vect) = TLorentzVector(0, 0, 0, 0);
    (*ppi0_vect) = TLorentzVector(0, 0, 0, 0);
    (*pprot_vect) = TLorentzVector(0, 0, 0, 0);
    (*pprot_sec_vect) = TLorentzVector(0, 0, 0, 0);
    (*pneut_vect) = TLorentzVector(0, 0, 0, 0);
    (*prem_vect) = TLorentzVector(0, 0, 0, 0);
    (*ptgtNuc_vect) = TLorentzVector(0, 0, 0, 0);
    (*infk_pprot_vect) = TLorentzVector(0, 0, 0, 0);
    (*pkp_vect) = TLorentzVector(0, 0, 0, 0);
    (*pkm_vect) = TLorentzVector(0, 0, 0, 0);
  }

  Enu_true = nu_4mom->E();
  PDGnu = event->PartInfo(0)->fPID;

  bool cc = (abs(event->Mode) < 30);
  (void)cc;

  //Get initial state nucleon (for CCQE events)
  if(PDGnu>0 && event->GetHMISNeutron()){
    (*ptgtNuc) = event->GetHMISNeutron()->fP;
  }
  else if(event->GetHMISProton()){
    (*ptgtNuc) = event->GetHMISProton()->fP;
  }
  // Add all pion distributions for the event.

  // Add classifier for CC0pi or CC1pi or CCOther
  // Save Modes Properly
  // Save low recoil measurements

  // Start Particle Loop
  UInt_t npart = event->Npart();
  for (UInt_t i = 0; i < npart; i++) {
    // Skip particles that weren't in the final state
    bool part_alive = event->PartInfo(i)->fIsAlive and
                      event->PartInfo(i)->Status() == kFinalState;
    if (!part_alive)
      continue;

    // PDG Particle
    int PDGpart = event->PartInfo(i)->fPID;
    TLorentzVector part_4mom = event->PartInfo(i)->fP;

    Nparticles++;

    // Get Charged Lepton for EM/CC and get the outgoing neutrino for NC
    // (Sorry, I know it's dumb the filled variabels are still called "mu" sometimes)
    if ( (abs(PDGpart) == abs(PDGnu) - 1) || (abs(PDGpart) == abs(PDGnu)) ){
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
      float m_n = (float)PhysConst::mass_proton * 1000.;
      W_nuc_rest = sqrt(-Q2_true + 2 * m_n * (Enu_true - ELep) + m_n * m_n);

      // Get the Bjorken x and y variables
      // Assume that E_had = Enu - Emu as in MINERvA
      bjorken_x = Q2_true / (2 * m_n * (Enu_true - ELep));
      bjorken_y = 1 - ELep / Enu_true;

      // STV and pn_rec - only really make sense for CC0pi

      if (minervaSTVPS){
        // Hard-code the cuts
        double ProtonMinCut = 450; // MeV
        double ProtonMaxCut = 1200; // MeV
        double ProtonThetaCut = 70; // degrees
        //restrict to MINERvA's STV anlaysis phase-space
        pn_rec = FitUtils::Get_pn_reco_C(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true);
        stv_dat = FitUtils::Get_STV_dalphat(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) * (180.0 / M_PI);
        stv_dpt = FitUtils::Get_STV_dpt(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) / 1000.0;
        stv_dphit = FitUtils::Get_STV_dphit(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) * (180.0 / M_PI);
      }
      else{
        pn_rec = FitUtils::Get_pn_reco_C(event, 0.0, 999999.9, -1.0, 14, true);
        stv_dat = FitUtils::Get_STV_dalphat(event, 0.0, 999999.9, -1.0, 14, true) * (180.0 / M_PI);
        stv_dpt = FitUtils::Get_STV_dpt(event, 0.0, 999999.9, -1.0, 14, true) / 1000.0;
        stv_dphit = FitUtils::Get_STV_dphit(event, 0.0, 999999.9, -1.0, 14, true) * (180.0 / M_PI);
      }


      TVector3 infk_pprot_3 = FitUtils::tppInfK(part_4mom, cos(ThetaLep), 25., true);
      double infk_Eprot = sqrt(infk_pprot_3.Mag()*infk_pprot_3.Mag() + PhysConst::mass_proton*PhysConst::mass_proton);
      TLorentzVector* infk_pprot_4 = new TLorentzVector(infk_pprot_3, infk_Eprot);
      *infk_pprot = *infk_pprot_4;
      if( (event->GetHMFSProton()) ) infkModImbal = ( infk_pprot_3*1000.0 - (event->GetHMFSProton())->P3() ).Mag();



      // Quasi-elastic ----------------------
      // ------------------------------------

      // Q2 QE Assuming Carbon Input. Should change this to be dynamic soon.
      Q2_QE =
          FitUtils::Q2QErec(part_4mom, cos(ThetaLep), 25., true) * 1000000.0;
      Enu_QE = FitUtils::EnuQErec(part_4mom, cos(ThetaLep), 25., true) * 1000.0;

      // Pion Production ----------------------
      // --------------------------------------

    } else if (PDGpart == 2212) {
      Nprotons++;
      if (Nprotons>1 && part_4mom.Vect().Mag() > proton_highmom) {
        (*pprot_sec) = (*pprot);
        proton_secondhighmom =(*pprot_sec).Vect().Mag();
      }
      else if (Nprotons>1 && part_4mom.Vect().Mag() < proton_highmom && part_4mom.Vect().Mag() > proton_secondhighmom){
        (*pprot_sec) = part_4mom;
        proton_secondhighmom =(*pprot_sec).Vect().Mag();
      }
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
    } else if (PDGpart == 321) {
      Nkplus++;
      if (part_4mom.Vect().Mag() > kplus_highmom) {
        kplus_highmom = part_4mom.Vect().Mag();
        (*pkp) = part_4mom;
      }
    } else if (PDGpart == -321) {
      Nkneg++;
      if (part_4mom.Vect().Mag() > kneg_highmom) {
        kneg_highmom = part_4mom.Vect().Mag();
        (*pkm) = part_4mom;
      }
    } else {
      Nother++;
    }
  } // end particle loop




  // Rest highmoms for next loop
  proton_secondhighmom=0;
  proton_highmom=0;
  neutron_highmom=0;
  piplus_highmom=0;
  pineg_highmom=0;
  pi0_highmom=0;
  kplus_highmom=0;
  kneg_highmom=0;

  // Start Particle Loop for PRE FSI particles
  for (UInt_t i = 0; i < npart; i++) {
    // Save particles that are just before the FSI cascade
    bool isPreFSI = event->fPrimaryVertex[i];
    if (!isPreFSI)
      continue;

    // PDG Particle
    int PDGpart = event->PartInfo(i)->fPID;
    TLorentzVector part_4mom = event->PartInfo(i)->fP;
    Nparticles_vect++;
    
    if (abs(PDGpart) == abs(PDGnu) - 1) {
      Nleptons_vect++;
    } else if (PDGpart == 2212) {
      Nprotons_vect++;
      if (Nprotons_vect>1 && part_4mom.Vect().Mag() > proton_highmom) {
        (*pprot_sec_vect) = (*pprot_vect);
        proton_secondhighmom =(*pprot_sec_vect).Vect().Mag();
      }
      else if (Nprotons_vect>1 && part_4mom.Vect().Mag() < proton_highmom && part_4mom.Vect().Mag() > proton_secondhighmom){
        (*pprot_sec_vect) = part_4mom;
        proton_secondhighmom =(*pprot_sec_vect).Vect().Mag();
      }
      if (part_4mom.Vect().Mag() > proton_highmom) {
        proton_highmom = part_4mom.Vect().Mag();
        (*pprot_vect) = part_4mom;
      }
    } else if (PDGpart == 2112) {
      Nneutrons_vect++;
      if (part_4mom.Vect().Mag() > neutron_highmom) {
        neutron_highmom = part_4mom.Vect().Mag();
        (*pneut_vect) = part_4mom;
      }
    } else if (PDGpart == 211) {
      Npiplus_vect++;
      if (part_4mom.Vect().Mag() > piplus_highmom) {
        piplus_highmom = part_4mom.Vect().Mag();
        (*ppip_vect) = part_4mom;
      }
    } else if (PDGpart == -211) {
      Npineg_vect++;
      if (part_4mom.Vect().Mag() > pineg_highmom) {
        pineg_highmom = part_4mom.Vect().Mag();
        (*ppim_vect) = part_4mom;
      }
    } else if (PDGpart == 111) {
      Npi0_vect++;
      if (part_4mom.Vect().Mag() > pi0_highmom) {
        pi0_highmom = part_4mom.Vect().Mag();
        (*ppi0_vect) = part_4mom;
      }
    } else if (PDGpart == 321) {
      Nkplus_vect++;
      if (part_4mom.Vect().Mag() > kplus_highmom) {
        kplus_highmom = part_4mom.Vect().Mag();
        (*pkp_vect) = part_4mom;
      }
    } else if (PDGpart == -321) {
      Nkneg_vect++;
      if (part_4mom.Vect().Mag() > kneg_highmom) {
        kneg_highmom = part_4mom.Vect().Mag();
        (*pkm_vect) = part_4mom;
      }
    } else {
      Nother_vect++;
    }
  } // end particle loop for PRE FSI particles



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
  if (Nleptons > 0 && Npi0 > 0)
    CosPmuPpi0 = cos(pmu->Vect().Angle(ppi0->Vect()));
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
  if (Npineg > 0 && Npi0 > 0)
    CosPpimPpi0 = cos(ppim->Vect().Angle(ppi0->Vect()));

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
      GetFluxHistogram()->GetBinContent(GetFluxHistogram()->FindBin(Enu)) /
      GetFluxHistogram()->Integral();

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
  int nuPDG = event->PartInfo(0)->fPID;

  // Generic signal flags
  flagCCINC = SignalDef::isCCINC(event, nuPDG);
  flagNCINC = SignalDef::isNCINC(event, nuPDG);
  flagCCQE = SignalDef::isCCQE(event, nuPDG);
  flagCCQELike = SignalDef::isCCQELike(event, nuPDG);
  flagCC0pi = SignalDef::isCC0pi(event, nuPDG);
  flagNCEL = SignalDef::isNCEL(event, nuPDG);
  flagNC0pi = SignalDef::isNC0pi(event, nuPDG);
  flagCCcoh = SignalDef::isCCCOH(event, nuPDG, 211);
  flagNCcoh = SignalDef::isNCCOH(event, nuPDG, 111);
  flagCC1pip = SignalDef::isCC1pi(event, nuPDG, 211);
  flagNC1pip = SignalDef::isNC1pi(event, nuPDG, 211);
  flagCC1pim = SignalDef::isCC1pi(event, nuPDG, -211);
  flagNC1pim = SignalDef::isNC1pi(event, nuPDG, -211);
  flagCC1pi0 = SignalDef::isCC1pi(event, nuPDG, 111);
  flagNC1pi0 = SignalDef::isNC1pi(event, nuPDG, 111);
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
  this->fCurrentNorm = norm;
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
