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

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC0pi_XSec_1DEe_nue.h"

//********************************************************************
MINERvA_CC0pi_XSec_1DEe_nue::MINERvA_CC0pi_XSec_1DEe_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//********************************************************************

  // Define Measurement
  fName = "MINERvA_CC0pi_XSec_1DEe_nue";
  fPlotTitles = "; E_{e} (GeV); d#sigma/dE_{e} (cm^{2}/GeV)";
  EnuMin = 0.0;
  EnuMax = 20.0;
  fNormError = 0.101;
  fDefaultTypes = "FIX/FULL";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Data File
  std::string datafile = FitPar::GetDataBase()+"/MINERvA/CC0pi/MINERvA_CC0pi_nue_Data_ARX1509_05729.root";
  std::string dist_name = "";

  dist_name = "1DEe";
  fPlotTitles = "; Q_{QE}^{2} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";

  SetDataFromFile(datafile, "Data_" + dist_name);
  SetCovarFromDataFile(datafile, "Covar_" + dist_name);

  // Setup Default MC Hists
  SetupDefaultHist();

  //  fFullCovar *= 10.0;
  //  covar      *= 1.0/10.0;

  // Different generators require slightly different rescaling factors.
  fScaleFactor = (fEventHist->Integral("width")*100.0*1E-38/(fNEvents+0.))/TotalIntegratedFlux();

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DEe_nue::FillEventVariables(FitEvent *event){
  //********************************************************************

  Enu_rec = 0.0;

  // Get the relevant signal information
  for (UInt_t j = 2; j < event->Npart(); ++j){

    int PID = (event->PartInfo(j))->fPID;
    if (!event->PartInfo(j)->fIsAlive) continue;

    if (abs(PID) == 11){

      Thetae     = fabs((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()));
      Ee         = (event->PartInfo(j))->fP.E()/1000.0;

      Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(Thetae), 34.,true);
      Q2QEe       = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(Thetae), 34.,true);

      break;
    }
  }

  LOG(EVT)<<"fXVar = "<<Ee<<std::endl;
  fXVar = Ee;

  return;
}



//********************************************************************
bool MINERvA_CC0pi_XSec_1DEe_nue::isSignal(FitEvent *event){
//*******************************************************************

  // Check that this is a nue CC0pi event
  if (!SignalDef::isCC0pi(event, 12, EnuMin, EnuMax)) return false;

  // Electron Enrgy
  if (Ee < 0.5) return false;

  return true;
};

