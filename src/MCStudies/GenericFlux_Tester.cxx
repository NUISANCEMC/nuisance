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

//******************************************************************** 
/// @brief Class to perform MC Studies on a custom measurement
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

  // 1. The generator is organised in SetupMeasurement so it gives the cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and divide by the number of neutrons 6.
  this->scaleFactor = (this->eventHist->Integral()*1E-38/(nevents+0.))  /this->TotalIntegratedFlux();

  FitPar::Config().out->cd();

  // Setup the TTree to save everything
  eventVariables = new TTree( (this->measurementName).c_str(), (this->measurementName).c_str() );
  eventVariables->Branch("Enu_true",  &Enu_true, "Enu_true/D");
  eventVariables->Branch("Enu_QE",    &Enu_QE,   "Enu_QE/D"  );
  eventVariables->Branch("PDGnu",     &PDGnu,    "PDGnu/D"   );
  
  eventVariables->Branch("Q2_true",  &Q2_true,  "Q2_true/D" );
  eventVariables->Branch("Q2_QE",    &Q2_QE,    "Q2_QE/D"   );

  eventVariables->Branch("MLep",      &MLep,      "MLep/D"  );
  eventVariables->Branch("ELep",      &ELep,      "ELep/D"  );
  eventVariables->Branch("TLep",      &TLep,      "TLep/D"  );
  eventVariables->Branch("CosLep",    &CosLep,    "CosLep/D");
  eventVariables->Branch("PPr",       &PPr,       "PPr/D"   );
  eventVariables->Branch("CosPr",     &CosPr,     "CosPr/D" );

  eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/D");
  eventVariables->Branch("Weight"    , &Weight,     "Weight/D"    );

};



//******************************************************************** 
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void GenericFlux_Tester::FillEventVariables(FitEvent *event){
//******************************************************************** 

  // Function used to extract any variables of interest to the event

  // Initialise everything to a bad value
  PDGnu   = -999.9;
  PDGLep  = -999.9;

  Enu_true = Enu_QE = Q2_true = Q2_QE = TLep = -999.9;
  CosPr = CosLep = PPr = -999.9;
  
  double proton_highmom = 0.0;

  // Get main event variables
  TLorentzVector nu_4mom = event->PartInfo(0)->fP;
  Enu_true = nu_4mom.E();
  PDGnu    = event->PartInfo(0)->fPID;

  bool cc = (abs(event->Mode) < 30);

  // Start Particle Loop
  UInt_t npart = event->Npart();
  for (UInt_t i = 0; i < npart; i++){

    // Skip particles that weren't in the final state
    bool part_alive = event->PartInfo(i)->fIsAlive;
    if (!part_alive) continue;
    
    int PDGpart = event->PartInfo(i)->fPID;
    
    // Find the charged lepton
    if (cc){

      TLorentzVector part_4mom = event->PartInfo(i)->fP;

      if (PDGpart == PDGnu - 1){
	PDGLep = PDGpart;

	TLep = FitUtils::T(part_4mom) * 1000.0;
	PLep = (part_4mom.Vect().Mag());
	ELep = (part_4mom.E());
	MLep = (part_4mom.Mag());
	CosLep = cos(part_4mom.Vect().Angle( nu_4mom.Vect() ));
      }
    }

    // Find highest momentum proton
    if (PDGpart == 2212){
       TLorentzVector part_4mom = event->PartInfo(i)->fP;
       
       if (part_4mom.Vect().Mag() > proton_highmom){
	 proton_highmom = part_4mom.Vect().Mag();

	 PPr = (part_4mom.Vect().Mag());
	 EPr = (part_4mom.E());
	 MPr = (part_4mom.Mag());
	 CosPr = cos(part_4mom.Vect().Angle( nu_4mom.Vect() ));

       }
    } 
  }



  // Fill the eventVariables Tree
  eventVariables->Fill();
  return;
};

//********************************************************************      
void GenericFlux_Tester::Write(std::string drawOpt){
//********************************************************************     

  // First save the TTree
  eventVariables->Write();
  
  // Now save any default plots we want to draw
  TH1D* Enu_hist = new TH1D("Enu_hist","Enu_hist;E;Events", 40, 0.0, 20.0*1.E3);
  eventVariables->Draw("Enu_tree >> Enu_hist", "Weight");
  Enu_hist->Write();

  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  return;
}



// -------------------------------------------------------------------
// Purely MC Plot
// Following functions are just overrides to handle this
// -------------------------------------------------------------------
//********************************************************************      
/// Everything is classed as signal...                  
bool GenericFlux_Tester::isSignal(FitEvent *event){
  //********************************************************************      
  (void) event;
  return true;
};


//********************************************************************    
void GenericFlux_Tester::ScaleEvents(){
//********************************************************************    
  // Saving everything to a TTree so no scaling required
  return;
}

//********************************************************************  
void GenericFlux_Tester::ApplyNormScale(double norm){
//********************************************************************  

  // Saving everything to a TTree so no scaling required
  this->currentNorm = norm;
  return;
}

//********************************************************************  
void GenericFlux_Tester::FillHistograms(){
//********************************************************************  
  // No Histograms need filling........
  return;
}

//******************************************************************** 
void GenericFlux_Tester::ResetAll(){
//******************************************************************** 
  eventVariables->Reset();
  return;
}

//********************************************************************  
double GenericFlux_Tester::GetChi2(){
//********************************************************************  
  // No Likelihood to test, purely MC
  return 0.0;
}


