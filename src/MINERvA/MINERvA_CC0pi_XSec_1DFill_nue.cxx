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

#include "MINERvA_CC0pi_XSec_1DFill_nue.h"

//******************************************************************** 
MINERvA_CC0pi_XSec_1DFill_nue::MINERvA_CC0pi_XSec_1DFill_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile, bool neutrino_mode, int fillvar){
//******************************************************************** 

  // Define Measurement
  if (neutrino_mode){
    if (fillvar == 0)      measurementName = "MINERvA_CC0pi_XSec_1DEe_nue";
    else if (fillvar == 1) measurementName = "MINERvA_CC0pi_XSec_1DQ2_nue";
    else if (fillvar == 2) measurementName = "MINERvA_CC0pi_XSec_1DThetae_nue";
  } else {
    if (fillvar == 0)      measurementName = "MINERvA_CC0pi_XSec_1DEe_nuebar";
    else if (fillvar == 1) measurementName = "MINERvA_CC0pi_XSec_1DQ2_nuebar";
    else if (fillvar == 2) measurementName = "MINERvA_CC0pi_XSec_1DThetae_nuebar";
  }

  plotTitles = "; E_{e} (GeV); d#sigma/dE_{e} (cm^{2}/GeV)";
  EnuMin = 0.0;
  EnuMax = 10.0;
  nue_flag = neutrino_mode;
  normError = 0.101;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Data File
  std::string datafile = std::string(std::getenv("NIWG_DATA"))+"/MINERvA/MINERvA_CC0pi_nue_Data_ARX1509_05729.root";
  std::string dist_name = "";
  
  if  (fillvar == 0) {
    dist_name = "1DEe";
    plotTitles = "; E_{e} (GeV); d#sigma/dE_{e} (cm^{2}/GeV)";

  } else if (fillvar == 1) {
    dist_name = "1DQ2";
    plotTitles = "; Q_{QE}^{2} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";

  } else if (fillvar == 2) { 
    dist_name = "1DThetae";
    plotTitles = "; #theta_{e} (deg.); d#sigma/d#theta_{e} (cm^{2}/deg)";

  }
  
  SetDataFromFile(datafile, "Data_" + dist_name);
  SetCovarFromDataFile(datafile, "Covar_" + dist_name);

  // Setup Default MC Hists
  SetupDefaultHist();

  // Different generators require slightly different rescaling factors.
  scaleFactor = (this->eventHist->Integral("width")*1E-39/(nevents+0.))/this->TotalIntegratedFlux(); 

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DFill_nue::FillEventVariables(FitEvent *event){
  //********************************************************************

  Enu_rec = 0.0;
  bad_particle = false;
  
  // Get the relevant signal information
  for (int j = 2; j < event->Npart(); ++j){

    int PID = (event->PartInfo(j))->fPID;
    
    if (abs(PID) == 11){

      Thetae     = fabs((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()));
      Ee = (event->PartInfo(j))->fP.E()/1000.0;

      if (PID > 0){
	Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 34.,true);
	Q2QEe   = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(Thetae), 34.,true);
      } else {
	Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 30.,false);
	Q2QEe   = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(Thetae), 30.,false);
      }
     
    } else if (PID != 2212 or
	       PID != 2112 or
	       PID != 22){
      bad_particle = true;
    }
  }
  
  LOG(EVT)<<"X_VAR = "<<Ee<<std::endl;
  
  if (Fill1DEe)  this->X_VAR = Ee;
  else if (Fill1DQ2e) this->X_VAR = Q2QEe;
  else if (Fill1DThetae) this->X_VAR = Thetae * 57.98;

  return;
}



//********************************************************************
bool MINERvA_CC0pi_XSec_1DFill_nue::isSignal(FitEvent *event){
//*******************************************************************

  if (!nue_flag){
    LOG(EVT) << "PID Nu = "<< ((event->PartInfo(0)->fPID) != -12)<<std::endl;
  }
  // Only look at numu events
  if ( nue_flag and (event->PartInfo(0)->fPID) != 12) return false;
  if (!nue_flag and (event->PartInfo(0)->fPID) != -12) return false;
  
  // restrict energy range
  if (Enu < this->EnuMin || Enu > this->EnuMax) return false;

  if (Ee < 0.5) return false;
  
  return true;
};

