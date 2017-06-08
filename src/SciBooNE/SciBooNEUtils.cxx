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

#include "SciBooNEUtils.h"
#include "FitParameters.h"
#include "FitUtils.h"

namespace FitPar{
  double SciBarDensity    = FitPar::Config().GetParD("SciBarDensity");
  double SciBarRecoDist   = FitPar::Config().GetParD("SciBarRecoDist");
  double PenetratingMuonE = FitPar::Config().GetParD("PenetratingMuonEnergy");
  double NumRangeSteps    = FitPar::Config().GetParI("NumRangeSteps");
}

double SciBooNEUtils::StoppedEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon){

  double eff = 1.;

  if (!effHist) return eff;
  eff = effHist->GetBinContent(effHist->FindBin(FitUtils::p(muon), FitUtils::th(nu, muon)/TMath::Pi()*180.));

  return eff;
}

double SciBooNEUtils::PenetratedEfficiency(FitParticle *nu, FitParticle *muon){

  double eff = 1.;

  if (FitUtils::th(nu, muon)/TMath::Pi()*180. > 50) eff = 0.;
  if (FitUtils::p(muon) < 1.4) eff = 0.;

  return eff;
}

double SciBooNEUtils::BetheBlochCH(double E, double mass){

  double beta2 = 1 - mass*mass/E/E;
  double gamma = 1./sqrt(1-beta2);
  double mass_ratio = PhysConst::mass_electron*1000./mass;
  // Argh, have to remember to convert to MeV or you'll hate yourself!
  double I2 = 68.7e-6*68.7e-6;

  double w_max = 2*PhysConst::mass_electron*1000.*beta2*gamma*gamma;
  w_max /= 1 + 2*gamma*mass_ratio + mass_ratio*mass_ratio;


  // Values taken from the PDG for K = 0.307075 MeV mol-1 cm2, mean ionization energy I = 68.7 eV (Polystyrene)
  // <Z/A> = 0.53768 (pdg.lbl.gov/AtomicNuclearProperties)
  double log_term = log(2*PhysConst::mass_electron*1000.*beta2*gamma*gamma*w_max/I2);
  double dedx = 0.307075*0.53768/beta2*(0.5*log_term - beta2);

  return dedx;
}


// This function returns an estimate of the range of the particle in scintillator.
// It uses crude integration and Bethe-Blocke to approximate the range.
double SciBooNEUtils::RangeInScintillator(FitParticle* particle, int nsteps){

  // The particle energy
  double E  = particle->fP.E();
  double M  = particle->fP.M();
  double Ek = E - M;

  double step_size = Ek/float(nsteps+1);
  double range = 0;

  // Add an offset to make the integral a touch more accurate
  Ek -= step_size/2.;
  for (int i = 0; i < nsteps; ++i){
    
    double dEdx = SciBooNEUtils::BetheBlochCH(Ek+M, M);
    
    Ek -= step_size;
    // dEdx is -ve
    range -= step_size/dEdx;
  }

  // Account for density of polystyrene
  range /= FitPar::SciBarDensity;

  // Range estimate is in cm
  return range;
}


// Function to calculate the distance the particle travels in scintillator
bool SciBooNEUtils::PassesDistanceCut(FitParticle* beam, FitParticle* particle){

  int PID     = particle->fPID;

  // Ignore particles which are not protons or pions
  if (abs(PID) != 211 && PID != 2212) return false;

  //double test = SciBooNEUtils::BetheBlochCH(particle);
  double dist  = SciBooNEUtils::RangeInScintillator(particle, FitPar::NumRangeSteps);
  double zdist = dist*cos(FitUtils::th(beam, particle));

  if (abs(zdist) < FitPar::SciBarRecoDist) return false;
  return true;
}


// void SciBooNEUtils::CreateModeArray(TH1* hist, TH1* modearray[]){

//   std::string nameArr[] = {"CCCOH", "CCRES", "CCQE", "2p2h", "Other"};
//   for (int i = 0; i < 5; ++i)
//     modearray[i] = (TH1*)hist->Clone(Form("%s_%s",hist->GetName(),nameArr[i].c_str()));
//   return;
// };

// void SciBooNEUtils::DeleteModeArray(TH1* modearray[]){
//   for (int i = 0; i < 5; ++i)
//     delete modearray[i];
//   return;
// };


// void SciBooNEUtils::FillModeArray(TH1* hist[], int mode, double xval, double weight){

//   switch(abs(mode)) {
//   case 16: 
//     // CCCOH case
//     hist[0]->Fill(xval, weight);
//     break;
//   case 11:
//   case 12:
//   case 13:
//     // CCRES case
//     hist[1]->Fill(xval, weight);
//     break;
//   case 1:
//     hist[2]->Fill(xval, weight);
//     break;
//   case 2:
//     // CCQE-like case
//     hist[3]->Fill(xval, weight);
//     break;
//   default:
//     // Everything else
//     hist[4]->Fill(xval, weight);    
//   }
//   return;
// };

// void SciBooNEUtils::ScaleModeArray(TH1* hist[], double factor, std::string option){
  
//   for (int i = 0; i < 5; ++i)
//     if (hist[i]) hist[i]->Scale(factor,option.c_str());
//   return;
// };

// void SciBooNEUtils::ResetModeArray(TH1* hist[]){
  
//   for (int i = 0; i < 5; ++i)
//     if (hist[i]) hist[i]->Reset();
//   return;
// };


// void SciBooNEUtils::WriteModeArray(TH1* hist[]){

//   for (int i = 0; i < 5; ++i)
//     if (hist[i]) hist[i]->Write();
//   return;
// };

bool SciBooNEUtils::is1TRK(FitEvent *event){
  
  int nCharged = 0;

  // For now, require a muon
  if (event->NumFSParticle(PhysConst::pdg_muons) != 1)
    return false;

  // For one track, require a single FS particle.
  for (UInt_t j = 2; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;

    // Look for pions, protons
    if (abs(PID) == 211 || PID == 2212){

      // Must be reconstructed as a track in SciBooNE
      if (!SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j))) continue;
      nCharged += 1;

    }
  } // end loop over particle stack

  // This is the 1 track sample, require only a muon
  if (nCharged != 0) return false;
  return true;

}

// withVA: 0 = either; -1 = no VA; +1 = VA
bool SciBooNEUtils::isMuPr(FitEvent *event, int VA){

  int nCharged = 0;
  int nProtons = 0;
  int nVertex  = 0;

  // For now, require a muon
  if (event->NumFSParticle(PhysConst::pdg_muons) != 1)
    return false;

  // For one track, require a single FS particle.
  for (UInt_t j = 2; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;

    // Look for pions, muons, protons
    if (abs(PID) == 211 || PID == 2212){

      // Must be reconstructed as a track in SciBooNE
      if (SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j))){
	nCharged += 1;
	if (PID == 2212) nProtons += 1;
      } else nVertex += 1;	
    }
  } // end loop over particle stack

  if (nCharged != 1) return false;
  if (nProtons != 1) return false;

  // Cover both VA cases
  if (VA > 0 && nVertex == 0) return false;
  if (VA < 0 && nVertex != 0) return false;
  return true;

}

// Shared signal definitions
bool SciBooNEUtils::isMuPi(FitEvent *event, int VA){
  
  int nCharged = 0;
  int nPions   = 0;
  int nVertex  = 0;

  // For now, require a muon       
  if (event->NumFSParticle(PhysConst::pdg_muons) != 1)
    return false;

  // For one track, require a single FS particle.      
  for (UInt_t j = 2; j < event->Npart(); j++){
    
    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;
    
    int PID = event->PartInfo(j)->fPID;
    
    // Look for pions, muons, protons    
    if (abs(PID) == 211 || PID == 2212){
      
      // Must be reconstructed as a track in SciBooNE    
      if (SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j))){
	nCharged += 1;
	if (abs(PID) == 211) nPions += 1;
      } else nVertex += 1;
    }
  } // end loop over particle stack   

  if (nCharged != 1) return false;
  if (nPions   != 1) return false;

  // Cover both VA cases
  if (VA > 0 && nVertex == 0) return false;
  if (VA < 0 && nVertex != 0) return false;
  return true;  
}

int SciBooNEUtils::GetNTracks(FitEvent *event){
  
  int nTrks = 0;

  if (event->NumFSParticle(PhysConst::pdg_muons) != 1)
    return 0;
  nTrks+=1; // Add the muon

  for (UInt_t j = 2; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    int PID = event->PartInfo(j)->fPID;
    // Look for pions, muons, protons
    if (abs(PID) == 211 || PID == 2212){

      // Must be reconstructed as a track in SciBooNE
      if (!SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j))) continue;
      nTrks += 1;
    }
  } // end loop over particle stack                                                                                                                                            
  return nTrks;
}

FitParticle* SciBooNEUtils::GetSecondaryTrack(FitEvent *event){
  
  for (UInt_t j = 2; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    // Need a pion or proton
    if (abs(event->PartInfo(j)->fPID) != 211 && 
	event->PartInfo(j)->fPID != 2212) continue;
    if (!SciBooNEUtils::PassesDistanceCut(event->PartInfo(0), event->PartInfo(j))) continue;
    return event->PartInfo(j);
  } // end loop over particle stack     
  
  return 0;
}


// NOTE: need to adapt this to allow for penetrating events...
// Simpler, but gives the same results as in Hirade-san's thesis
double SciBooNEUtils::CalcThetaPr(FitEvent *event, bool penetrated){
  
  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  FitParticle *nu   = event->GetNeutrinoIn();
  FitParticle* secondary = SciBooNEUtils::GetSecondaryTrack(event);

  if (!muon || !nu || !secondary) return -999;

  // Construct the vector p_pr = (-p_mux, -p_muy, Enurec - pmucosthetamu)
  // where p_mux, p_muy are the projections of the muon momentum onto the x and y dimension respectively
  double pmu   = muon->fP.Vect().Mag();
  double pmu_x = muon->fP.Vect().X();
  double pmu_y = muon->fP.Vect().Y();

  if (penetrated){
    pmu = 1400.;
    double ratio = 1.4/muon->fP.Vect().Mag();
    TVector3 mod_mu = muon->fP.Vect()*ratio;
    pmu_x = mod_mu.X();
    pmu_y = mod_mu.Y();
  }

  double Enuqe = FitUtils::EnuQErec(pmu/1000.,cos(FitUtils::th(nu, muon)), 27., true)*1000.;
  double p_pr_z = Enuqe - pmu*cos(FitUtils::th(nu, muon));

  TVector3 p_pr  = TVector3(-pmu_x, -pmu_y, p_pr_z);
  double thetapr = p_pr.Angle(secondary->fP.Vect())/TMath::Pi()*180.;

  return thetapr;
}

double SciBooNEUtils::CalcThetaPi(FitEvent *event){

  FitParticle *nu   = event->GetNeutrinoIn();
  FitParticle* secondary = SciBooNEUtils::GetSecondaryTrack(event);

  if (!secondary) return -999.;

  double thetapi = FitUtils::th(nu, secondary)/TMath::Pi()*180.;
  return thetapi;
}

// Function to return the MainTrk
//bool SciBooNEUtils::MainTrk(FitEvent *event, bool penetrated){
//}


/// Functions to deal with the SB mode stacks
SciBooNEUtils::ModeStack::ModeStack(std::string name, std::string title, TH1* hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "CCCOH",  "CCCOH", kGreen+2, 2, 3244);
  AddMode(1, "CCRES",  "CCRES", kRed,     2, 3304);
  AddMode(2, "CCQE",   "CCQE",  kGray+2,  2, 1001);
  AddMode(3, "2o2h",   "2p2h",  kMagenta, 2, 1001);
  AddMode(4, "Other",  "Other", kAzure+1, 2, 1001);
  
  StackBase::SetupStack(hist);
};



int SciBooNEUtils::ModeStack::ConvertModeToIndex(int mode){
  switch (abs(mode)){
  case 16: return 0; // CCCOH
  case 11:
  case 12:
  case 13: return 1; // CCRES
  case  1: return 2; // CCQE
  case  2: return 3; // 2p2h
  default: return 4; // Other
  }
};


void SciBooNEUtils::ModeStack::Fill(int mode, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack::Fill(FitEvent* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

void SciBooNEUtils::ModeStack::Fill(BaseFitEvt* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(SciBooNEUtils::ModeStack::ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

