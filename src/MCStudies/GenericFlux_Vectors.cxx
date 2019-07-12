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

#include "GenericFlux_Vectors.h"

GenericFlux_Vectors::GenericFlux_Vectors(std::string name,
                                         std::string inputfile, FitWeight *rw,
                                         std::string type,
                                         std::string fakeDataFile) {
  // Measurement Details
  fName = name;
  eventVariables = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 1E10;  // Arbritrarily high energy limit

  if (Config::HasPar("EnuMin")) {
    EnuMin = Config::GetParD("EnuMin");
  }

  if (Config::HasPar("EnuMax")) {
    EnuMax = Config::GetParD("EnuMax");
  }

  SavePreFSI = Config::Get().GetParB("nuisflat_SavePreFSI");
  LOG(SAM) << "Running GenericFlux_Vectors saving pre-FSI particles? " << SavePreFSI << std::endl;

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
      (this->PredictedEventRate("width", 0, EnuMax) / double(fNEvents)) /
      this->TotalIntegratedFlux("width");

  LOG(SAM) << " Generic Flux Scaling Factor = " << fScaleFactor
           << " [= " << (GetEventHistogram()->Integral("width") * 1E-38) << "/("
           << (fNEvents + 0.) << "*" << TotalIntegratedFlux("width") << ")]"
           << std::endl;

  if (fScaleFactor <= 0.0) {
    ERR(WRN) << "SCALE FACTOR TOO LOW " << std::endl;
    throw;
  }

  // Setup our TTrees
  this->AddEventVariablesToTree();
  this->AddSignalFlagsToTree();
}

void GenericFlux_Vectors::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  LOG(SAM) << "Adding Event Variables" << std::endl;

  eventVariables->Branch("Mode", &Mode, "Mode/I");
  eventVariables->Branch("cc", &cc, "cc/B");
  eventVariables->Branch("PDGnu", &PDGnu, "PDGnu/I");
  eventVariables->Branch("Enu_true", &Enu_true, "Enu_true/F");
  eventVariables->Branch("tgt", &tgt, "tgt/I");
  eventVariables->Branch("tgta", &tgta, "tgta/I");
  eventVariables->Branch("tgtz", &tgtz, "tgtz/I");
  eventVariables->Branch("PDGLep", &PDGLep, "PDGLep/I");
  eventVariables->Branch("ELep", &ELep, "ELep/F");
  eventVariables->Branch("CosLep", &CosLep, "CosLep/F");

  // Basic interaction kinematics
  eventVariables->Branch("Q2", &Q2, "Q2/F");
  eventVariables->Branch("q0", &q0, "q0/F");
  eventVariables->Branch("q3", &q3, "q3/F");
  eventVariables->Branch("Enu_QE", &Enu_QE, "Enu_QE/F");
  eventVariables->Branch("Q2_QE", &Q2_QE, "Q2_QE/F");
  eventVariables->Branch("W_nuc_rest", &W_nuc_rest, "W_nuc_rest/F");
  eventVariables->Branch("W", &W, "W/F");
  eventVariables->Branch("W_genie", &W_genie, "W_genie/F");
  eventVariables->Branch("x", &x, "x/F");
  eventVariables->Branch("y", &y, "y/F");
  eventVariables->Branch("Eav", &Eav, "Eav/F");
  eventVariables->Branch("EavAlt", &EavAlt, "EavAlt/F");

  eventVariables->Branch("dalphat", &dalphat, "dalphat/F");
  eventVariables->Branch("dpt", &dpt, "dpt/F");
  eventVariables->Branch("dphit", &dphit, "dphit/F");
  eventVariables->Branch("pnreco_C", &pnreco_C, "pnreco_C/F");

  // Save outgoing particle vectors
  eventVariables->Branch("nfsp", &nfsp, "nfsp/I");
  eventVariables->Branch("px", px, "px[nfsp]/F");
  eventVariables->Branch("py", py, "py[nfsp]/F");
  eventVariables->Branch("pz", pz, "pz[nfsp]/F");
  eventVariables->Branch("E", E, "E[nfsp]/F");
  eventVariables->Branch("pdg", pdg, "pdg[nfsp]/I");
  eventVariables->Branch("pdg_rank", pdg_rank, "pdg_rank[nfsp]/I");
  eventVariables->Branch("status", status, "status[nfsp]/I");
  eventVariables->Branch("isalive", isalive, "isalive[nfsp]/O");
  eventVariables->Branch("isprimary", isprimary, "isprimary[nfsp]/O");

  // Event Scaling Information
  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
  // Should be a double because may be 1E-39 and less
  eventVariables->Branch("fScaleFactor", &fScaleFactor, "fScaleFactor/D");

  // The customs
  eventVariables->Branch("CustomWeight", &CustomWeight, "CustomWeight/F");
  eventVariables->Branch("CustomWeightArray", CustomWeightArray, "CustomWeightArray[6]/F");

  return;
}

void GenericFlux_Vectors::FillEventVariables(FitEvent *event) {

  ResetVariables();

  // Fill Signal Variables
  FillSignalFlags(event);
  LOG(DEB) << "Filling signal" << std::endl;

  // Now fill the information
  Mode = event->Mode;
  cc = (abs(event->Mode) < 30);

  // Get the incoming neutrino and outgoing lepton
  FitParticle *nu = event->GetNeutrinoIn();
  FitParticle *lep = event->GetHMFSAnyLepton();

  PDGnu = nu->fPID;
  Enu_true = nu->fP.E() / 1E3;
  tgt = event->fTargetPDG;
  tgta = event->fTargetA;
  tgtz = event->fTargetZ;

  if (lep != NULL) {
    PDGLep = lep->fPID;
    ELep = lep->fP.E() / 1E3;
    CosLep = cos(nu->fP.Vect().Angle(lep->fP.Vect()));

    // Basic interaction kinematics
    Q2 = -1 * (nu->fP - lep->fP).Mag2() / 1E6;
    q0 = (nu->fP - lep->fP).E() / 1E3;
    q3 = (nu->fP - lep->fP).Vect().Mag() / 1E3;

    // These assume C12 binding from MINERvA... not ideal
    Enu_QE = FitUtils::EnuQErec(lep->fP, CosLep, 34., true);
    Q2_QE = FitUtils::Q2QErec(lep->fP, CosLep, 34., true);

    Eav = FitUtils::GetErecoil_MINERvA_LowRecoil(event)/1.E3;
    EavAlt = FitUtils::Eavailable(event)/1.E3;

    // Get W_true with assumption of initial state nucleon at rest
    float m_n = (float)PhysConst::mass_proton;
    // Q2 assuming nucleon at rest
    W_nuc_rest = sqrt(-Q2 + 2 * m_n * q0 + m_n * m_n);
    // True Q2
    W = sqrt(-Q2 + 2 * m_n * q0 + m_n * m_n);
    x = Q2 / (2 * m_n * q0);
    y = 1 - ELep / Enu_true;

    dalphat = FitUtils::Get_STV_dalphat(event, PDGnu, true);
    dpt = FitUtils::Get_STV_dpt(event, PDGnu, true);
    dphit = FitUtils::Get_STV_dphit(event, PDGnu, true);
    pnreco_C = FitUtils::Get_pn_reco_C(event, PDGnu, true);
  }

  // Loop over the particles and store all the final state particles in a vector
  for (UInt_t i = 0; i < event->Npart(); ++i) {

    bool part_alive = event->PartInfo(i)->fIsAlive &&
      event->PartInfo(i)->Status() == kFinalState;
    if (!SavePreFSI) {
      if (!part_alive) continue;
    }

    partList.push_back(event->PartInfo(i));
  }

  // Save outgoing particle vectors
  nfsp = (int)partList.size();
  std::map<int, std::vector<std::pair<double, int> > > pdgMap;

  for (int i = 0; i < nfsp; ++i) {
    px[i] = partList[i]->fP.X() / 1E3;
    py[i] = partList[i]->fP.Y() / 1E3;
    pz[i] = partList[i]->fP.Z() / 1E3;
    E[i] = partList[i]->fP.E() / 1E3;
    pdg[i] = partList[i]->fPID;
    status[i] = partList[i]->Status();
    isalive[i] = partList[i]->fIsAlive;
    isprimary[i] = event->fPrimaryVertex[i];
    pdgMap[pdg[i]].push_back(std::make_pair(partList[i]->fP.Vect().Mag(), i));
  }  

  for(std::map<int, std::vector<std::pair<double, int> > >::iterator iter = pdgMap.begin(); 
      iter != pdgMap.end(); ++iter){
    std::vector<std::pair<double, int> > thisVect = iter->second;
    std::sort(thisVect.begin(), thisVect.end());

    // Now save the order... a bit funky to avoid inverting
    int nPart = (int)thisVect.size()-1;
    for (int i=nPart; i >= 0; --i) {
      pdg_rank[thisVect[i].second] = nPart-i;
    }
  }

#ifdef __GENIE_ENABLED__
  if (event->fType == kGENIE) {
    EventRecord *  gevent      = static_cast<EventRecord*>(event->genie_event->event);
    const Interaction * interaction = gevent->Summary();
    const Kinematics &   kine       = interaction->Kine();
    double W_genie  = kine.W();
  }
#endif

  // Fill event weights
  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;
  // And the Customs
  CustomWeight = event->CustomWeight;
  for (int i = 0; i < 6; ++i) {
    CustomWeightArray[i] = event->CustomWeightArray[i];
  }

  // Fill the eventVariables Tree
  eventVariables->Fill();
  return;
};

//********************************************************************
void GenericFlux_Vectors::ResetVariables() {
  //********************************************************************

  cc = false;

  // Reset all Function used to extract any variables of interest to the event
  Mode = PDGnu = tgt = tgta = tgtz = PDGLep = 0;

  Enu_true = ELep = CosLep = Q2 = q0 = q3 = Enu_QE = Q2_QE = W_nuc_rest = W = x = y = Eav = EavAlt = -999.9;

  W_genie = -999;
  // Other fun variables
  // MINERvA-like ones
  dalphat = dpt = dphit = pnreco_C = -999.99;

  nfsp = 0;
  for (int i = 0; i < kMAX; ++i){
    px[i] = py[i] = pz[i] = E[i] = -999;
<<<<<<< HEAD
    pdg[i] = pdg_rank[i] = 0;
=======
    pdg[i] = 0;
    status[i] = -999;
    isalive[i] = false;
    isprimary[i] = false;
>>>>>>> 8cb8d610b53c4930caea73bde1186b7dbb04981c
  }

  Weight = InputWeight = RWWeight = 0.0;

  CustomWeight = 0.0;
  for (int i = 0; i < 6; ++i) CustomWeightArray[i] = 0.0;

  partList.clear();

  flagCCINC = flagNCINC = flagCCQE = flagCC0pi = flagCCQELike = flagNCEL = flagNC0pi = flagCCcoh = flagNCcoh = flagCC1pip = flagNC1pip = flagCC1pim = flagNC1pim = flagCC1pi0 = flagNC1pi0 = false;
}

//********************************************************************
void GenericFlux_Vectors::FillSignalFlags(FitEvent *event) {
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

void GenericFlux_Vectors::AddSignalFlagsToTree() {
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
        (this->fName + "_VARS").c_str());
  }

  LOG(SAM) << "Adding signal flags" << std::endl;

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

void GenericFlux_Vectors::Write(std::string drawOpt) {

  // First save the TTree
  eventVariables->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  return;
}

// Override functions which aren't really necessary
bool GenericFlux_Vectors::isSignal(FitEvent *event) {
  (void)event;
  return true;
};

void GenericFlux_Vectors::ScaleEvents() { return; }

void GenericFlux_Vectors::ApplyNormScale(float norm) {
  this->fCurrentNorm = norm;
  return;
}

void GenericFlux_Vectors::FillHistograms() { return; }

void GenericFlux_Vectors::ResetAll() {
  eventVariables->Reset();
  return;
}

float GenericFlux_Vectors::GetChi2() { return 0.0; }
