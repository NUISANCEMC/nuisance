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
  eventVariables = new TTree( (this->measurementName+"_VARS").c_str(), (this->measurementName+"_VARS").c_str() );
  eventVariables->Branch("Mode",      &Mode,     "Mode/I");
  eventVariables->Branch("Enu_true",  &Enu_true, "Enu_true/D");
  eventVariables->Branch("Enu_QE",    &Enu_QE,   "Enu_QE/D"  );
  eventVariables->Branch("PDGnu",     &PDGnu,    "PDGnu/D"   );
  
  eventVariables->Branch("Q2_true",  &Q2_true,  "Q2_true/D" );
  eventVariables->Branch("Q2_QE",    &Q2_QE,    "Q2_QE/D"   );
  eventVariables->Branch("q0_true",  &q0_true,  "q0_true/D"   );
  eventVariables->Branch("q3_true",  &q3_true,  "q3_true/D"   );

  eventVariables->Branch("MLep",      &MLep,      "MLep/D"  );
  eventVariables->Branch("ELep",      &ELep,      "ELep/D"  );
  eventVariables->Branch("TLep",      &TLep,      "TLep/D"  );
  eventVariables->Branch("CosLep",    &CosLep,    "CosLep/D");
  eventVariables->Branch("PPr",       &PPr,       "PPr/D"   );
  eventVariables->Branch("CosPr",     &CosPr,     "CosPr/D" );

  eventVariables->Branch("FluxWeight", &FluxWeight, "FluxWeight/D");
  eventVariables->Branch("Weight"    , &Weight,     "Weight/D"    );

  

  // Signal Definitions from SignalDef.cxx
  eventVariables->Branch("flagCCQE_full", &flagCCQE_full, "flagCCQE_full/O");
  eventVariables->Branch("flagCCQE_rest", &flagCCQE_rest, "flagCCQE_rest/O");
  eventVariables->Branch("flagCCQEBar_full", &flagCCQEBar_full, "flagCCQEBar_full/O");
  eventVariables->Branch("flagCCQEBar_rest", &flagCCQEBar_rest, "flagCCQEBar_rest/O");

  eventVariables->Branch("flagCC1pip_MiniBooNE", &flagCC1pip_MiniBooNE, "flagCC1pip_MiniBooNE/O");

  eventVariables->Branch("flagCC1pip_MINERvA_full", &flagCC1pip_MINERvA_full, "flagCC1pip_MINERvA_full/O");
  eventVariables->Branch("flagCC1pip_MINERvA_rest", &flagCC1pip_MINERvA_rest, "flagCC1pip_MINERvA_rest/O");
  eventVariables->Branch("flagCCNpip_MINERvA_full", &flagCCNpip_MINERvA_full, "flagCCNpip_MINERvA_full/O");
  eventVariables->Branch("flagCCNpip_MINERva_rest", &flagCCNpip_MINERva_rest, "flagCCNpip_MINERva_rest/O");

  eventVariables->Branch("flagCC1pip_T2K", &flagCC1pip_T2K, "flagCC1pip_T2K/O");

  eventVariables->Branch("flagCC1pi0_MiniBooNE", &flagCC1pi0_MiniBooNE, "flagCC1pi0_MiniBooNE/O");

  eventVariables->Branch("flagCC1pi0Bar_MINERvA", &flagCC1pi0Bar_MINERvA, "flagCC1pi0Bar_MINERvA/O");

  eventVariables->Branch("flagNC1pi0_MiniBooNE", &flagNC1pi0_MiniBooNE, "flagNC1pi0_MiniBooNE/O");
  eventVariables->Branch("flagNC1pi0Bar_MiniBooNE", &flagNC1pi0Bar_MiniBooNE, "flagNC1pi0Bar_MiniBooNE/O");

  eventVariables->Branch("flagCCcoh_MINERvA", &flagCCcoh_MINERvA, "flagCCcoh_MINERvA/O");
  eventVariables->Branch("flagCCcohBar_MINERvA", &flagCCcohBar_MINERvA, "flagCCcohBar_MINERvA/O");

  eventVariables->Branch("flagCCQEnumu_MINERvA_full", &flagCCQEnumu_MINERvA_full, "flagCCQEnumu_MINERvA_full/O");
  eventVariables->Branch("flagCCQEnumubar_MINERvA_full", &flagCCQEnumubar_MINERvA_full, "flagCCQEnumubar_MINERvA_full/O");
  eventVariables->Branch("flagCCQEnumu_MINERvA_rest", &flagCCQEnumu_MINERvA_rest, "flagCCQEnumu_MINERvA_rest/O");
  eventVariables->Branch("flagCCQEnumubar_MINERvA_rest", &flagCCQEnumubar_MINERvA_rest, "flagCCQEnumubar_MINERvA_rest/O");
  
  eventVariables->Branch("flagCCincLowRecoil_MINERvA",
			 &flagCCincLowRecoil_MINERvA, "flagCCincLowRecoil_MINERvA/O");
  eventVariables->Branch("flagCCincLowRecoil_MINERvA_reqhad",
			 &flagCCincLowRecoil_MINERvA_reqhad, "flagCCincLowRecoil_MINERvA_reqhad/O");
  
  eventVariables->Branch("flagCCQELike_MiniBooNE", &flagCCQELike_MiniBooNE, "flagCCQELike_MiniBooNE/O");
  eventVariables->Branch("flagCCQE_MiniBooNE", &flagCCQE_MiniBooNE, "flagCCQE_MiniBooNE/O");
  eventVariables->Branch("flagCCQEBar_MiniBooNE", &flagCCQEBar_MiniBooNE, "flagCCQEBar_MiniBooNE/O");
  
};



//******************************************************************** 
/// @details Extract Enu and totcrs from event assuming quasi-elastic scattering
void GenericFlux_Tester::FillEventVariables(FitEvent *event){
//******************************************************************** 

  // Fill Signal Variables
  FillSignalFlags(event);
  
  // Function used to extract any variables of interest to the event
  Mode = event->Mode;

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
	
	Q2_true = (part_4mom - nu_4mom).Mag2();

	double ThetaLep = (event->PartInfo(0))->fP.Vect().Angle((event->PartInfo(i))->fP.Vect());    
	
	// Q2 QE Assuming Carbon Input. Should change this to be dynamic soon.
	Q2_QE  = FitUtils::Q2QErec(  part_4mom,   cos(ThetaLep),  34.,  true) * 1000000.0;
	Enu_QE = FitUtils::EnuQErec( part_4mom,  cos(ThetaLep), 34., true) * 1000.0; 

	q0_true = (part_4mom - nu_4mom).E();
	q3_true = (part_4mom - nu_4mom).Vect().Mag();
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

  // Flux Weights
  FluxWeight = fluxHist->GetBinContent(fluxHist->FindBin(Enu))/fluxHist->Integral();

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
  eventVariables->Draw("Enu_true >> Enu_hist", "Weight");
  Enu_hist->Write();

  // Q2 Diff XSec per nucleon
  TH1D* Q2_hist = new TH1D("Q2_hist","Q2_hist;Q2;d#sigma/dQ^{2}", 40, 0.0, 2.0);
  eventVariables->Draw("Q2_true/1E6 >> Q2_hist", "Weight");
  Q2_hist->Scale(scaleFactor,"width"); // DiffXSec Scaling
  Q2_hist->Write();
  
  
  // Save Flux and Event Histograms too
  GetInput()->GetFluxHistogram()->Write();
  GetInput()->GetEventHistogram()->Write();

  return;
}

//********************************************************************   
void GenericFlux_Tester::FillSignalFlags(FitEvent *event){
//********************************************************************

  
  flagCCQE_full = SignalDef::isCCQE(event, EnuMin, EnuMax, false); 
  flagCCQE_rest = SignalDef::isCCQE(event, EnuMin, EnuMax, true);
  flagCCQEBar_full = SignalDef::isCCQEBar(event, EnuMin, EnuMax, false);
  flagCCQEBar_rest = SignalDef::isCCQEBar(event, EnuMin, EnuMax, true);
  flagCC1pip_MiniBooNE = false;
  flagCC1pip_MINERvA_full = false;
  flagCC1pip_MINERvA_rest = false;
  flagCCNpip_MINERvA_full = false;
  flagCCNpip_MINERva_rest = false;
  flagCC1pip_T2K = false;
  flagCC1pi0_MiniBooNE = false;
  flagCC1pi0Bar_MINERvA = false;
  flagNC1pi0_MiniBooNE = false;
  flagNC1pi0Bar_MiniBooNE = false;
  flagCCcoh_MINERvA = false;
  flagCCcohBar_MINERvA = false;

  flagCCQEnumu_MINERvA_full    = SignalDef::isCCQEnumu_MINERvA(event, 1.5, 10.0, true);
  flagCCQEnumubar_MINERvA_full = SignalDef::isCCQEnumubar_MINERvA(event, 1.5, 10.0, true);
  flagCCQEnumu_MINERvA_rest    = SignalDef::isCCQEnumu_MINERvA(event, 1.5, 10.0, false);
  flagCCQEnumubar_MINERvA_rest = SignalDef::isCCQEnumubar_MINERvA(event, 1.5, 10.0, false);

  flagCCincLowRecoil_MINERvA = SignalDef::isCCincLowRecoil_MINERvA(event, 2.0, 6.0, false);
  flagCCincLowRecoil_MINERvA_reqhad = SignalDef::isCCincLowRecoil_MINERvA(event, 2.0, 6.0, true);
  
  flagCCQELike_MiniBooNE = SignalDef::isMiniBooNE_CCQELike(event, 0.0, 3.0);
  flagCCQE_MiniBooNE     = SignalDef::isMiniBooNE_CCQE(event, 0.0, 3.0);
  flagCCQEBar_MiniBooNE  = SignalDef::isMiniBooNE_CCQEBar(event, 0.0, 3.0);

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


