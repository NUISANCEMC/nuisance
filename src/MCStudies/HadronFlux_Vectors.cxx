// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "HadronFlux_Vectors.h"

HadronFlux_Vectors::HadronFlux_Vectors(std::string name,
				       std::string inputfile, FitWeight *rw,
				       std::string type,
				       std::string fakeDataFile) {
  // Measurement Details
  fName = name;
  eventVariables = NULL;

  SavePreFSI = Config::Get().GetParB("nuisflat_SavePreFSI");
  NUIS_LOG(SAM, "Running HadronFlux_Vectors saving pre-FSI particles? "
                    << SavePreFSI);

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

  // This needs some care...
  this->fScaleFactor =
      (this->PredictedEventRate("width", 0, EnuMax) / double(fNEvents)) /
      this->TotalIntegratedFlux("width");

  NUIS_LOG(SAM, "Generic Flux Scaling Factor = "
                    << fScaleFactor << " [= "
                    << (GetEventHistogram()->Integral("width") * 1E-38) << "/("
                    << (fNEvents + 0.) << "*" << TotalIntegratedFlux("width")
                    << ")]");

  if (fScaleFactor <= 0.0) {
    // NUIS_ABORT("SCALE FACTOR TOO LOW");
    std::cout << "SCALE FACTOR TOO LOW" << std::endl;
  }

  this->AddEventVariablesToTree();
}

void HadronFlux_Vectors::AddEventVariablesToTree() {
  // Setup the TTree to save everything
  if (!eventVariables) {
    Config::Get().out->cd();
    eventVariables = new TTree((this->fName + "_VARS").c_str(),
                               (this->fName + "_VARS").c_str());
  }

  NUIS_LOG(SAM, "Adding Event Variables");

  eventVariables->Branch("Mode", &Mode, "Mode/I");
  eventVariables->Branch("in_pdg", &in_pdg, "in_pdg/I");
  eventVariables->Branch("in_etrue", &in_etrue, "in_etrue/F");
  eventVariables->Branch("in_ektrue", &in_ektrue, "in_ektrue/F");
  eventVariables->Branch("in_ptrue", &in_ptrue, "in_ptrue/F");
  eventVariables->Branch("tgt", &tgt, "tgt/I");
  eventVariables->Branch("tgta", &tgta, "tgta/I");
  eventVariables->Branch("tgtz", &tgtz, "tgtz/I");

  // Save outgoing particle vectors
  eventVariables->Branch("nfsp", &nfsp, "nfsp/I");
  eventVariables->Branch("px", px, "px[nfsp]/F");
  eventVariables->Branch("py", py, "py[nfsp]/F");
  eventVariables->Branch("pz", pz, "pz[nfsp]/F");
  eventVariables->Branch("E", E, "E[nfsp]/F");
  eventVariables->Branch("pdg", pdg, "pdg[nfsp]/I");
  eventVariables->Branch("pdg_rank", pdg_rank, "pdg_rank[nfsp]/I");

  // Save init particle vectors
  eventVariables->Branch("ninitp", &ninitp, "ninitp/I");
  eventVariables->Branch("px_init", px_init, "px_init[ninitp]/F");
  eventVariables->Branch("py_init", py_init, "py_init[ninitp]/F");
  eventVariables->Branch("pz_init", pz_init, "pz_init[ninitp]/F");
  eventVariables->Branch("E_init", E_init, "E_init[ninitp]/F");
  eventVariables->Branch("pdg_init", pdg_init, "pdg_init[ninitp]/I");

  // Save pre-FSI vectors
  eventVariables->Branch("nvertp", &nvertp, "nvertp/I");
  eventVariables->Branch("px_vert", px_vert, "px_vert[nvertp]/F");
  eventVariables->Branch("py_vert", py_vert, "py_vert[nvertp]/F");
  eventVariables->Branch("pz_vert", pz_vert, "pz_vert[nvertp]/F");
  eventVariables->Branch("E_vert", E_vert, "E_vert[nvertp]/F");
  eventVariables->Branch("pdg_vert", pdg_vert, "pdg_vert[nvertp]/I");

  // Event Scaling Information
  eventVariables->Branch("Weight", &Weight, "Weight/F");
  eventVariables->Branch("InputWeight", &InputWeight, "InputWeight/F");
  eventVariables->Branch("RWWeight", &RWWeight, "RWWeight/F");
  // Should be a double because may be 1E-39 and less
  eventVariables->Branch("fScaleFactor", &fScaleFactor, "fScaleFactor/D");

  // The customs
  eventVariables->Branch("CustomWeight", &CustomWeight, "CustomWeight/F");

  return;
}

void HadronFlux_Vectors::FillEventVariables(FitEvent *event) {

  ResetVariables();

  // Fill Signal Variables
  NUIS_LOG(DEB, "Filling event tree");

  // Now fill the information
  Mode = event->Mode;

  // Get the incoming particle
  // TODO: check this works
  FitParticle *in_part = event->GetBeamPart();

  in_pdg = in_part->fPID;
  in_etrue = in_part->fP.E() / 1E3;
  in_ektrue = (in_part->fP.E() - in_part->fP.M()) / 1E3;
  in_ptrue = in_part->fP.Vect().Mag() / 1E3;
  tgt = event->fTargetPDG;
  tgta = event->fTargetA;
  tgtz = event->fTargetZ;

  // Loop over the particles and store all the final state particles in a vector
  for (UInt_t i = 0; i < event->Npart(); ++i) {

    if (event->PartInfo(i)->fIsAlive &&
        event->PartInfo(i)->Status() == kFinalState)
      partList.push_back(event->PartInfo(i));

    if (SavePreFSI && event->fPrimaryVertex[i])
      vertList.push_back(event->PartInfo(i));

    if (SavePreFSI && event->PartInfo(i)->IsInitialState())
      initList.push_back(event->PartInfo(i));
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
    pdgMap[pdg[i]].push_back(std::make_pair(partList[i]->fP.Vect().Mag(), i));
  }

  for (std::map<int, std::vector<std::pair<double, int> > >::iterator iter =
           pdgMap.begin();
       iter != pdgMap.end(); ++iter) {
    std::vector<std::pair<double, int> > thisVect = iter->second;
    std::sort(thisVect.begin(), thisVect.end());

    // Now save the order... a bit funky to avoid inverting
    int nPart = (int)thisVect.size() - 1;
    for (int i = nPart; i >= 0; --i) {
      pdg_rank[thisVect[i].second] = nPart - i;
    }
  }

  // Save pre-FSI particles
  nvertp = (int)vertList.size();
  for (int i = 0; i < nvertp; ++i) {
    px_vert[i] = vertList[i]->fP.X() / 1E3;
    py_vert[i] = vertList[i]->fP.Y() / 1E3;
    pz_vert[i] = vertList[i]->fP.Z() / 1E3;
    E_vert[i] = vertList[i]->fP.E() / 1E3;
    pdg_vert[i] = vertList[i]->fPID;
  }

  // Save init particles
  ninitp = (int)initList.size();
  for (int i = 0; i < ninitp; ++i) {
    px_init[i] = initList[i]->fP.X() / 1E3;
    py_init[i] = initList[i]->fP.Y() / 1E3;
    pz_init[i] = initList[i]->fP.Z() / 1E3;
    E_init[i] = initList[i]->fP.E() / 1E3;
    pdg_init[i] = initList[i]->fPID;
  }

  // Fill event weights
  Weight = event->RWWeight * event->InputWeight;
  RWWeight = event->RWWeight;
  InputWeight = event->InputWeight;
  // And the Customs
  CustomWeight = event->CustomWeight;

  // Fill the eventVariables Tree
  eventVariables->Fill();
  return;
};

void HadronFlux_Vectors::ResetVariables() {

  Mode = in_pdg = tgt = tgta = tgtz = 0;

  in_etrue = in_ektrue = in_ptrue = -999.9;

  nfsp = ninitp = nvertp = 0;
  for (int i = 0; i < kMAX; ++i) {
    px[i] = py[i] = pz[i] = E[i] = -999;
    pdg[i] = pdg_rank[i] = 0;

    px_init[i] = py_init[i] = pz_init[i] = E_init[i] = -999;
    pdg_init[i] = 0;

    px_vert[i] = py_vert[i] = pz_vert[i] = E_vert[i] = -999;
    pdg_vert[i] = 0;
  }

  Weight = InputWeight = RWWeight = 0.0;

  CustomWeight = 0.0;

  partList.clear();
  initList.clear();
  vertList.clear();

}

void HadronFlux_Vectors::Write(std::string drawOpt) {

  // First save the TTree
  eventVariables->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  return;
}



// Override functions which aren't really necessary
bool HadronFlux_Vectors::isSignal(FitEvent *event) {
  (void)event;
  return true;
};

void HadronFlux_Vectors::ScaleEvents() { return; }

void HadronFlux_Vectors::ApplyNormScale(float norm) {
  this->fCurrentNorm = norm;
  return;
}

void HadronFlux_Vectors::FillHistograms() { return; }

void HadronFlux_Vectors::ResetAll() {
  // eventVariables->Reset();
  return;
}

float HadronFlux_Vectors::GetChi2() { return 0.0; }
