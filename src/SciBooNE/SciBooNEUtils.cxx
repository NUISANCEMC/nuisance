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

double SciBooNEUtils::CalcEfficiency(TH2D *effHist, FitParticle *nu, FitParticle *muon){

  double eff = 1.;

  if (!effHist) return eff;

  double pmu = muon->fP.Vect().Mag()/1000.;
  double thetamu = nu->fP.Vect().Angle(muon->fP.Vect());
  eff = effHist->GetBinContent(effHist->FindBin(pmu, thetamu));

  return eff;
}



// Function to calculate the distance the particle travels in scintillator
bool SciBooNEUtils::DistanceInScintillator(FitParticle* beam, FitParticle* particle){

  double mom  = particle->fP.Vect().Mag();
  double zmom = mom*cos(beam->fP.Vect().Angle(particle->fP.Vect()));
  int PID     = particle->fPID;
  
  if (abs(PID) == 211){
    if (fabs(zmom) < FitPar::Config().GetParD("SciBooNE_COH_pion_cut"))
      return false;
  } else if (PID == 2212){
    if (fabs(zmom) < FitPar::Config().GetParD("SciBooNE_COH_proton_cut"))
      return false;
  }
  return true;
}


void SciBooNEUtils::CreateModeArray(TH1* hist, TH1* modearray[]){

  std::string nameArr[] = {"CCCOH", "CCRES", "CCQElike", "Other"};
  for (int i = 0; i < 4; ++i)
    modearray[i] = (TH1*)hist->Clone(Form("%s_%s",hist->GetName(),nameArr[i].c_str()));
  return;
};

void SciBooNEUtils::DeleteModeArray(TH1* modearray[]){
  for (int i = 0; i < 4; ++i)
    delete modearray[i];
  return;
};


void SciBooNEUtils::FillModeArray(TH1* hist[], int mode, double xval, double weight){

  switch(abs(mode)) {
  case 16: 
    // CCCOH case
    hist[0]->Fill(xval, weight);
    break;
  case 11:
  case 12:
  case 13:
    // CCRES case
    hist[1]->Fill(xval, weight);
    break;
  case 1:
  case 2:
    // CCQE-like case
    hist[2]->Fill(xval, weight);
    break;
  default:
    // Everything else
    hist[3]->Fill(xval, weight);    
  }
  return;
};

void SciBooNEUtils::ScaleModeArray(TH1* hist[], double factor, std::string option){
  
  for (int i = 0; i < 4; ++i)
    if (hist[i]) hist[i]->Scale(factor,option.c_str());
  return;
};

void SciBooNEUtils::ResetModeArray(TH1* hist[]){
  
  for (int i = 0; i < 4; ++i)
    if (hist[i]) hist[i]->Reset();
  return;
};


void SciBooNEUtils::WriteModeArray(TH1* hist[]){

  for (int i = 0; i < 4; ++i)
    if (hist[i]) hist[i]->Write();
  return;
};


// Shared signal definitions
bool SciBooNEUtils::isMuPiSignal(FitEvent *event, bool withVA){
  
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
      if (SciBooNEUtils::DistanceInScintillator(event->PartInfo(0), event->PartInfo(j))){
	nCharged += 1;
	if (PID == 211) nPions += 1;
      } else nVertex += 1;
    }
    // Also include neutrons in VA     
    // if (PID == 2112) nVertex += 1;
  } // end loop over particle stack   

  if (nCharged != 1) return false;
  if (nPions   != 1) return false;

  // Cover both VA cases
  if (withVA  && nVertex == 0) return false;
  if (!withVA && nVertex != 0) return false;
  return true;  
}

FitParticle* SciBooNEUtils::GetSecondaryTrack(FitEvent *event){
  
  for (UInt_t j = 2; j < event->Npart(); j++){

    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fNEUTStatusCode != 0) continue;

    // Need a pion which 
    if (event->PartInfo(j)->fPID != 211) continue;
    if (!SciBooNEUtils::DistanceInScintillator(event->PartInfo(0), event->PartInfo(j))) continue;
    return event->PartInfo(j);
  } // end loop over particle stack                
  
  return 0;
}

double SciBooNEUtils::CalcThetaPr(FitEvent *event){
  
  FitParticle *muon = event->GetHMFSParticle(PhysConst::pdg_muons);
  FitParticle *nu   = event->GetNeutrinoIn();

  // Construct the vector p_pr = (-p_mux, -p_muy, Enurec - pmucosthetamu)
  // where p_mux, p_muy are the projections of the muon momentum onto the x and y dimension respectively
  double Enuqe = FitUtils::EnuQErec(muon->fP,cos(nu->fP.Vect().Angle(muon->fP.Vect())), 27., true);
  double p_pr_z = Enuqe - muon->fP.Vect().Mag()*cos(nu->fP.Vect().Angle(muon->fP.Vect()));

  FitParticle* secondary = SciBooNEUtils::GetSecondaryTrack(event);
  
  if (!secondary) return -999.;
  
  TVector3 p_pr  = TVector3(-muon->fP.Vect().X(), -muon->fP.Vect().Y(), -p_pr_z);
  double thetapr = p_pr.Angle(secondary->fP.Vect())/TMath::Pi()*180.;

  return thetapr;
}

double SciBooNEUtils::CalcThetaPi(FitEvent *event){

  FitParticle *nu   = event->GetNeutrinoIn();
  FitParticle* secondary = SciBooNEUtils::GetSecondaryTrack(event);

  if (!secondary) return -999.;

  double thetapi = secondary->fP.Vect().Angle(nu->fP.Vect())/TMath::Pi()*180.;
  return thetapi;
}
