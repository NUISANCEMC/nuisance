#include "MINERvA_CC0pi_XSec_1DQ2_nu_proton.h"
#include <string>
#include <sstream>

// The constructor
MINERvA_CC0pi_XSec_1DQ2_nu_proton::MINERvA_CC0pi_XSec_1DQ2_nu_proton(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  // Check if this is a shape only fit  
  this->SetFitOptions(type);

  // Define the energy region
  this->EnuMin = 0.0;
  this->EnuMax = 100.0;

  // In future read most of these from a card file
  this->measurementName = "MINERvA_numu_1D_proton";
  this->plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/GeV^{2})";

  //  this->isShape = false;
  this->SetDataValues(FitPar::GetDataBase()+"/MINERvA/proton_Q2QE_nu_data.txt");
  this->SetCovarMatrixFromText(FitPar::GetDataBase()+"/MINERvA/proton_Q2QE_nu_covar.txt", 7);
  
  // Setup whole shit load of histograms
  this->mcHist = new TH1D((this->measurementName+"_MC").c_str(), (this->measurementName+this->plotTitles).c_str(), this->data_points-1, this->xBins);
  this->mcFine = new TH1D((this->measurementName+"_MC_FINE").c_str(), (this->measurementName+this->plotTitles).c_str(), 100, -2, 2);

  this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))/this->fluxHist->Integral(); // NEUT    
};


void MINERvA_CC0pi_XSec_1DQ2_nu_proton::FillEventVariables(FitEvent *event){

  double pE_highest;
  double weight = event->weight;
  // Reset Fill Variables
  Enu  = -999.9;
  CosThetaMu  = -999.0;
  Emu  = -999.0;
  Tmu  = -999.0;
  Q2mu  = -999.0;
  q3  = -999.0;
  Q2true  = -999.0;
  Tp  = -999.0;
  Ep  = -999.0;
  Q2p  = -999.0;
  Np   = 0;
  Nn   = 0;
  FSI  = 0;
  bad_particle = false;

  pT = 0.0;
  pE_highest = -999.9; // Very highland-esque way of doing things
  double pE_muon = -999.0;
  double pE_total = 0.0;
  muon_found = false;
  proton_found = false;
  int protonid = 0;
  int muonid = 0;

  Enu = event->PartInfo(0)->fP.E()/1000.0;
  this->Weight = weight;
  Mode = event->Mode;


   // Loop over the particle stack and find highest momentum FS proton
  for (int j = 0; j < event->Npart(); ++j){    
    if (!event->PartInfo(j)->fIsAlive) continue;
    if (event->PartInfo(j)->fPID == 2112){
      Nn++;
    }
    
    if (event->PartInfo(j)->fPID == 2212){
      Np++;
      
      // if momentum is higher than saved
      if (event->PartInfo(j)->fP.E() > pE_highest or pE_highest == -999.9) {
	pE_highest = event->PartInfo(j)->fP.E();
	Tp = event->PartInfo(j)->fP.E() - 938;
	Ep = event->PartInfo(j)->fP.E();
	protonid = j;
      }
      pE_total += event->PartInfo(j)->fP.E();
      
    } else if (event->PartInfo(j)->fPID == 13){
      
      pE_muon = FitUtils::Q2QErec((event->PartInfo(j))->fP,
				  cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()))), 34.);
      

      CosThetaMu = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));
      ThetaMu = ((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect()));
      Emu = (event->PartInfo(j))->fP.E();
      Tmu = (event->PartInfo(j))->fP.E() - 104;
      Q2mu = pE_muon;
      q3 = ((event->PartInfo(0))->fP.Vect() - (event->PartInfo(j))->fP.Vect()).Mag();
      Q2true = (((event->PartInfo(0))->fP) - ((event->PartInfo(j))->fP)).Mag()/1000.0;
      muonid = j;
    } else if (event->PartInfo(j)->fPID != 13 and
	       event->PartInfo(j)->fPID != 22 and
	       event->PartInfo(j)->fPID != 2112 and
	       event->PartInfo(j)->fPID != 2212){
      bad_particle = true;
    }
  }

  pT = pE_highest -  0.93827203;
  if (pE_highest > 0.110) proton_found = true;
  if (ThetaMu < 1.22173) muon_found = false;

  double q2qe    = (FitUtils::ProtonQ2QErec(pE_highest,34.));  
  if (muon_found and proton_found ) 
    Q2p = q2qe;
  else 
    Q2p = -999.0;
  Pmuon = pE_muon;
  
  Coplanar = (event->PartInfo(protonid))->fP.Vect().Angle(((event->PartInfo(muonid))->fP.Vect()));

  return;
};


bool MINERvA_CC0pi_XSec_1DQ2_nu_proton::isSignal(FitEvent *event){

  // Look for the outgoing proton in the final state.
  if (bad_particle) return false;

  if (!proton_found or !muon_found) return false;

  // Restrict energy range
  if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;

  return true;
};

