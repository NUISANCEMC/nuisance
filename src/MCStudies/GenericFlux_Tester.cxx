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

#include "GenericFlux_Tester.h"


// NOTE STILL VERY EARLY CLASS
// - DEFINITIONS of signal still need to be added
// - Pion different calculations need to be added

//******************************************************************** 
/// @brief Class to perform CCQE Fake Data Studies on a custom measurement
GenericFlux_Tester::GenericFlux_Tester(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){
//******************************************************************** 
  
  // Measurement Details
  measurementName = name;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 100000.; // Arbritrarily high energy limit

  // Set default fitter flags
  isDiag = true;
  isShape = false;
  isRawEvents = false;

  // This function will sort out the input files automatically and parse all the inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile); 

  // Setup dataHist as a placeholder
  this->dataHist = new TH1D(("empty_data"),("empty-data"),1,0,1);
  this->SetupDefaultHist();
  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  // Set TOPOLOGY DEFINITION
  if (name.find("CCQE") != std::string::npos) filltype = kCCQEFill;
  else if (name.find("MEC") != std::string::npos) filltype = kMECFill;
  else if (name.find("CCQEMEC") != std::string::npos) filltype = kCCQEMECFill;
  else if (name.find("CC0PI") != std::string::npos) filltype = kCC0PIFill;

  // Setup other histograms
  this->mcHist_1DQ2 = new TH1D( (measurementName + "_1DQ2").c_str(), 
				(measurementName + "_1DQ2;Q^{2}_{QE} [GeV^{2}c^{-2}];d#sigma/dQ^{2}_{QE} [cm^{2}GeV^{-2}c^{2}]").c_str(),
				30, 0.0, 3.0 );

  this->mcHist_1DTmu = new TH1D((measurementName + "_1DTmu").c_str(),
                                (measurementName + "_1DTmu;T_{#mu} [GeV];d#sigma/dT_{#mu} [cm^{2}GeV^{-1}]").c_str(),
				30, 0.0, 30.0 );
  
  this->mcHist_1DCosTheta = new TH1D((measurementName + "_1DCosTheta").c_str(),
				     (measurementName + "_1DCosTheta;cos#theta;d#sigma/dcos#theta [cm^{2}]").c_str(),
				     30, -1.0, 1.0 );
  
  this->mcHist_1DEnu = new TH1D((measurementName + "_1DEnu").c_str(),
				(measurementName + "_1DEnu;E_{#nu,true} [GeV];d#sigma/dE_{#nu} [cm^{2}GeV^{-1}]").c_str(),
				30, 0.0, 20.0 );
  
  this->mcHist_1DErec = new TH1D((measurementName + "_1DErec").c_str(),
				 (measurementName + "_1DErec;E_{#nu,rec.} [GeV];d#sigma/dE_{#nu} [cm^{2}GeV^{-1}]").c_str(),
				 30, 0.0, 20.0 );
  
  this->mcHist_1DEdiff = new TH1D((measurementName + "_1DEdiff").c_str(),
				  (measurementName + "_1DEdiff;E_{#nu,true} - E_{#nu,rec.} [GeV];d#sigma/dE_{#nu,diff} [cm^{2}GeV^{-1}]").c_str(),
				  60, -2.0, 2.0 );
  
  this->mcHist_2DTmuCosTheta = new TH2D((measurementName + "_2DTmuCosTheta").c_str(),
					(measurementName + "_2DTmuCosTheta;T_{#mu} [GeV];cos#theta;d^{2}#sigma/dT_{#mu}dcos#theta [cm^{2}GeV^{-1}]").c_str(),
					30, 0.0, 3.0,
					30, -1.0, 1.0);

  // 3. The generator is organised in SetupMeasurement so it gives the cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and divide by the number of neutrons 6.
  this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))  /this->TotalIntegratedFlux();
  
  // Set ANTINU Flag
  antinu = (measurementName.find("antinu") != std::string::npos);

  // Set Target Scalings and binding
  if (measurementName.find("CH") != std::string::npos){ // < ---- CH
    this->scaleFactor *= 13.0 / 6.0;
    if (!antinu) binding_E = 34.0;
    else binding_E = 30.0;
  }

  if (measurementName.find("H20") != std::string::npos){ // < ---- H2)
    this->scaleFactor *= 18.0 / 8.0;
    if (!antinu) binding_E = 36.0;
    else binding_E = 32.0;
  }

};



//******************************************************************** 
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void GenericFlux_Tester::FillEventVariables(FitEvent *event){
//******************************************************************** 

  // Define empty variables
  this->X_VAR = -1.0; // LEAVE AS EMPTY

  // Set Flags First ----------
  Mode = event->Mode;

  isCCQE = (Mode == 1);
  isCCQEMEC = (Mode == 1 or Mode == 2);
  isMEC = (Mode == 2);

  isRES1PI = (Mode > 16 && Mode < 21);
  isRESNPI = (Mode > 16 && Mode < 21);

  isCC0PI   = true;
  isCC1PI   = false;
  isCCOTHER = false;

  // Setup Variables to crap values
  Q2 = -999.9;
  CosTheta = -999.9;
  Tmu = -999.9;
  Enu = -999.9;
  Erec = -999.9;
  Ediff = -999.9;


  // Get Neutrino Energy and PDG
  Enu = event->PartInfo(0)->fP.E()/1000.0;
  int NuPDG = event->PartInfo(0)->fPID;

  // Loop over the particle stack and get lepton
  for (int j = 2; j < event->Npart(); ++j){
    
    int partPDG = (event->PartInfo(j))->fPID;

    // LEPTON Calculations
    if (abs(partPDG) == abs(NuPDG) - 1){
      
      double ThetaMu     = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(j))->fP.Vect());
      CosTheta = cos(ThetaMu);
      Tmu = FitUtils::T((event->PartInfo(j))->fP);
      
      // QE Calculations
      if (isCCQEMEC){
	Erec = FitUtils::EnuQErec((event->PartInfo(j))->fP, CosTheta, binding_E, !antinu);
	Ediff = Erec - Enu;
	
	Q2   = FitUtils::Q2QErec( (event->PartInfo(j))->fP, CosTheta, binding_E, !antinu);
      }
    }

    // isCC0PI FLAG
    if (partPDG == 211 or partPDG == -211) isCC0PI = false;
  }

  return;
};

//******************************************************************** 
/// @details Signal is true CCQE scattering
///
/// Everything is classed as signal...
bool GenericFlux_Tester::isSignal(FitEvent *event){
//******************************************************************** 

  return true;
};

//********************************************************************    
void GenericFlux_Tester::ScaleEvents(){
//********************************************************************    

  this->mcHist_1DQ2       ->Scale( scaleFactor, "width" );
  this->mcHist_1DTmu      ->Scale( scaleFactor, "width" );
  this->mcHist_1DCosTheta ->Scale( scaleFactor, "width" );
  //  this->mcHist_1DEnu      ->Scale( scaleFactor, "width" );
  //  this->mcHist_1DErec     ->Scale( scaleFactor, "width" );
  //  this->mcHist_1DEdiff    ->Scale( scaleFactor, "width" );

  this->mcHist_2DTmuCosTheta ->Scale( scaleFactor, "width" );

  return;
}

//********************************************************************  
void GenericFlux_Tester::ApplyNormScale(double norm){
//********************************************************************  

  double scaleF = 1.0;
  if (norm > 0.0) scaleF = 1.0/norm;
  else scaleF = 0.0;

  this->mcHist_1DQ2       ->Scale( scaleF );
  this->mcHist_1DTmu      ->Scale( scaleF );
  this->mcHist_1DCosTheta ->Scale( scaleF );
  this->mcHist_1DEnu      ->Scale( scaleF );
  this->mcHist_1DErec     ->Scale( scaleF );
  this->mcHist_1DEdiff    ->Scale( scaleF );

  this->mcHist_2DTmuCosTheta ->Scale( scaleF );

  return;
}

//********************************************************************  
void GenericFlux_Tester::FillHistograms(){
//********************************************************************  

  if ( (filltype == kCCQEFill and !isCCQE) or
       (filltype == kCCQEMECFill and !isCCQEMEC) or
       (filltype == kMECFill and !isMEC) )
    return;

  // Do FILLING
  this->mcHist_1DQ2       ->Fill( Q2,       Weight );
  this->mcHist_1DTmu      ->Fill( Tmu,      Weight );
  this->mcHist_1DCosTheta ->Fill( CosTheta, Weight );
  this->mcHist_1DEnu      ->Fill( Enu,      Weight );
  this->mcHist_1DErec     ->Fill( Erec,     Weight );
  this->mcHist_1DEdiff    ->Fill( Ediff,    Weight );

  this->mcHist_2DTmuCosTheta ->Fill( Tmu, CosTheta, Weight);

  return;
}

//******************************************************************** 
void GenericFlux_Tester::ResetAll(){
//******************************************************************** 

  this->mcHist_1DQ2->Reset();
  this->mcHist_1DTmu->Reset();
  this->mcHist_1DCosTheta->Reset();
  this->mcHist_1DEnu->Reset();
  this->mcHist_1DErec->Reset();
  this->mcHist_1DEdiff->Reset();
  this->mcHist_2DTmuCosTheta->Reset();

  return;
}

//********************************************************************  
void GenericFlux_Tester::Write(std::string drawOpt){
//********************************************************************  

  this->mcHist_1DQ2->Write();
  this->mcHist_1DTmu->Write();
  this->mcHist_1DCosTheta->Write();
  this->mcHist_1DEnu->Write();
  this->mcHist_1DErec->Write();
  this->mcHist_1DEdiff->Write();
  this->mcHist_2DTmuCosTheta->Write();
  this->fluxHist->Write();
  this->eventHist->Write();
  
  return;
}

//********************************************************************  
double GenericFlux_Tester::GetChi2(){
//********************************************************************  
  return 0.0;
}


