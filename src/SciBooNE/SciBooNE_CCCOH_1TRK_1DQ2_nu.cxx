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

#include "SciBooNE_CCCOH_1TRK_1DQ2_nu.h"

SciBooNE_CCCOH_1TRK_1DQ2_nu::SciBooNE_CCCOH_1TRK_1DQ2_nu(std::string name, std::string inputfile,
							 FitWeight *rw, std::string type, 
							 std::string fakeDataFile){

  // Measurement Details
  fName = name;
  fDefaultTypes = "DIAG";
  fAllowedTypes += "EVT";
  HM_track = NULL;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  // fIsRawEvents = true;

  // Setup Plots
  this->fPlotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  this->SetDataValues(FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig10a_CVs.csv");
  this->GetTH2DFromFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");
  this->SetupDefaultHist();

  PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_PDG);

  // Estimate the number of CH molecules in SciBooNE...
  double nTargets = 10.6E6/13.*6.022E23;
  // This is sort of meaningless right now
  this->fScaleFactor = GetEventHistogram()->Integral()*1E-38*13./double(fNEvents)*nTargets;

  std::cout << "Quick test: evthist->Integral() = " << GetEventHistogram()->Integral() << "; evthist->Integral(width) = " << GetEventHistogram()->Integral("width") << std::endl;

};

void SciBooNE_CCCOH_1TRK_1DQ2_nu::FillEventVariables(FitEvent *event){

  //if (this->HM_track == NULL) return;
  if (event->NumFSParticle(PhysConst::pdg_muons) == 0) return;

  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;

  q2qe = FitUtils::Q2QErec(muon->fP,cos(Pnu.Vect().Angle(muon->fP.Vect())), 27., true);

  if (q2qe < 0) return;

  // Need to figure out if this is the best place to set weights long term... but...
  this->Weight *= this->CalcEfficiency(event->GetNeutrinoIn(), muon);
  
  // Set X Variables
  fXVar = q2qe;
  return;
};


bool SciBooNE_CCCOH_1TRK_1DQ2_nu::isSignal(FitEvent *event){

  int nCharged = 0;
  this->HM_track = NULL;

  // For now, choose the muon
  if (event->NumFSParticle(PhysConst::pdg_muons) == 0)
    return false;

  TLorentzVector Pmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  // For one track, require a single FS particle.
  for (UInt_t j = 0; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;

    // Look for pions, muons, protons
    if (abs(PID) == 211 || abs(PID) == 13 || PID == 2212){
      
      // Must be reconstructed as a track in SciBooNE
      if (! SciBooNEUtils::PassesCOHDistanceCut(event->PartInfo(j))) continue;
      nCharged += 1;
      if (!this->HM_track)
	this->HM_track = event->PartInfo(j);
      else if (event->PartInfo(j)->fP.Vect().Mag2() > this->HM_track->fP.Vect().Mag2())
	this->HM_track = event->PartInfo(j);
    }
  } // end loop over particle stack

  // This is the 1 track sample
  if (nCharged == 1) return true;

  return false;

};

// This function reads in the efficiency matrix from a root file
void SciBooNE_CCCOH_1TRK_1DQ2_nu::GetTH2DFromFile(std::string fileName, std::string histName){
  
  TFile *inFile = new TFile(fileName.c_str());
  this->muonStopEff = (TH2D*)(inFile->Get(histName.c_str()))->Clone();  
  this->muonStopEff->SetDirectory(0);
  inFile->Close();
  return;
}

// Expects to have a histogram in terms of p-theta which gives the efficiency, 
// and the relevant FitParticle to calculate a weight for
double SciBooNE_CCCOH_1TRK_1DQ2_nu::CalcEfficiency(FitParticle *nu, FitParticle *HMT){

  double eff = 1.;
  
  if (this->muonStopEff){
    double pmu = HMT->fP.Vect().Mag()/1000.;
    double thetamu = nu->fP.Vect().Angle(HMT->fP.Vect());
    eff = this->muonStopEff->GetBinContent(this->muonStopEff->FindBin(pmu, thetamu));
  }

  return eff;
}
