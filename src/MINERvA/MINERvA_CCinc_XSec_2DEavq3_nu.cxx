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

#include "MINERvA_CCinc_XSec_2DEavq3_nu.h"

//********************************************************************   
MINERvA_CCinc_XSec_2DEavq3_nu::MINERvA_CCinc_XSec_2DEavq3_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//********************************************************************

  // Measurement Details
  measurementName = "MINERvA_CCinc_XSec_2DEavq3_nu";
  plotTitles = "; q_{3} (GeV); E_{avail} (GeV); d^{2}#sigma/dq_{3}dE_{avail} (cm^{2}/GeV^{2})";
  EnuMin = 2.;
  EnuMax = 6.;
  default_types = "FIX/FULL";
  allowed_types = "FIX,FREE,SHAPE/FULL,DIAG/MASK";
  hadroncut = FitPar::Config().GetParI("MINERvA_CCinc_XSec_2DEavq3_nu.hadron_cut") > 0;
  normError = 0.107;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Binning for the 2D Histograms
  this->data_points_x = 7;
  this->data_points_y = 17;
  Double_t tempx[7] = {0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8};
  Double_t tempy[17] = {0.0, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.20, 0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.80};
  this->xBins = tempx;
  this->yBins = tempy;
  
  // Fill data and 1Dto2D Maps for covariance
  SetDataValuesFromText(std::string(std::getenv("NIWG_DATA"))+"/MINERvA/CCEavQ3/data_2D.txt", 1E-42);
  SetMapValuesFromText(std::string(std::getenv("NIWG_DATA"))+"/MINERvA/CCEavQ3/map_2D.txt");
  SetCovarMatrixFromChol(std::string(std::getenv("NIWG_DATA"))+"/MINERvA/CCEavQ3/covar_2D.txt", 67);

  // Data is in 1E-42 and so is the covariance, need to scale accordingly.
  (*this->fullcovar) *= 1E-16;
  (*this->covar) *= 1E16;
  
  // Set data errors from covariance matrix
  StatUtils::SetDataErrorFromCov(dataHist, fullcovar, mapHist, 1E-38);
    
  // Setup mc Histograms
  SetupDefaultHist();
 
  // Set Scale Factor
  scaleFactor = (this->eventHist->Integral("width")*1E-42/(nevents+0.))/this->TotalIntegratedFlux(); 
};



//******************************************************************** 
void MINERvA_CCinc_XSec_2DEavq3_nu::FillEventVariables(FitEvent *event){
//********************************************************************

  Mode = event->Mode;

  // Set starting variables
  Enu  = (event->PartInfo(0))->fP.E()/1000.0;
  Eav  = 0.0; // Energy Avaiable
  q3   = 0.0; // Three momentum transfer
  q0   = 0.0; // True Energy Transfer
  true_q3 = 0.0;
  
  // Cuts on muon and hadron found
  muonfound   = false;
  hadronfound = false;

  // Counters for protons
  nprotons = npions = npiz = npip = npim = ne = ngam = nneutrons = nother = 0;
  nparticles = 0;

  // Individual particle variables
  PID  = 0;
  Q2   = 0.0;
  q = TLorentzVector(); // reset vector

  // Muon Variables -----------------
  // Muon should be in slot 2 or 3.
  for (int j = 2; j < event->Npart(); j++){

    PID = event->PartInfo(j)->fPID;
    
    if (PID == 13){

      // Found the muon
      if (!muonfound) muonfound = true;
      
      // Set q from muon
      q = ((event->PartInfo(0))->fP - (event->PartInfo(j))->fP);
      q0 = q.E()/1000.0;
      
      // Other muon variables
      ThetaMu = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
      pmu = ((event->PartInfo(j))->fP.Vect().Mag())/1000.0;
    
      Emu     = ((event->PartInfo(j))->fP.E())/1000.0;
      
      Enu_rec = Emu + q0;

      // Set Q2 from reconstruction method            
      Q2 = 2*Enu_rec * (Emu - pmu * cos(ThetaMu)) - 0.1057*0.1057;
      
      // merge together for q3        
      q3 = sqrt( Q2 + q0*q0 );

      continue;
    }
  
    if (!(event->PartInfo(j))->fIsAlive) continue;
    if (event->PartInfo(j)->fStatus != 0) continue;

    // Eav Varible -----------------
    // Proton and charged pions KE                                                                                                                                                              
    if (PID == 2212 or PID ==  211 or PID == -211){
      if (!hadronfound) hadronfound = true;
      
      Eav += (event->PartInfo(j)->fP.E() - event->PartInfo(j)->fP.Mag())/1000.0;
    }
    // Not a neutron TE                                                                                                                                                                               
    else if (PID != 2112 and PID < 999 and PID != 22 and PID != 14){
      if (!hadronfound) hadronfound = true;
      Eav += (event->PartInfo(j)->fP.E())/1000.0;
    }
  }
    
  this->X_VAR = q3;
  this->Y_VAR = Eav;

  return;
}

//******************************************************************** 
bool MINERvA_CCinc_XSec_2DEavq3_nu::isSignal(FitEvent *event){
//********************************************************************

  // Need at least one muon
  if (!muonfound) return false;

  // Require Eav > 0.0
  if (hadroncut and !hadronfound) return false;
  
  // Only look at numu events
  if ((event->PartInfo(0))->fPID != 14) return false;

  // Restrict true energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;
  
  // Restrict reconstructed energy range
  if (Enu_rec < this->EnuMin || Enu_rec > this->EnuMax) return false;
  
  // Cut on muon angle greated than 20deg
  if (cos(ThetaMu) < 0.93969262078) return false;

  // Cut on muon energy < 1.5 GeV
  if (Emu < 1.5) return false;
  
  return true;
};

