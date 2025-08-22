// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
  fHist_NuMu_Enu = new TH1D("NuMu_Enu_MC", "NuMu_Enu_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NuMu_Enu_Modes = new MCStudies::OfficialNIWGStack("NuMu_Enu_MC_MODES",
      "NuMu_Enu_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMu_Enu);
  fHist_NuMu_Enu_Pions = new MCStudies::OfficialPionStack("NuMu_Enu_MC_PIONS",
      "NuMu_Enu_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMu_Enu);

  fHist_NuMu_EnuRates = new TH1D("NuMu_EnuRates_MC", "NuMu_EnuRates_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NuMu_EnuRates_Modes = new MCStudies::OfficialNIWGStack("NuMu_EnuRates_MC_MODES",
      "NuMu_EnuRates_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMu_Enu);
  fHist_NuMu_EnuRates_Pions = new MCStudies::OfficialPionStack("NuMu_EnuRates_MC_PIONS",
      "NuMu_EnuRates_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMu_Enu);

  fHist_NuMu_Q2 = new TH1D("NuMu_Q2_MC", "NuMu_Q2_MC;Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})", 45, 0.0, 3.0);
  fHist_NuMu_Q2_Modes = new MCStudies::OfficialNIWGStack("NuMu_Q2_MC_MODES",
      "NuMu_Q2_MC_MODES;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NuMu_Q2);
  fHist_NuMu_Q2_Pions = new MCStudies::OfficialPionStack("NuMu_Q2_MC_PIONS",
      "NuMu_Q2_MC_PIONS;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NuMu_Q2);

  fHist_NuMu_Pmu = new TH1D("NuMu_Pmu_MC", "NuMu_Pmu_MC;P_{#mu} (GeV);#sigma (cm^{2}/nucleon/GeV)", 45, 0.0, 3.0);
  fHist_NuMu_Pmu_Modes = new MCStudies::OfficialNIWGStack("NuMu_Pmu_MC_MODES",
      "NuMu_Pmu_MC_MODES;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NuMu_Pmu);
  fHist_NuMu_Pmu_Pions = new MCStudies::OfficialPionStack("NuMu_Pmu_MC_PIONS",
      "NuMu_Pmu_MC_PIONS;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NuMu_Pmu);

  fHist_NuMu_Cosmu = new TH1D("NuMu_Cosmu_MC", "NuMu_Cosmu_MC;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)", 45, -1.0, 1.0);
  fHist_NuMu_Cosmu_Modes = new MCStudies::OfficialNIWGStack("NuMu_Cosmu_MC_MODES",
      "NuMu_Cosmu_MC_MODES;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NuMu_Cosmu);
  fHist_NuMu_Cosmu_Pions = new MCStudies::OfficialPionStack("NuMu_Cosmu_MC_PIONS",
      "NuMu_Cosmu_MC_PIONS;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NuMu_Cosmu);


  fHist_NuMuBar_Enu = new TH1D("NuMuBar_Enu_MC", "NuMuBar_Enu_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NuMuBar_Enu_Modes = new MCStudies::OfficialNIWGStack("NuMuBar_Enu_MC_MODES",
      "NuMuBar_Enu_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMuBar_Enu);
  fHist_NuMuBar_Enu_Pions = new MCStudies::OfficialPionStack("NuMuBar_Enu_MC_PIONS",
      "NuMuBar_Enu_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMuBar_Enu);

  fHist_NuMuBar_EnuRates = new TH1D("NuMuBar_EnuRates_MC", "NuMuBar_EnuRates_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NuMuBar_EnuRates_Modes = new MCStudies::OfficialNIWGStack("NuMuBar_EnuRates_MC_MODES",
      "NuMuBar_EnuRates_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMuBar_Enu);
  fHist_NuMuBar_EnuRates_Pions = new MCStudies::OfficialPionStack("NuMuBar_EnuRates_MC_PIONS",
      "NuMuBar_EnuRates_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NuMuBar_Enu);

  fHist_NuMuBar_Q2 = new TH1D("NuMuBar_Q2_MC", "NuMuBar_Q2_MC;Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})", 45, 0.0, 3.0);
  fHist_NuMuBar_Q2_Modes = new MCStudies::OfficialNIWGStack("NuMuBar_Q2_MC_MODES",
      "NuMuBar_Q2_MC_MODES;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NuMuBar_Q2);
  fHist_NuMuBar_Q2_Pions = new MCStudies::OfficialPionStack("NuMuBar_Q2_MC_PIONS",
      "NuMuBar_Q2_MC_PIONS;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NuMuBar_Q2);

  fHist_NuMuBar_Pmu = new TH1D("NuMuBar_Pmu_MC", "NuMuBar_Pmu_MC;P_{#mu} (GeV);#sigma (cm^{2}/nucleon/GeV)", 45, 0.0, 3.0);
  fHist_NuMuBar_Pmu_Modes = new MCStudies::OfficialNIWGStack("NuMuBar_Pmu_MC_MODES",
      "NuMuBar_Pmu_MC_MODES;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NuMuBar_Pmu);
  fHist_NuMuBar_Pmu_Pions = new MCStudies::OfficialPionStack("NuMuBar_Pmu_MC_PIONS",
      "NuMuBar_Pmu_MC_PIONS;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NuMuBar_Pmu);

  fHist_NuMuBar_Cosmu = new TH1D("NuMuBar_Cosmu_MC", "NuMuBar_Cosmu_MC;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)", 45, -1.0, 1.0);
  fHist_NuMuBar_Cosmu_Modes = new MCStudies::OfficialNIWGStack("NuMuBar_Cosmu_MC_MODES",
      "NuMuBar_Cosmu_MC_MODES;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NuMuBar_Cosmu);
  fHist_NuMuBar_Cosmu_Pions = new MCStudies::OfficialPionStack("NuMuBar_Cosmu_MC_PIONS",
      "NuMuBar_Cosmu_MC_PIONS;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NuMuBar_Cosmu);


  fHist_Nue_Enu = new TH1D("Nue_Enu_MC", "Nue_Enu_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_Nue_Enu_Modes = new MCStudies::OfficialNIWGStack("Nue_Enu_MC_MODES",
      "Nue_Enu_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_Nue_Enu);
  fHist_Nue_Enu_Pions = new MCStudies::OfficialPionStack("Nue_Enu_MC_PIONS",
      "Nue_Enu_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_Nue_Enu);

  fHist_Nue_EnuRates = new TH1D("Nue_EnuRates_MC", "Nue_EnuRates_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_Nue_EnuRates_Modes = new MCStudies::OfficialNIWGStack("Nue_EnuRates_MC_MODES",
      "Nue_EnuRates_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_Nue_Enu);
  fHist_Nue_EnuRates_Pions = new MCStudies::OfficialPionStack("Nue_EnuRates_MC_PIONS",
      "Nue_EnuRates_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_Nue_Enu);

  fHist_Nue_Q2 = new TH1D("Nue_Q2_MC", "Nue_Q2_MC;Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})", 45, 0.0, 3.0);
  fHist_Nue_Q2_Modes = new MCStudies::OfficialNIWGStack("Nue_Q2_MC_MODES",
      "Nue_Q2_MC_MODES;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_Nue_Q2);
  fHist_Nue_Q2_Pions = new MCStudies::OfficialPionStack("Nue_Q2_MC_PIONS",
      "Nue_Q2_MC_PIONS;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_Nue_Q2);

  fHist_Nue_Pmu = new TH1D("Nue_Pmu_MC", "Nue_Pmu_MC;P_{#mu} (GeV);#sigma (cm^{2}/nucleon/GeV)", 45, 0.0, 3.0);
  fHist_Nue_Pmu_Modes = new MCStudies::OfficialNIWGStack("Nue_Pmu_MC_MODES",
      "Nue_Pmu_MC_MODES;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_Nue_Pmu);
  fHist_Nue_Pmu_Pions = new MCStudies::OfficialPionStack("Nue_Pmu_MC_PIONS",
      "Nue_Pmu_MC_PIONS;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_Nue_Pmu);

  fHist_Nue_Cosmu = new TH1D("Nue_Cosmu_MC", "Nue_Cosmu_MC;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)", 45, -1.0, 1.0);
  fHist_Nue_Cosmu_Modes = new MCStudies::OfficialNIWGStack("Nue_Cosmu_MC_MODES",
      "Nue_Cosmu_MC_MODES;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_Nue_Cosmu);
  fHist_Nue_Cosmu_Pions = new MCStudies::OfficialPionStack("Nue_Cosmu_MC_PIONS",
      "Nue_Cosmu_MC_PIONS;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_Nue_Cosmu);


  fHist_NueBar_Enu = new TH1D("NueBar_Enu_MC", "NueBar_Enu_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NueBar_Enu_Modes = new MCStudies::OfficialNIWGStack("NueBar_Enu_MC_MODES",
      "NueBar_Enu_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NueBar_Enu);
  fHist_NueBar_Enu_Pions = new MCStudies::OfficialPionStack("NueBar_Enu_MC_PIONS",
      "NueBar_Enu_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NueBar_Enu);

  fHist_NueBar_EnuRates = new TH1D("NueBar_EnuRates_MC", "NueBar_EnuRates_MC;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)", 45, 0.0, 3.0);
  fHist_NueBar_EnuRates_Modes = new MCStudies::OfficialNIWGStack("NueBar_EnuRates_MC_MODES",
      "NueBar_EnuRates_MC_MODES;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NueBar_Enu);
  fHist_NueBar_EnuRates_Pions = new MCStudies::OfficialPionStack("NueBar_EnuRates_MC_PIONS",
      "NueBar_EnuRates_MC_PIONS;E_{#nu}^{True} (GeV);#sigma (cm^{2}/nucleon)",
      fHist_NueBar_Enu);

  fHist_NueBar_Q2 = new TH1D("NueBar_Q2_MC", "NueBar_Q2_MC;Q^{2} (GeV);d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})", 45, 0.0, 3.0);
  fHist_NueBar_Q2_Modes = new MCStudies::OfficialNIWGStack("NueBar_Q2_MC_MODES",
      "NueBar_Q2_MC_MODES;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NueBar_Q2);
  fHist_NueBar_Q2_Pions = new MCStudies::OfficialPionStack("NueBar_Q2_MC_PIONS",
      "NueBar_Q2_MC_PIONS;Q^{2} (GeV);#sigma (cm^{2}/nucleon/GeV^{2})",
      fHist_NueBar_Q2);

  fHist_NueBar_Pmu = new TH1D("NueBar_Pmu_MC", "NueBar_Pmu_MC;P_{#mu} (GeV);#sigma (cm^{2}/nucleon/GeV)", 45, 0.0, 3.0);
  fHist_NueBar_Pmu_Modes = new MCStudies::OfficialNIWGStack("NueBar_Pmu_MC_MODES",
      "NueBar_Pmu_MC_MODES;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NueBar_Pmu);
  fHist_NueBar_Pmu_Pions = new MCStudies::OfficialPionStack("NueBar_Pmu_MC_PIONS",
      "NueBar_Pmu_MC_PIONS;P_{#mu} (GeV);d#sigma/dP_{#mu} (cm^{2}/nucleon/GeV)",
      fHist_NueBar_Pmu);

  fHist_NueBar_Cosmu = new TH1D("NueBar_Cosmu_MC", "NueBar_Cosmu_MC;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)", 45, -1.0, 1.0);
  fHist_NueBar_Cosmu_Modes = new MCStudies::OfficialNIWGStack("NueBar_Cosmu_MC_MODES",
      "NueBar_Cosmu_MC_MODES;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NueBar_Cosmu);
  fHist_NueBar_Cosmu_Pions = new MCStudies::OfficialPionStack("NueBar_Cosmu_MC_PIONS",
      "NueBar_Cosmu_MC_PIONS;cos#theta_{#mu};d#sigma/dcos#theta_{#mu} (cm^{2}/nucleon)",
      fHist_NueBar_Cosmu);



  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};




//********************************************************************
void OfficialNIWGPlots::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (!event->GetNeutrinoIn() or abs(event->Mode) > 30) return;
  TLorentzVector vectnu = event->GetNeutrinoIn()->fP;
  int leptons[] = {12,-12,14,-14};
  if (!event->GetHMFSParticle(leptons))return;
  TLorentzVector vectlep = event->GetHMFSParticle(leptons)->fP;

  double Q2 = fabs((vectlep - vectnu).Mag2()) / 1.E6;
  double Enu = vectnu.E() / 1.E3;
  double Pmu = vectlep.Vect().Mag() / 1.E3;
  double Cosmu = cos(vectlep.Vect().Angle(vectnu.Vect()));

  bool nue = (abs(event->GetNeutrinoIn()->fPID) == 12);
  bool nubar = (event->GetNeutrinoIn()->fPID > 0);

  if (OfficialNIWGPlots::isSignal(event)) {

    if (!nue and !nubar) {
      fHist_NuMu_Enu->Fill(Enu, Weight);
      fHist_NuMu_Enu_Modes->Fill(event, Enu, Weight);
      fHist_NuMu_Enu_Pions->Fill(event, Enu, Weight);

      fHist_NuMu_EnuRates->Fill(Enu, Weight);
      fHist_NuMu_EnuRates_Modes->Fill(event, Enu, Weight);
      fHist_NuMu_EnuRates_Pions->Fill(event, Enu, Weight);

      fHist_NuMu_Q2->Fill(Q2, Weight);
      fHist_NuMu_Q2_Modes->Fill(event, Q2, Weight);
      fHist_NuMu_Q2_Pions->Fill(event, Q2, Weight);

      fHist_NuMu_Pmu->Fill(Pmu, Weight);
      fHist_NuMu_Pmu_Modes->Fill(event, Pmu, Weight);
      fHist_NuMu_Pmu_Pions->Fill(event, Pmu, Weight);

      fHist_NuMu_Cosmu->Fill(Cosmu, Weight);
      fHist_NuMu_Cosmu_Modes->Fill(event, Cosmu, Weight);
      fHist_NuMu_Cosmu_Pions->Fill(event, Cosmu, Weight);
    } else if (!nue and nubar){
      fHist_NuMuBar_Enu->Fill(Enu, Weight);
      fHist_NuMuBar_Enu_Modes->Fill(event, Enu, Weight);
      fHist_NuMuBar_Enu_Pions->Fill(event, Enu, Weight);

      fHist_NuMuBar_EnuRates->Fill(Enu, Weight);
      fHist_NuMuBar_EnuRates_Modes->Fill(event, Enu, Weight);
      fHist_NuMuBar_EnuRates_Pions->Fill(event, Enu, Weight);

      fHist_NuMuBar_Q2->Fill(Q2, Weight);
      fHist_NuMuBar_Q2_Modes->Fill(event, Q2, Weight);
      fHist_NuMuBar_Q2_Pions->Fill(event, Q2, Weight);

      fHist_NuMuBar_Pmu->Fill(Pmu, Weight);
      fHist_NuMuBar_Pmu_Modes->Fill(event, Pmu, Weight);
      fHist_NuMuBar_Pmu_Pions->Fill(event, Pmu, Weight);

      fHist_NuMuBar_Cosmu->Fill(Cosmu, Weight);
      fHist_NuMuBar_Cosmu_Modes->Fill(event, Cosmu, Weight);
      fHist_NuMuBar_Cosmu_Pions->Fill(event, Cosmu, Weight);	
    } else if (nue and !nubar){
      fHist_Nue_Enu->Fill(Enu, Weight);
      fHist_Nue_Enu_Modes->Fill(event, Enu, Weight);
      fHist_Nue_Enu_Pions->Fill(event, Enu, Weight);

      fHist_Nue_EnuRates->Fill(Enu, Weight);
      fHist_Nue_EnuRates_Modes->Fill(event, Enu, Weight);
      fHist_Nue_EnuRates_Pions->Fill(event, Enu, Weight);

      fHist_Nue_Q2->Fill(Q2, Weight);
      fHist_Nue_Q2_Modes->Fill(event, Q2, Weight);
      fHist_Nue_Q2_Pions->Fill(event, Q2, Weight);

      fHist_Nue_Pmu->Fill(Pmu, Weight);
      fHist_Nue_Pmu_Modes->Fill(event, Pmu, Weight);
      fHist_Nue_Pmu_Pions->Fill(event, Pmu, Weight);

      fHist_Nue_Cosmu->Fill(Cosmu, Weight);
      fHist_Nue_Cosmu_Modes->Fill(event, Cosmu, Weight);
      fHist_Nue_Cosmu_Pions->Fill(event, Cosmu, Weight);			
    } else if (nue and nubar){
      fHist_NueBar_Enu->Fill(Enu, Weight);
      fHist_NueBar_Enu_Modes->Fill(event, Enu, Weight);
      fHist_NueBar_Enu_Pions->Fill(event, Enu, Weight);

      fHist_NueBar_EnuRates->Fill(Enu, Weight);
      fHist_NueBar_EnuRates_Modes->Fill(event, Enu, Weight);
      fHist_NueBar_EnuRates_Pions->Fill(event, Enu, Weight);

      fHist_NueBar_Q2->Fill(Q2, Weight);
      fHist_NueBar_Q2_Modes->Fill(event, Q2, Weight);
      fHist_NueBar_Q2_Pions->Fill(event, Q2, Weight);

      fHist_NueBar_Pmu->Fill(Pmu, Weight);
      fHist_NueBar_Pmu_Modes->Fill(event, Pmu, Weight);
      fHist_NueBar_Pmu_Pions->Fill(event, Pmu, Weight);

      fHist_NueBar_Cosmu->Fill(Cosmu, Weight);
      fHist_NueBar_Cosmu_Modes->Fill(event, Cosmu, Weight);
      fHist_NueBar_Cosmu_Pions->Fill(event, Cosmu, Weight);	
    }
  }

  return;
};

//********************************************************************
void OfficialNIWGPlots::Write(std::string drawOpt) {
  //********************************************************************

  (void)drawOpt;
  NUIS_LOG(FIT,"Writing OfficialNIWGPlots ");

  fHist_NuMu_Enu->Write();
  fHist_NuMu_Enu_Modes->Write();
  fHist_NuMu_Enu_Pions->Write();

  fHist_NuMu_EnuRates->Write();
  fHist_NuMu_EnuRates_Modes->Write();
  fHist_NuMu_EnuRates_Pions->Write();

  fHist_NuMu_Q2->Write();
  fHist_NuMu_Q2_Modes->Write();
  fHist_NuMu_Q2_Pions->Write();

  fHist_NuMu_Pmu->Write();
  fHist_NuMu_Pmu_Modes->Write();
  fHist_NuMu_Pmu_Pions->Write();

  fHist_NuMu_Cosmu->Write();
  fHist_NuMu_Cosmu_Modes->Write();
  fHist_NuMu_Cosmu_Pions->Write();

  fHist_NuMuBar_Enu->Write();
  fHist_NuMuBar_Enu_Modes->Write();
  fHist_NuMuBar_Enu_Pions->Write();

  fHist_NuMuBar_EnuRates->Write();
  fHist_NuMuBar_EnuRates_Modes->Write();
  fHist_NuMuBar_EnuRates_Pions->Write();

  fHist_NuMuBar_Q2->Write();
  fHist_NuMuBar_Q2_Modes->Write();
  fHist_NuMuBar_Q2_Pions->Write();

  fHist_NuMuBar_Pmu->Write();
  fHist_NuMuBar_Pmu_Modes->Write();
  fHist_NuMuBar_Pmu_Pions->Write();

  fHist_NuMuBar_Cosmu->Write();
  fHist_NuMuBar_Cosmu_Modes->Write();
  fHist_NuMuBar_Cosmu_Pions->Write();

  fHist_Nue_Enu->Write();
  fHist_Nue_Enu_Modes->Write();
  fHist_Nue_Enu_Pions->Write();

  fHist_Nue_EnuRates->Write();
  fHist_Nue_EnuRates_Modes->Write();
  fHist_Nue_EnuRates_Pions->Write();

  fHist_Nue_Q2->Write();
  fHist_Nue_Q2_Modes->Write();
  fHist_Nue_Q2_Pions->Write();

  fHist_Nue_Pmu->Write();
  fHist_Nue_Pmu_Modes->Write();
  fHist_Nue_Pmu_Pions->Write();

  fHist_Nue_Cosmu->Write();
  fHist_Nue_Cosmu_Modes->Write();
  fHist_Nue_Cosmu_Pions->Write();

  fHist_NueBar_Enu->Write();
  fHist_NueBar_Enu_Modes->Write();
  fHist_NueBar_Enu_Pions->Write();

  fHist_NueBar_EnuRates->Write();
  fHist_NueBar_EnuRates_Modes->Write();
  fHist_NueBar_EnuRates_Pions->Write();

  fHist_NueBar_Q2->Write();
  fHist_NueBar_Q2_Modes->Write();
  fHist_NueBar_Q2_Pions->Write();

  fHist_NueBar_Pmu->Write();
  fHist_NueBar_Pmu_Modes->Write();
  fHist_NueBar_Pmu_Pions->Write();

  fHist_NueBar_Cosmu->Write();
  fHist_NueBar_Cosmu_Modes->Write();
  fHist_NueBar_Cosmu_Pions->Write();

  return;
}

//********************************************************************
void OfficialNIWGPlots::ResetAll() {
  //********************************************************************

  fHist_NuMu_Enu->Reset();
  fHist_NuMu_Enu_Modes->Reset();
  fHist_NuMu_Enu_Pions->Reset();

  fHist_NuMu_EnuRates->Reset();
  fHist_NuMu_EnuRates_Modes->Reset();
  fHist_NuMu_EnuRates_Pions->Reset();

  fHist_NuMu_Q2->Reset();
  fHist_NuMu_Q2_Modes->Reset();
  fHist_NuMu_Q2_Pions->Reset();

  fHist_NuMu_Pmu->Reset();
  fHist_NuMu_Pmu_Modes->Reset();
  fHist_NuMu_Pmu_Pions->Reset();

  fHist_NuMu_Cosmu->Reset();
  fHist_NuMu_Cosmu_Modes->Reset();
  fHist_NuMu_Cosmu_Pions->Reset();


  fHist_NuMuBar_Enu->Reset();
  fHist_NuMuBar_Enu_Modes->Reset();
  fHist_NuMuBar_Enu_Pions->Reset();

  fHist_NuMuBar_EnuRates->Reset();
  fHist_NuMuBar_EnuRates_Modes->Reset();
  fHist_NuMuBar_EnuRates_Pions->Reset();

  fHist_NuMuBar_Q2->Reset();
  fHist_NuMuBar_Q2_Modes->Reset();
  fHist_NuMuBar_Q2_Pions->Reset();

  fHist_NuMuBar_Pmu->Reset();
  fHist_NuMuBar_Pmu_Modes->Reset();
  fHist_NuMuBar_Pmu_Pions->Reset();

  fHist_NuMuBar_Cosmu->Reset();
  fHist_NuMuBar_Cosmu_Modes->Reset();
  fHist_NuMuBar_Cosmu_Pions->Reset();

  fHist_Nue_Enu->Reset();
  fHist_Nue_Enu_Modes->Reset();
  fHist_Nue_Enu_Pions->Reset();

  fHist_Nue_EnuRates->Reset();
  fHist_Nue_EnuRates_Modes->Reset();
  fHist_Nue_EnuRates_Pions->Reset();

  fHist_Nue_Q2->Reset();
  fHist_Nue_Q2_Modes->Reset();
  fHist_Nue_Q2_Pions->Reset();

  fHist_Nue_Pmu->Reset();
  fHist_Nue_Pmu_Modes->Reset();
  fHist_Nue_Pmu_Pions->Reset();

  fHist_Nue_Cosmu->Reset();
  fHist_Nue_Cosmu_Modes->Reset();
  fHist_Nue_Cosmu_Pions->Reset();


  fHist_NueBar_Enu->Reset();
  fHist_NueBar_Enu_Modes->Reset();
  fHist_NueBar_Enu_Pions->Reset();

  fHist_NueBar_EnuRates->Reset();
  fHist_NueBar_EnuRates_Modes->Reset();
  fHist_NueBar_EnuRates_Pions->Reset();

  fHist_NueBar_Q2->Reset();
  fHist_NueBar_Q2_Modes->Reset();
  fHist_NueBar_Q2_Pions->Reset();

  fHist_NueBar_Pmu->Reset();
  fHist_NueBar_Pmu_Modes->Reset();
  fHist_NueBar_Pmu_Pions->Reset();

  fHist_NueBar_Cosmu->Reset();
  fHist_NueBar_Cosmu_Modes->Reset();
  fHist_NueBar_Cosmu_Pions->Reset();

  return;
}


//********************************************************************
void OfficialNIWGPlots::ScaleEvents() {
  //********************************************************************

  fHist_NuMu_EnuRates->Scale(fScaleFactorEnuXSec);
  fHist_NuMu_EnuRates_Modes->Scale(fScaleFactorEnuXSec);
  fHist_NuMu_EnuRates_Pions->Scale(fScaleFactorEnuXSec);

  PlotUtils::FluxUnfoldedScaling(fHist_NuMu_Enu, GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NuMu_Enu_Modes->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NuMu_Enu_Pions->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);

  fHist_NuMu_Q2->Scale(fScaleFactor, "width");
  fHist_NuMu_Q2_Modes->Scale(fScaleFactor, "width");
  fHist_NuMu_Q2_Pions->Scale(fScaleFactor, "width");

  fHist_NuMu_Pmu->Scale(fScaleFactor, "width");
  fHist_NuMu_Pmu_Modes->Scale(fScaleFactor, "width");
  fHist_NuMu_Pmu_Pions->Scale(fScaleFactor, "width");

  fHist_NuMu_Cosmu->Scale(fScaleFactor, "width");
  fHist_NuMu_Cosmu_Modes->Scale(fScaleFactor, "width");
  fHist_NuMu_Cosmu_Pions->Scale(fScaleFactor, "width");

  fHist_NuMu_EnuRates->Scale(fScaleFactorEnuXSec);
  fHist_NuMu_EnuRates_Modes->Scale(fScaleFactorEnuXSec);
  fHist_NuMu_EnuRates_Pions->Scale(fScaleFactorEnuXSec);

  PlotUtils::FluxUnfoldedScaling(fHist_NuMuBar_Enu, GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NuMuBar_Enu_Modes->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NuMuBar_Enu_Pions->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);

  fHist_NuMuBar_Q2->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Q2_Modes->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Q2_Pions->Scale(fScaleFactor, "width");

  fHist_NuMuBar_Pmu->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Pmu_Modes->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Pmu_Pions->Scale(fScaleFactor, "width");

  fHist_NuMuBar_Cosmu->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Cosmu_Modes->Scale(fScaleFactor, "width");
  fHist_NuMuBar_Cosmu_Pions->Scale(fScaleFactor, "width");


  fHist_Nue_EnuRates->Scale(fScaleFactorEnuXSec);
  fHist_Nue_EnuRates_Modes->Scale(fScaleFactorEnuXSec);
  fHist_Nue_EnuRates_Pions->Scale(fScaleFactorEnuXSec);

  PlotUtils::FluxUnfoldedScaling(fHist_Nue_Enu, GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_Nue_Enu_Modes->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_Nue_Enu_Pions->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);

  fHist_Nue_Q2->Scale(fScaleFactor, "width");
  fHist_Nue_Q2_Modes->Scale(fScaleFactor, "width");
  fHist_Nue_Q2_Pions->Scale(fScaleFactor, "width");

  fHist_Nue_Pmu->Scale(fScaleFactor, "width");
  fHist_Nue_Pmu_Modes->Scale(fScaleFactor, "width");
  fHist_Nue_Pmu_Pions->Scale(fScaleFactor, "width");

  fHist_Nue_Cosmu->Scale(fScaleFactor, "width");
  fHist_Nue_Cosmu_Modes->Scale(fScaleFactor, "width");
  fHist_Nue_Cosmu_Pions->Scale(fScaleFactor, "width");

  fHist_Nue_EnuRates->Scale(fScaleFactorEnuXSec);
  fHist_Nue_EnuRates_Modes->Scale(fScaleFactorEnuXSec);
  fHist_Nue_EnuRates_Pions->Scale(fScaleFactorEnuXSec);

  PlotUtils::FluxUnfoldedScaling(fHist_NueBar_Enu, GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NueBar_Enu_Modes->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);
  fHist_NueBar_Enu_Pions->FluxUnfold(GetFluxHistogram(), GetEventHistogram(), fScaleFactorEnuXSec, fNEvents);

  fHist_NueBar_Q2->Scale(fScaleFactor, "width");
  fHist_NueBar_Q2_Modes->Scale(fScaleFactor, "width");
  fHist_NueBar_Q2_Pions->Scale(fScaleFactor, "width");

  fHist_NueBar_Pmu->Scale(fScaleFactor, "width");
  fHist_NueBar_Pmu_Modes->Scale(fScaleFactor, "width");
  fHist_NueBar_Pmu_Pions->Scale(fScaleFactor, "width");

  fHist_NueBar_Cosmu->Scale(fScaleFactor, "width");
  fHist_NueBar_Cosmu_Modes->Scale(fScaleFactor, "width");
  fHist_NueBar_Cosmu_Pions->Scale(fScaleFactor, "width");

  return;
}



//********************************************************************
/// Select only events with final state Muons
bool OfficialNIWGPlots::isSignal(FitEvent *event) {
  //********************************************************************

  if (abs(event->Mode) > 30) return false;

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
