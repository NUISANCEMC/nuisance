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

#include "OfficialNIWGPlots.h"

#include "T2K_SignalDef.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
OfficialNIWGPlots::OfficialNIWGPlots(nuiskey samplekey) {
//********************************************************************

	// Sample overview ---------------------------------------------------
	std::string descrip = "NIWG Official plots sample. \n" \
	                      "Target: Any \n" \
	                      "Flux: T2K Flux \n" \
	                      "Signal: CC Inclusive \n";

	// Setup common settings
	fSettings = LoadSampleSettings(samplekey);
	fSettings.SetDescription(descrip);
	fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
	fSettings.SetEnuRange(0.0, 30.0);
	fSettings.DefineAllowedTargets("C,H");
	fSettings.DefineAllowedSpecies("numu");
	fSettings.SetTitle("NIWG Official Plots 2017");
	fSettings.SetOnlyMC(1);
	FinaliseSampleSettings();

	// Scaling Setup ---------------------------------------------------
	// ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
	fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");
	fScaleFactorEnuXSec = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);
	fScaleFactorDifXSec = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

	// Plot Setup -------------------------------------------------------
	fHist_Enu = new TH1D("Enu_MC", "Enu_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
	fHist_Enu_Modes = new MCStudies::OfficialNIWGStack("Enu_MC_MODES",
	                  "Enu_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
	                  fHist_Enu);
	fHist_Enu_Pions = new MCStudies::OfficialPionStack("Enu_MC_PIONS",
							   "Enu_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
							   fHist_Enu);


	fHist_EnuRates = new TH1D("EnuRates_MC", "EnuRates_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
        fHist_EnuRates_Modes = new MCStudies::OfficialNIWGStack("EnuRates_MC_MODES",
								"EnuRates_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
								fHist_Enu);
	fHist_EnuRates_Pions = new MCStudies::OfficialPionStack("EnuRates_MC_PIONS",
                                                                "EnuRates_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
                                                                fHist_Enu);


	fHist_Q2 = new TH1D("Q2_MC", "Q2_MC;Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})", 45, 0.0, 3.0);
	fHist_Q2_Modes = new MCStudies::OfficialNIWGStack("Q2_MC_MODES",
	                  "Q2_MC_MODES;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
	                  fHist_Q2);
	fHist_Q2_Pions = new MCStudies::OfficialPionStack("Q2_MC_PIONS",
							  "Q2_MC_PIONS;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
							  fHist_Q2);


	fHist_Pmu = new TH1D("Pmu_MC", "Pmu_MC;P_{#mu} (GeV);#sigma (cm^{2}/nucleon/GeV)", 45, 0.0, 3.0);
	fHist_Pmu_Modes = new MCStudies::OfficialNIWGStack("Pmu_MC_MODES",
	                  "Pmu_MC_MODES;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
	                  fHist_Pmu);
	fHist_Pmu_Pions = new MCStudies::OfficialPionStack("Pmu_MC_PIONS",
							   "Pmu_MC_PIONS;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
							   fHist_Pmu);


	fHist_Cosmu = new TH1D("Cosmu_MC", "Cosmu_MC;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)", 45, -1.0, 1.0);
	fHist_Cosmu_Modes = new MCStudies::OfficialNIWGStack("Cosmu_MC_MODES",
	                  "Cosmu_MC_MODES;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
	                  fHist_Cosmu);
	fHist_Cosmu_Pions = new MCStudies::OfficialPionStack("Cosmu_MC_PIONS",
							     "Cosmu_MC_PIONS;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
							     fHist_Cosmu);




	// Final setup  ---------------------------------------------------
	FinaliseMeasurement();

};




//********************************************************************
void OfficialNIWGPlots::FillEventVariables(FitEvent *event) {
//********************************************************************

  if (!event->GetNeutrinoIn() or !event->GetHMFSParticle(PhysConst::pdg_muons)) return;
  TLorentzVector vectnu = event->GetNeutrinoIn()->fP;
  TLorentzVector vectmu = event->GetHMFSParticle(PhysConst::pdg_muons)->fP;

  double Q2 = fabs((vectmu - vectnu).Mag2())/1.E6;
  double Enu = vectnu.E()/1.E3;
  double Pmu = vectmu.Vect().Mag()/1.E3;
  double Cosmu = cos(vectmu.Vect().Angle(vectnu.Vect()));
  
  if (OfficialNIWGPlots::isSignal(event)) {
    fHist_Enu->Fill(Enu, Weight);
    fHist_Enu_Modes->Fill(event, Enu, Weight);
    fHist_Enu_Pions->Fill(event, Enu, Weight);

    fHist_EnuRates->Fill(Enu, Weight);
    fHist_EnuRates_Modes->Fill(event, Enu, Weight);
    fHist_EnuRates_Pions->Fill(event, Enu, Weight);

    fHist_Q2->Fill(Q2, Weight);
    fHist_Q2_Modes->Fill(event, Q2, Weight);
    fHist_Q2_Pions->Fill(event, Q2, Weight);

    fHist_Pmu->Fill(Pmu, Weight);
    fHist_Pmu_Modes->Fill(event, Pmu, Weight);
    fHist_Pmu_Pions->Fill(event, Pmu, Weight);

    fHist_Cosmu->Fill(Cosmu, Weight);
    fHist_Cosmu_Modes->Fill(event, Cosmu, Weight);
    fHist_Cosmu_Pions->Fill(event, Cosmu, Weight);
  }
  
  return;
};

//********************************************************************
void OfficialNIWGPlots::Write(std::string drawOpt) {
//********************************************************************

	LOG(FIT) << "Writing OfficialNIWGPlots " << std::endl;

	fHist_Enu->Write();
	fHist_Enu_Modes->Write();
	fHist_Enu_Pions->Write();

	fHist_EnuRates->Write();
	fHist_EnuRates_Modes->Write();
	fHist_EnuRates_Pions->Write();

	fHist_Q2->Write();
	fHist_Q2_Modes->Write();
	fHist_Q2_Pions->Write();

	fHist_Pmu->Write();
	fHist_Pmu_Modes->Write();
	fHist_Pmu_Pions->Write();

	fHist_Cosmu->Write();
	fHist_Cosmu_Modes->Write();
	fHist_Cosmu_Pions->Write();

	return;
}

//********************************************************************
void OfficialNIWGPlots::ResetAll() {
//********************************************************************

	fHist_Enu->Reset();
	fHist_Enu_Modes->Reset();
	fHist_Enu_Pions->Reset();

	fHist_EnuRates->Reset();
        fHist_EnuRates_Modes->Reset();
	fHist_EnuRates_Pions->Reset();

	fHist_Q2->Reset();
	fHist_Q2_Modes->Reset();
	fHist_Q2_Pions->Reset();

	fHist_Pmu->Reset();
	fHist_Pmu_Modes->Reset();
	fHist_Pmu_Pions->Reset();

	fHist_Cosmu->Reset();
	fHist_Cosmu_Modes->Reset();
	fHist_Cosmu_Pions->Reset();


	return;
}


//********************************************************************
void OfficialNIWGPlots::ScaleEvents() {
//********************************************************************

  fHist_EnuRates->Scale(fScaleFactorEnuXSec);
  fHist_EnuRates_Modes->Scale(fScaleFactorEnuXSec);
  fHist_EnuRates_Pions->Scale(fScaleFactorEnuXSec);

  PlotUtils::FluxUnfoldedScaling(fHist_Enu, GetFluxHistogram(),GetEventHistogram(),fScaleFactorEnuXSec, fNEvents);
  fHist_Enu_Modes->FluxUnfold(GetFluxHistogram(),GetEventHistogram(),fScaleFactorEnuXSec);
  fHist_Enu_Pions->FluxUnfold(GetFluxHistogram(),GetEventHistogram(),fScaleFactorEnuXSec);

  fHist_Q2->Scale(fScaleFactor, "width");
  fHist_Q2_Modes->Scale(fScaleFactor, "width");
  fHist_Q2_Pions->Scale(fScaleFactor, "width");

  fHist_Pmu->Scale(fScaleFactor, "width");
  fHist_Pmu_Modes->Scale(fScaleFactor, "width");
  fHist_Pmu_Pions->Scale(fScaleFactor, "width");

  fHist_Cosmu->Scale(fScaleFactor, "width");
  fHist_Cosmu_Modes->Scale(fScaleFactor, "width");
  fHist_Cosmu_Pions->Scale(fScaleFactor, "width");

 
  return;
}



//********************************************************************
/// Select only events with final state Muons
bool OfficialNIWGPlots::isSignal(FitEvent *event) {
//********************************************************************

	if (abs(event->Mode) > 30) return false;
	if (event->NumFSParticle(13) + event->NumFSParticle(-13) != 1) return false;

	// Do we want any other signal?
	return true;
};





/// Functions to deal with the SB mode stacks
MCStudies::OfficialNIWGStack::OfficialNIWGStack(std::string name, std::string title, TH1* hist) {
	fName = name;
	fTitle = title;

	AddMode(0, "CC0PI",      "CC-0#pi",  kRed,     2, 1001);
	AddMode(1, "CC1PI",      "CC-1#pi",  kBlue,  2, 1001);
	AddMode(2, "CCOther",    "CC-Other",  kMagenta, 2, 1001);
	AddMode(3, "CCCOH",      "CC-Coherent", kGreen, 2, 1001);
	AddMode(4, "OTHER",      "Other",    kYellow, 2, 1001);
	StackBase::SetupStack(hist);
};

int MCStudies::OfficialNIWGStack::ConvertModeToIndex(FitEvent* event) {

	// Other
  if (((event->NumFSParticle(13) + event->NumFSParticle(-13)) != 1)) return 4;

	// CC 0 PI
	if ((event->NumFSMesons() == 0)) return 0;

	// CC Coherent                                                                                                                                                                                                                     
        if (abs(event->Mode) == 16) return 3;

	// CC 1 PI
	if ((event->NumFSParticle(PhysConst::pdg_charged_pions)) == 1) return 1;

	//  CC Other
	return 2;
};

void MCStudies::OfficialNIWGStack::Fill(FitEvent* evt, double x, double y, double z, double weight) {
	StackBase::FillStack(this->ConvertModeToIndex(evt), x, y, z, weight);
};


/// Functions to deal with the SB mode stacks                                                                                                                                                                                               
MCStudies::OfficialPionStack::OfficialPionStack(std::string name, std::string title, TH1* hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "CC0PI",      "CC-0#pi",  kRed,     2, 1001);
  AddMode(1, "CC1PI",      "CC-1#pi",  kBlue,  2, 1001);
  AddMode(2, "CCNPI",      "CC-N#pi",  kGreen, 2, 1001);
  AddMode(3, "OTHER",      "Other",    kYellow, 2, 1001);
  StackBase::SetupStack(hist);
};

int MCStudies::OfficialPionStack::ConvertModeToIndex(FitEvent* event) {

  // Other                                                                                                                                                                                                                            
  if (((event->NumFSParticle(13) + event->NumFSParticle(-13)) != 1)) return 3;

  // CC 0 PI              
  int npi = (event->NumFSParticle(PhysConst::pdg_charged_pions));
  if (npi == 0) return 0;
  if (npi == 1) return 1;
  if (npi > 1) return 2;
  return 3;
};

void MCStudies::OfficialPionStack::Fill(FitEvent* evt, double x, double y, double z, double weight) {
  StackBase::FillStack(this->ConvertModeToIndex(evt), x, y, z, weight);
};
