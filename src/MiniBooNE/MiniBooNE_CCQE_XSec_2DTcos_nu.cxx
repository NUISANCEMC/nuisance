/// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "MiniBooNE_CCQE_XSec_2DTcos_nu.h"

//******************************************************************** 
MiniBooNE_CCQE_XSec_2DTcos_nu::MiniBooNE_CCQE_XSec_2DTcos_nu(std::string name, std::string inputfile,
							     FitWeight *rw, std::string type,
							     std::string fakeDataFile){
//******************************************************************** 

  // Measurement Details
  fName = name;
  EnuMin = 0.;
  EnuMax = 3.;
  isDiag = true;
  fNormError = 0.107;
  default_types = "FIX/DIAG";
  allowed_types = "FIX,FREE,SHAPE/DIAG/NORM";
  Measurement2D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Setup Plots
  fPlotTitles = "; T_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dT_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";
  ccqelike = name.find("CCQELike") != std::string::npos;

  // Define Bin Edges
  data_points_x = 19;
  data_points_y = 21;
  Double_t tempx[19] = { 0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
  Double_t tempy[21] = {-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  xBins = tempx;
  yBins = tempy;

  // Setup Data Plots
  if (!ccqelike){
    SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_con.txt", 1E-41, 
		  FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_err.txt", 1E-42);
  } else {
     SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_like.txt", 1E-41, 
		   FitPar::GetDataBase()+"/MiniBooNE/ccqe/aski_err.txt",  1E-42);
  }
  SetupDefaultHist();
  
  // Setup Covariances
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fullcovar);
  isDiag = true;

   // Different generators require slightly different rescaling factors.
  fScaleFactor = (fEventHist->Integral("width")*1E-38/(nevents+0.))*14.08/6./TotalIntegratedFlux(); 
  
};

//******************************************************************** 
void  MiniBooNE_CCQE_XSec_2DTcos_nu::FillEventVariables(FitEvent *event){
//******************************************************************** 
  
  // Init
  Ekmu = -999.9;
  costheta = -999.9;

  // Loop over the particle stack
  for (UInt_t j = 2; j < event->Npart(); ++j){
    
    int PID = (event->PartInfo(j))->fPID;

    // Look for the outgoing muon
    if (PID == 13 or (ccqelike and PID == -13)){
    
      // Now find the kinematic values and fill the histogram
      Ekmu     = (event->PartInfo(j))->fP.E()/1000.0 - 0.105658367;
      costheta = cos(((event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect())));

      break;
    } 
  }

  // Set X and Y Variables
  fXVar = Ekmu;
  fYVar = costheta;

  return;
};

//******************************************************************** 
bool MiniBooNE_CCQE_XSec_2DTcos_nu::isSignal(FitEvent *event){
//******************************************************************** 

  bool signal = true;
  // 2 Different Signal Definitions
  if (ccqelike) signal = SignalDef::isMiniBooNE_CCQELike(event, EnuMin, EnuMax);
  else signal =  SignalDef::isMiniBooNE_CCQE(event, EnuMin, EnuMax);

  
  //if (signal) std::cout<<"Signal Values = "<<fXVar<<" "<<fYVar<<std::endl;
    
  return signal;
};

