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

#include "K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks.h"
#include "TLorentzVector.h"

//******************************************************************** 
K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks::K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

  // Define the Measurement
  measurementName = name;
  plotTitles = "; cos(#theta_{#mu}); Events";
  EnuMin = 0.3;
  EnuMax = 5.;
  isDiag = true;
  isRawEvents = true;
  matchnorm = type.find("SCL") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  
  // Set the sample type
  if      (name.find("1trk") != std::string::npos)  { k2k_sample_type = fK2K_1track;   }
  else if (name.find("2trkQE") != std::string::npos){ k2k_sample_type = fK2K_2trackQE; }
  else if (name.find("2trkNonQE") != std::string::npos){ k2k_sample_type = fK2K_2trackNONQE; }
  
  // Setup the cuts
  switch(k2k_sample_type){

  case fK2K_1track:
    ncharged_cut = 1;
    deltaphi_cut = 999;
    break;
    
  case fK2K_2trackQE:
    ncharged_cut = 2;
    deltaphi_cut = 999; // change later
    break;

  case fK2K_2trackNONQE:
    ncharged_cut = 2;
    deltaphi_cut = 999;
    break;
    
  default:
    LOG(SAM) << this->measurementName  << "Incorrect K2K sample type included: " << name << std::endl;
    ncharged_cut = 0;
    deltaphi_cut = 0;
    break;
  }

  // Set the plot to be read in
  std::string plotname = "";
  switch(k2k_sample_type){

  case fK2K_1track:        plotname = "Data_1DCosThetaMu_SmlAngle_1track";        break;
  case fK2K_2trackQE:      plotname = "Data_1DCosThetaMu_SmlAngle_2trackQE";      break;
  case fK2K_2trackNONQE:      plotname = "Data_1DCosThetaMu_SmlAngle_2trackNONQE";     break;
  }
  
  this->SetDataFromFile(FitPar::GetDataBase()+"/K2K/K2K_Data_PRD74_052002.root", plotname);

  // Setup MC Plots
  this->SetupDefaultHist();

  // Forced to be diag for now
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar = StatUtils::GetInvert(fullcovar);
  
  // Different generators require slightly different rescaling factors.
  if      (this->eventType == 0) this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); // NEUT
  else if (this->eventType == 1) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.)); // NUWRO
  else if (this->eventType == 5) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.)); // GENIE      

};



//********************************************************************      
void K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks::FillEventVariables(FitEvent *event){
//********************************************************************      

  ncharged = 0;
  bad_particle = false;
  CosThetaMu = 0.0;
  
  // loop particles
  for (int i = 2; i < event->Npart(); i++){

    // skip bad ones
    if (!(event->PartInfo(i))->fIsAlive or (event->PartInfo(i))->fStatus != 0 ) continue;

    // pid check
    if ((event->PartInfo(i))->fPID == 13){
      
      CosThetaMu = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(i))->fP.Vect())));
      
      // CC0pi like
    } else if ((event->PartInfo(i))->fPID != 22 &&
	       (event->PartInfo(i))->fPID != 2212 &&
	       (event->PartInfo(i))->fPID != 2112 &&
	       (event->PartInfo(i))->fPID != 13){

      bad_particle = true;

      // ncharged particles
    }
    
    if ((event->PartInfo(i))->fPID == 2212 or
	(event->PartInfo(i))->fPID == 13){
      
      ncharged++;
      
    }  
  }

  this->X_VAR = CosThetaMu;

  LOG(EVT) << "Event variables for "<<this->measurementName<<std::endl;
  LOG(EVT)<<"X_VAR = "<<this->X_VAR<<std::endl;
  LOG(EVT)<<"ncharged = "<<ncharged<<std::endl;
  LOG(EVT)<<"bad_particle = "<<bad_particle<<std::endl;
  
  return;
}

//******************************************************************** 
bool K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks::isSignal(FitEvent *event){
//******************************************************************** 
 
  // Signal is defined as a CC0pi sample in the final state with only 1 charged track
  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict energy range
  //if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;
  
  if (bad_particle) return false;

  if (ncharged != ncharged_cut) return false;

  return true;
};

//********************************************************************  
void K2K_CC0pi_XSec_1DCosThetaMu_nu_subtrks::ScaleEvents(){
//********************************************************************  

  this->mcHist->Scale(this->scaleFactor);
  this->mcFine->Scale(this->scaleFactor);
  
  return;
}
