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

#include "K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks.h"
#include "TLorentzVector.h"

//******************************************************************** 
K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

   // Define Measurement
  measurementName = "K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks";
  plotTitles = "; #Delta#Phi (Deg.); Events";
  EnuMin = 0.3;
  EnuMax = 5.;
  isDiag = true;
  isRawEvents = true;
  matchnorm = type.find("SCL") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);


  // Set the plot to be read in
  this->SetDataFromFile(FitPar::GetDataBase()+"/K2K/K2K_Data_PRD74_052002.root", "Data_1DDelTheta_2tracks");

  // Forced to be diag for now
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar= StatUtils::GetInvert(fullcovar);


  // Setup MC Hists
  SetupDefaultHist();
  this->K2KI_holder   = (TH1D*) this->mcHist->Clone("K2KI_holder");
  this->K2KIIa_holder = (TH1D*) this->mcHist->Clone("K2KIIa_holder");
  
  //  double POT = 0.922E20;
  //  double cm2 = 1.0E12 * 130000;
  //  double gev = 1.0E9;

  // Different generators require slightly different rescaling factors.
  if      (this->eventType == 0) this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); // NEUT
  else if (this->eventType == 1) this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); // NUWRO
  else if (this->eventType == 5) this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); // GENIE      

  // Other Variables
  finished_fit = false;
  mcSignalEvt = 0;
  dataSignalEvt = 13440;
  
};



//********************************************************************      
void K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::FillEventVariables(FitEvent *event){
//********************************************************************      

  bad_particle = false;
  Pmu = 0.0;
  ncharged = 0;

  TLorentzVector vecpnu = (event->PartInfo(0))->fP;
  TLorentzVector vecpmu;
  TLorentzVector vecppr;
  TLorentzVector vecpne;
  
  // loop particles
  for (int i = 0; i < event->Npart(); i++){

    if (i < 2 and (event->PartInfo(i))->fPID == 2112){
      vecpne = (event->PartInfo(i))->fP;
    }
    
    // skip bad ones
    if (!(event->PartInfo(i))->fIsAlive or (event->PartInfo(i))->fStatus != 0 ) continue;

    // pid check
    if ((event->PartInfo(i))->fPID == 13){

      ThetaMu = (event->PartInfo(i))->fP.Vect().Angle( (event->PartInfo(0))->fP.Vect() ) * 57.2958;
	
      Pmu = (event->PartInfo(i))->fP.Vect().Mag()/1000.0;
      ncharged++;
      vecpnu = (event->PartInfo(i))->fP;
      
      // CC0pi like
    } else if ((event->PartInfo(i))->fPID == 2212){

      if ((event->PartInfo(i))->fP.Vect().Mag() > 600.0){
	ncharged++;
	vecppr = (event->PartInfo(i))->fP;
      }
    
    } else if ((event->PartInfo(i))->fPID == -211 or
	       (event->PartInfo(i))->fPID == 211){

      if ((event->PartInfo(i))->fP.Vect().Mag() > 200.0)
	ncharged++;
    } else if ((event->PartInfo(i))->fPID != 22 and
	       (event->PartInfo(i))->fPID != 2112){
      bad_particle == true;
    }
  }

  if (ncharged == 2){
    double Mmu = 105.392;
    double Mpr = 938.12;
    double Mne = 935.12;
    double Emu = vecpmu.E();
    double Epr = vecppr.E();
    double Eb  = Epr - Mne;

    DelPhi = acos( ( Mmu*Mmu + Mpr*Mpr + Mne*Mne + 2*Epr*Mne + 2*Eb*Emu ) /
		     ( vecpmu.Vect().Mag() * vecppr.Vect().Mag() ) ) * 59.78;

  }
  
  this->X_VAR = DelPhi;

  LOG(EVT) << "Event variables for "<<this->measurementName<<std::endl;
  LOG(EVT)<<"X_VAR = "<<this->X_VAR<<std::endl;
  LOG(EVT)<<"bad_particle = "<<bad_particle<<std::endl;
  LOG(EVT)<<"ncharged = "<<ncharged<<std::endl;
  
  return;
}

//******************************************************************** 
bool K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::isSignal(FitEvent *event){
//******************************************************************** 
 
  // Signal is defined as a CC0pi sample in the final state with only 1 charged track
  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict energy range
  if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;
  
  //  if (bad_particle) return false;

  if (Pmu < 0.550 or Pmu > 3.5) return false;

  if (!finished_fit and ncharged < 4) mcSignalEvt++;

  if (ncharged > 2 or ncharged == 0) return false;

  if (ievt < (8144.0/(4967.0+8144.0))*(nevents+0.) and
      Pmu < 0.675)
    return false;
    
  
  return true;
};

//******************************************************************** 
void K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::FillHistograms(){
//******************************************************************** 

  if (Signal){
 
    if (ievt < (8144.0 * (nevents + 0.)/(this->dataSignalEvt+0.)))
      this->K2KI_holder->Fill(this->X_VAR, Weight);
    else if (ievt >= (8144.0 * (nevents + 0.)/(this->dataSignalEvt+0.)))
      if (Pmu > 0.675)
	this->K2KIIa_holder->Fill(this->X_VAR, Weight);
    
  }
  
  Measurement1D::FillHistograms();
  return;

}


//********************************************************************  
void K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::ScaleEvents(){
//********************************************************************  

  if (!finished_fit){
    finished_fit = true;
    if (matchnorm){
      //     this->scaleFactor = (dataSignalEvt + 0.) / (mcSignalEvt + 0.);
      this->scaleFactor = (this->dataHist->Integral() + 0.) /(this->mcHist->Integral());
    }
  }
  
  this->mcHist->Scale(this->scaleFactor);
  this->mcFine->Scale(this->scaleFactor);

  this->K2KI_holder->Scale(this->scaleFactor);
  this->K2KIIa_holder->Scale(this->scaleFactor);

  PlotUtils::ScaleNeutModeArray((TH1**)this->mcHist_PDG, this->scaleFactor, "");
  
  return;
}

//********************************************************************
void K2K_CC0pi_XSec_1DDelPhi_nu_Ntrks::ResetAll(){
//********************************************************************

  this->K2KI_holder->Reset();
  this->K2KIIa_holder->Reset();

  return;
}
