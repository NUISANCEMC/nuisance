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

#include "MiniBooNE_CCQE_XSec_2DTcos_antinu.h"

//******************************************************************** 
MiniBooNE_CCQE_XSec_2DTcos_antinu::MiniBooNE_CCQE_XSec_2DTcos_antinu(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details
  measurementName = name;
  plotTitles = "; Q^{2}_{QE} (GeV^{2}); d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})";
  EnuMin = 0.;
  EnuMax = 3.;
  isDiag = true;
  normError = 0.130;
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  this->plotTitles = "; T_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dT_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";
  this->ccqelike = name.find("CCQELike") != std::string::npos;

  // Define Bin Edges
  this->data_points_x = 19;
  this->data_points_y = 21;
  Double_t tempx[19] = { 0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
  Double_t tempy[21] = {-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  this->xBins = tempx;
  this->yBins = tempy;

  // Setup Data Plots
  if (!ccqelike){
    this->SetDataValues(std::string(std::getenv("NIWG_DATA"))+"/MiniBooNE/anti-ccqe/aski_con.txt", 1E-41, 
			std::string(std::getenv("NIWG_DATA"))+"/MiniBooNE/anti-ccqe/aski_err.txt", 1E-42);
  } else {
     this->SetDataValues(std::string(std::getenv("NIWG_DATA"))+"/MiniBooNE/anti-ccqe/aski_like.txt", 1E-41, 
			std::string(std::getenv("NIWG_DATA"))+"/MiniBooNE/anti-ccqe/aski_err.txt", 1E-42);
  }
  this->SetupDefaultHist();
  
  // Setup Covariances
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);
  isDiag = true;

   // Different generators require slightly different rescaling factors.
  if      (this->eventType == 0) this->scaleFactor = (this->eventHist->Integral("width")*1E-38/(nevents+0.))*14.08/8./this->TotalIntegratedFlux(); // NEUT
  else if (this->eventType == 1) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))*14.08/8./this->fluxHist->Integral(); // NUWRO
  else if (this->eventType == 5) this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))*14.08/8./this->fluxHist->Integral(); // GENIE

};

//******************************************************************** 
void  MiniBooNE_CCQE_XSec_2DTcos_antinu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  bad_particle = false;
  Ekmu = 0.0;
  costheta = 0.0;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    
    int PID = (event->PartInfo(j))->fPID;

    // Look for the outgoing muon
    if (PID == -13){
    
      // Now find the kinematic values and fill the histogram
      Ekmu     = (event->PartInfo(j))->fP.E()/1000 - 0.105658367;
      costheta = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));

      if (!ccqelike) break;
      continue;
    }

    // Check for bad particles;
    if (PID != 2112 and PID != 22  and \
	PID != 2212 and	PID != -13)      
	bad_particle = true;
      
  }

  // Set X Variables
  this->X_VAR = Ekmu;
  this->Y_VAR = costheta;
  
  return;
};

//******************************************************************** 
bool MiniBooNE_CCQE_XSec_2DTcos_antinu::isSignal(FitEvent *event){
//******************************************************************** 

  // For now, define as the true mode being CCQE or npnh
  if (!ccqelike && Mode != -1 && Mode != -2) return false;

  // If CCQELike Signal
  if (ccqelike and bad_particle) return false;

  // Only look at numu events
  if ((event->PartInfo(0))->fPID != -14) return false;

  // Restrict energy range
  if ((event->PartInfo(0))->fP.E() < this->EnuMin*1000 || (event->PartInfo(0))->fP.E() > this->EnuMax*1000) return false;

  return true;
};

