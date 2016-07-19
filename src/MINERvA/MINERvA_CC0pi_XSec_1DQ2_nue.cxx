#include "MINERvA_CC0pi_XSec_1DQ2_nue.h"

//******************************************************************** 
MINERvA_CC0pi_XSec_1DQ2_nue::MINERvA_CC0pi_XSec_1DQ2_nue(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){
//******************************************************************** 

  // Define Measurement
  measurementName = "MINERvA_CC0pi_XSec_1DQ2_nue";
  plotTitles = "; E_{e} (GeV); d#sigma/dE_{e} (cm^{2}/GeV)";
  EnuMin = 0.0;
  EnuMax = 10.0;
  normError = 0.101;
  default_types = "FIX/FULL";
  allowed_types = "FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Data File
  std::string datafile = FitPar::GetDataBase()+"/MINERvA/MINERvA_CC0pi_nue_Data_ARX1509_05729.root";
  std::string dist_name = "";
  
  dist_name = "1DQ2";
  plotTitles = "; Q_{QE}^{2} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  
  SetDataFromFile(datafile, "Data_" + dist_name);
  SetCovarFromDataFile(datafile, "Covar_" + dist_name);

  // Setup Default MC Hists
  SetupDefaultHist();

  // Different generators require slightly different rescaling factors.
  scaleFactor = (this->eventHist->Integral("width")*1E-38/(nevents+0.))/this->TotalIntegratedFlux(); 

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DQ2_nue::FillEventVariables(FitEvent *event){
  //********************************************************************

  Enu_rec = 0.0;
  bad_particle = false;
  
  // Get the relevant signal information
  for (int j = 2; j < event->Npart(); ++j){

    int PID = (event->PartInfo(j))->fPID;
    if (!event->PartInfo(j)->fIsAlive) continue;

    if (abs(PID) == 11){

      Thetae     = fabs((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()));
      Ee = (event->PartInfo(j))->fP.E()/1000.0;

      Enu_rec     = FitUtils::EnuQErec((event->PartInfo(j))->fP, cos(ThetaMu), 34.,true);
      Q2QEe   = FitUtils::Q2QErec((event->PartInfo(j))->fP, cos(Thetae), 34.,true);
     
    } else if (PID != 2212 and
	       PID != 2112 and
	       PID != 22){
      bad_particle = true;
    }
  }
  
  this->X_VAR = Q2QEe;
  LOG(EVT) << "X_VAR = "<<X_VAR<<std::endl;
  return;
}



//********************************************************************
bool MINERvA_CC0pi_XSec_1DQ2_nue::isSignal(FitEvent *event){
//*******************************************************************

  // Only look at numu events
  //if ( nue_flag and (event->PartInfo(0)->fPID) != 12) return false;
  //if (!nue_flag and (event->PartInfo(0)->fPID) != -12) return false;

  // Get Nue/NueBar events
  if (fabs(event->PDGnu()) != 12)  return false;

  // ONLY CC0PI
  if (bad_particle) return false;
      
  // restrict energy range
  if (event->Enu()/1000.0 < this->EnuMin || event->Enu()/1000.0 > this->EnuMax) return false;

  if (Ee < 0.5) return false;
  
  return true;
};

