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

#include "K2K_CC0pi_XSec_1DQ2_nu_subtrks.h"
#include "TLorentzVector.h"

//******************************************************************** 
K2K_CC0pi_XSec_1DQ2_nu_subtrks::K2K_CC0pi_XSec_1DQ2_nu_subtrks(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

  // Define the Measurement
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV/c^{2}); Events";
  EnuMin = 0.3;
  EnuMax = 5.;
  isDiag = true;
  isRawEvents = true;
  matchnorm = type.find("SCL") != std::string::npos;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Set the sample type
  if (name.find("K2KI_") != std::string::npos){
    if      (name.find("_1trk")      != std::string::npos){ k2k_sample_type = fK2KI_1track;   }
    else if (name.find("_2trkQE")    != std::string::npos){ k2k_sample_type = fK2KI_2trackQE;   }
    else if (name.find("_2trkNonQE") != std::string::npos){ k2k_sample_type = fK2KI_2trackNONQE;   }

  } else if (name.find("K2KIIa_") != std::string::npos){
    if      (name.find("_1trk")      != std::string::npos){ k2k_sample_type = fK2KIIa_1track;   }
    else if (name.find("_2trkQE")    != std::string::npos){ k2k_sample_type = fK2KIIa_2trackQE;   }
    else if (name.find("_2trkNonQE") != std::string::npos){ k2k_sample_type = fK2KIIa_2trackNONQE;   }
  }
  
  // Setup the cuts
  switch(k2k_sample_type){

  case fK2KI_1track:
  case fK2KIIa_1track:
    ncharged_cut = 1;
    deltaphi_cut = 999;
    break;
    
  case fK2KI_2trackQE:
  case fK2KIIa_2trackQE:
    ncharged_cut  = 2;
    deltaphi_cut  = 0; // change later
    deltaphi_type = false;
    break;

  case fK2KI_2trackNONQE:
  case fK2KIIa_2trackNONQE:
    ncharged_cut  = 2;
    deltaphi_cut  = 999;
    deltaphi_type = true;
    break;
    
  default:
    LOG(SAM) << this->measurementName  << "Incorrect K2K sample type included: " << name << std::endl;
    ncharged_cut = 0;
    deltaphi_cut = 0;
    deltaphi_type = false;
    break;
  }

  
  // Set the plot to be read in
  std::string plotname = "";
  switch(k2k_sample_type){

  case fK2KI_1track:        plotname = "Data_1DQ2_K2KI_1track";        break;
  case fK2KIIa_1track:      plotname = "Data_1DQ2_K2KIIa_1track";      break;
  case fK2KI_2trackQE:      plotname = "Data_1DQ2_K2KI_2trackQE";     break;
  case fK2KIIa_2trackQE:    plotname = "Data_1DQ2_K2KIIa_2trackQE";    break;
  case fK2KI_2trackNONQE:   plotname = "Data_1DQ2_K2KI_2trackNONQE";   break;
  case fK2KIIa_2trackNONQE: plotname = "Data_1DQ2_K2KIIa_2trackNONQE"; break;
  }
  
  this->SetDataFromFile(std::string(std::getenv("NIWG_DATA"))+"/K2K/K2K_Data_PRD74_052002.root", plotname);
  
  // Setup Covariance;
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar= StatUtils::GetInvert(fullcovar);
  
  // setup mc histograms
  SetupDefaultHist();

  // Different generators require slightly different rescaling factors.
  if      (this->eventType == 0) this->scaleFactor = (this->eventHist->Integral()/(nevents+0.)); // NEUT
  else if (this->eventType == 1) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.)); // NUWRO
  else if (this->eventType == 5) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.)); // GENIE      

};



//********************************************************************      
void K2K_CC0pi_XSec_1DQ2_nu_subtrks::FillEventVariables(FitEvent *event){
//********************************************************************      

  ncharged = 0;
  bad_particle = false;
  CosThetaMu = -999.9;
  q2qe = -1.0;
  deltaphi = 0.0;
  
  LOG(EVT) << "Filling event variables for K2K" << this->measurementName<<std::endl;

  //  TLorentzVector pnu = (event->PartInfo(0))->fP;
  //  TLorentzVector pmu;
  //  TLorentzVector ppr;
  
  // loop particles
  for (int i = 2; i < event->Npart(); i++){

    // skip bad ones
    if (!(event->PartInfo(i))->fIsAlive or (event->PartInfo(i))->fStatus != 0 ) continue;

    // pid check
    if ((event->PartInfo(i))->fPID == 13){

      ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(i))->fP.Vect());
      q2qe        = FitUtils::Q2QErec((event->PartInfo(i))->fP, cos(ThetaMu), 34. ,true);

      deltaphi = 0.0;

      // FOR QE approx the momentum between the muon and proton should equal the neutrino.
      //  pmu = (event->PartInfo(i))->fP;
      ncharged++;
      
      // CC0pi like
    } else if ((event->PartInfo(i))->fPID != 22 &&
	       (event->PartInfo(i))->fPID != 2212 &&
	       (event->PartInfo(i))->fPID != 2112 &&
	       (event->PartInfo(i))->fPID != 13){

      bad_particle = true;

      // ncharged particles
    }
    
    //  if ((event->PartInfo(i))->fPID == 2212){
    //      ppr =  (event->PartInfo(i))->fP;
    //      ncharged++;
    //    }
  }

  // if (ncharged == 2){
    
  //   TLorentzVector pQE = pnu - pmu;
  //   deltaphi = pQE.Vect().Angle(ppr.Vect());
    
  // }
  
  this->X_VAR = q2qe;

  LOG(EVT) << "Event variables for "<<this->measurementName<<std::endl;
  LOG(EVT)<<"X_VAR = "<<this->X_VAR<<std::endl;
  LOG(EVT)<<"ncharged = "<<ncharged<<std::endl;
  LOG(EVT)<<"bad_particle = "<<bad_particle<<std::endl;
  LOG(EVT)<<"Delta_phi = "<<deltaphi<<std::endl;
  return;
}

//******************************************************************** 
bool K2K_CC0pi_XSec_1DQ2_nu_subtrks::isSignal(FitEvent *event){
//******************************************************************** 
 
  // Signal is defined as a CC0pi sample in the final state with only 1 charged track
  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict energy range
  //if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;
  
  if (bad_particle) return false;
  
  if (( ncharged > ncharged_cut and ncharged == 2) or ncharged == 0) return false;

  // QE Cuts
  if ( ( deltaphi_type and deltaphi > deltaphi_cut) or
       (!deltaphi_type and deltaphi < deltaphi_cut) ) return false;
    
  return true;
};

//********************************************************************  
void K2K_CC0pi_XSec_1DQ2_nu_subtrks::ScaleEvents(){
//********************************************************************  

  this->mcHist->Scale(this->scaleFactor);
  this->mcFine->Scale(this->scaleFactor);

  //  double scaleF = 5933.0/this->mcHist->Integral();
  //  LOG(SAM) << this->measurementName  <<"ScaleF = "<<scaleF<<std::endl;

  //  this->mcHist->Scale(scaleF);
  //  this->mcFine->Scale(scaleF);
  
  // this->mcHist->Scale(
  // this->mcFine->Scale(this->dataHist->Integral() / this->mcHist->Integral());

  // PlotUtils::ScaleNeutModeArray((TH1**)this->mcHist_PDG, this->dataHist->Integral() / this->mcHist->Integral(), "");
  
  return;
}
