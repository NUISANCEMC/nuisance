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
            MINERvA xsec on H in anti-neutrino ME era
            Nature 614 (2023) 7946, 48-53
            See Tejin Cai (Rochester) PhD thesis for full information
*/

#include "MINERvA_CC0pi_XSec_1DQ2_antinu_H.h"

//********************************************************************  
void MINERvA_CC0pi_XSec_1DQ2_antinu_H::SetupDataSettings() {
//********************************************************************  

  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");

  // Define what files to use from the dist
  std::string basedir = "MINERvA/CC0pi_1D/Hydrogen/";
  std::string histname = "";
  std::string xbinning = basedir;
  std::string ybinning = basedir;

  std::string distdescript = "MINERvA_CC0pi_XSec_1DQ2_antinu_H sample";
  // The edited data files just contain; low Q2, xsec, stat+sys
  std::string datafile = basedir+"xsec_mod_acc.csv";
  std::string covfile = basedir+"cov_tot.csv";
  //std::string datafile = basedir+"xsec_mod_acc_hack.csv";
  //std::string covfile = basedir+"cov_tot_hack.csv";
  std::string titles    = "MINERvA CC0#pi #bar{#nu}_{#mu} Q^{2} H;Q^{2} (GeV^{2}); d#sigma/dQ^{2} (cm^{2}/(GeV^{2})/nucleon)";

  fScaleFactor  = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();
  fScaleFactor *= 12; // Result is cm2/GeV2/Hydrogen in a CH target, so need to scale the /nucleon cross-section appropriately

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"\
                        "Target: H \n";

  descrip += "Flux: MINERvA Medium Energy RHC anti-numu  \n" \
              "Signal: CC-0pi on H\n";
  fSettings.SetDescription(descrip);

  // The input ROOT file in the fSettings
  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);
  fSettings.SetCovarInput(FitPar::GetDataBase() + covfile);

  // Set the data 
  SetDataFromTextFile(fSettings.GetDataInput());
  fDataHist->Scale(1E-38); // release comes in units of 1E-38
  
  // Covariance comes in 1E-40, so need to scale by 1E-2 (NUISANCE assumes 1E-38 for uncert.)
  SetCovarMatrixFromText(fSettings.GetCovarInput(), fDataHist->GetXaxis()->GetNbins(), 1E-4);
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
  double Q2 = -1*(Pnu-Pmu).Mag2()/1.E6;

  // Just a check to make sure Q2QE on hydrogen and Q2 true is the same, and it is (to 1E-3 GeV2)
  /*
  const double Mn = 0.9396*1E3;// GeV/c2
  const double Mp = 0.9382*1E3;// GeV/c2
  double costhetamu = cos(Pmu.Vect().Angle(Pnu.Vect()));
  double EnuQE = (Mn*Mn-Mp*Mp-Pmu.Mag2()+2*Mp*Pmu.E())/(2*(Mp-Pmu.E()+Pmu.Vect().Mag()*costhetamu));
  double Q2QE = 2*EnuQE*(Pmu.E()-Pmu.Vect().Mag()*costhetamu)-Pmu.Mag2();
  Q2QE *= 1E-6;

  if (isSignal(event) && fabs(Q2QE-Q2)>1E-3) {
    std::cout << "***" << std::endl;
    std::cout << Q2 << std::endl;
    std::cout << Q2QE << std::endl;
  }
  */

  fXVar = Q2;
};

//********************************************************************
bool MINERvA_CC0pi_XSec_1DQ2_antinu_H::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCC0pi_anti_MINERvAPTPZ_ME_H(event, -14, EnuMin, EnuMax);
};
