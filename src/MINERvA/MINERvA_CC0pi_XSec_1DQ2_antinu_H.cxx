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
/*
  Author :  Clarence Wret 2022
            Embargoed data release so not including for now
            MINERvA xsec on H in anti-neutrino ME era
            See Tejin Cai (Rochester) PhD thesis for full information
*/

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_1DQ2_antinu_H.h"


//********************************************************************  
void MINERvA_CC0pi_XSec_1DQ2_antinu_H::SetupDataSettings() {
//********************************************************************  

  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");

  // Define what files to use from the dist
  std::string basedir = "MINERvA/CC0pi_1D/Hydrogen";
  std::string histname = "";
  std::string xbinning = basedir;
  std::string ybinning = basedir;

  std::string distdescript = "MINERvA_CC0pi_XSec_2Dptpz_antinu sample";
  std::string datafile = basedir+"xsec_mod.csv";
  std::string covfile = basedir+"cross_sections_muonpz_muonpt_lowangleqelike_minerva_covariance.csv";
  std::string titles    = "MINERvA CC0#pi #bar{#nu}_{#mu} p_{t} p_{z};p_{t} (GeV);p_{z} (GeV);d^{2}#sigma/dp_{t}dp_{z} (cm^{2}/GeV^{2}/nucleon)";

  fScaleFactor  = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );
  fSettings.SetZTitle( GeneralUtils::ParseToStr(titles,";")[3] );

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"\
                        "Target: H \n";

  descrip += "Flux: MINERvA Medium Energy RHC anti-numu  \n" \
              "Signal: CC-0pi on H\n";
  fSettings.SetDescription(descrip);

  // The input ROOT file in the fSettings
  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);

  // Set the data 
  SetDataFromTextFile(fSettings.GetDataInput());
};

//********************************************************************
MINERvA_CC0pi_XSec_1DQ2_antinu_H::MINERvA_CC0pi_XSec_1DQ2_antinu_H(nuiskey samplekey) {
  //********************************************************************

  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("antinumu");

  // Set up the data and covariance matrix
  SetupDataSettings();

  FinaliseSampleSettings();

  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC0pi_XSec_1DQ2_antinu_H::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Checking to see if there is a Muon
  if (event->NumFSParticle(-13) == 0) return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  fXVar = -1*(Pnu-Pmu).Mag2();
};

//********************************************************************
bool MINERvA_CC0pi_XSec_1DQ2_antinu_H::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_anti_MINERvAPTPZ_ME_H(event, -14, EnuMin, EnuMax);
};
