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

GenericFlux_Vectors::GenericFlux_Vectors(std::string name, std::string inputfile,
                                       FitWeight *rw, std::string type,
                                       std::string fakeDataFile) {
  // Measurement Details
  fName = name;
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

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  fScaleFactor =
      (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) /
      this->TotalIntegratedFlux();

  LOG(SAM) << " Generic Flux Scaling Factor = " << fScaleFactor << std::endl;

  if (fScaleFactor <= 0.0) {
    ERR(WRN) << "SCALE FACTOR TOO LOW " << std::endl;
  }

  // Setup our TTrees
  this->AddEventVariablesToTree();
}

void GenericFlux_Vectors::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  LOG(SAM) << "Adding Event Variables" << std::endl;

  eventVariables->Branch("Mode",   &Mode,   "Mode/I"  );
  eventVariables->Branch("cc",     &cc,     "cc/B"    );
  eventVariables->Branch("PDGnu",  &PDGnu,  "PDGnu/I" );
  eventVariables->Branch("Enu",    &Enu,    "Enu/F" );
  eventVariables->Branch("tgt",    &tgt,    "tgt/I"   );
  eventVariables->Branch("PDGLep", &PDGLep, "PDGLep/I");
  eventVariables->Branch("ELep",   &ELep,   "ELep/F"  );
  eventVariables->Branch("CosLep", &CosLep, "CosLep/F");

  // Basic interaction kinematics
  eventVariables->Branch("Q2",     &Q2,     "Q2/F");
  eventVariables->Branch("q0",     &q0,     "q0/F");
  eventVariables->Branch("q3",     &q3,     "q3/F");
  eventVariables->Branch("Enu_QE", &Enu_QE, "Enu_QE/F");
  eventVariables->Branch("Q2_QE",  &Q2_QE,  "Q2_QE/F");
  eventVariables->Branch("W_nuc_rest", &W_nuc_rest, "W_nuc_rest/F");
  eventVariables->Branch("W", 	   &W, 	    "W/F");
  eventVariables->Branch("x", 	   &x, 	    "x/F");
  eventVariables->Branch("y", 	   &y, 	    "y/F");

  // Save outgoing particle vectors
  eventVariables->Branch("nfsp", &nfsp, "nfsp/I");
  eventVariables->Branch("px",   px,    "px[nfsp]/F");
  eventVariables->Branch("py",   py,    "py[nfsp]/F");
  eventVariables->Branch("pz",   pz,    "pz[nfsp]/F");
  eventVariables->Branch("E",    E,     "E[nfsp]/F");
  eventVariables->Branch("pdg",  pdg,   "pdg[nfsp]/I");

  // Event Scaling Information
  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
  eventVariables->Branch("fScaleFactor", &fScaleFactor, "fScaleFactor/F");

  return;
}


void GenericFlux_Vectors::FillEventVariables(FitEvent *event) {

  // Reset all Function used to extract any variables of interest to the event
  PDGnu = tgt = PDGLep = 0;

  Enu = ELep = CosLep = Q2 = q0 = q3 = Enu_QE = Q2_QE = W_nuc_rest = W = x = y =
      -999.9;

  nfsp = 0;
  for (int i = 0; i < kMAX; ++i){
    px[i] = py[i] = pz[i] = E[i] = -999;
    pdg[i] = 0;
  }

  Weight = InputWeight = RWWeight = 0;

  partList.clear();

  // Now fill the information
  Mode = event->Mode;
  cc = (abs(event->Mode) < 30);

  // Get the incoming neutrino and outgoing lepton
  FitParticle *nu  = event->GetNeutrinoIn();
  FitParticle *lep = event->GetHMFSAnyLepton();

  PDGnu = nu->fPID;
  Enu = nu->fP.E()/1E3;
  tgt = event->fTargetPDG;
  PDGLep = lep->fPID;
  ELep = lep->fP.E()/1E3;
  CosLep = cos(nu->fP.Vect().Angle(lep->fP.Vect()));

  // Basic interaction kinematics
  Q2 = -1*(nu->fP - lep->fP).Mag2()/1E6;
  q0 = (nu->fP - lep->fP).E()/1E3;
  q3 = (nu->fP - lep->fP).Vect().Mag()/1E3;

  // These assume C12 binding from MINERvA... not ideal
  Enu_QE = FitUtils::EnuQErec(lep->fP, CosLep, 34., true);
  Q2_QE  = FitUtils::Q2QErec(lep->fP, CosLep, 34., true);

  // Get W_true with assumption of initial state nucleon at rest
  float m_n = (float)PhysConst::mass_proton;
  W_nuc_rest = sqrt(-Q2 + 2 * m_n * q0 + m_n * m_n);
  W = sqrt(-Q2 + 2 * m_n * q0 + m_n * m_n);
  x = Q2/(2 * m_n * q0);
  y = 1 - ELep/Enu;

  // Loop over the particles and store all the final state particles in a vector
  for (UInt_t i = 0; i < event->Npart(); ++i) {

    bool part_alive = event->PartInfo(i)->fIsAlive and event->PartInfo(i)->Status() == kFinalState;
    if (!part_alive) continue;

    partList .push_back(event->PartInfo(i));
  }

  // Save outgoing particle vectors
  nfsp = (int)partList.size();

  for (int i = 0; i < nfsp; ++i){
    px[i]  = partList[i]->fP.X()/1E3;
    py[i]  = partList[i]->fP.Y()/1E3;
    pz[i]  = partList[i]->fP.Z()/1E3;
    E[i]   = partList[i]->fP.E()/1E3;
    pdg[i] = partList[i]->fPID;
  }

  // Fill event weights
  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;

  // Fill the eventVariables Tree
  eventVariables->Fill();
  return;
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

void GenericFlux_Vectors::ScaleEvents() {
  return;
}

void GenericFlux_Vectors::ApplyNormScale(float norm) {
  this->fCurrentNorm = norm;
  return;
}

void GenericFlux_Vectors::FillHistograms() {
  return;
}

void GenericFlux_Vectors::ResetAll() {
  eventVariables->Reset();
  return;
}

float GenericFlux_Vectors::GetChi2() {
  return 0.0;
}
