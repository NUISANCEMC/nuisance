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
#include "MINERvA_SignalDef.h"
#include "MINERvA_NukeCC1pip_XSec_1D_nu.h"

//********************************************************************
void MINERvA_NukeCC1pip_XSec_1D_nu::SetupDataSettings(){
//********************************************************************

  std::string name = fSettings.GetS("name");

  // Have C, CH, H2O, Fe, Pb target
  std::string target_str = "";
  if (name.find("MINERvA_NukeCC1pip_CH_XSec_1D")      != std::string::npos) {
    fTarget = kCH;
    target_str = "CH";
  }
  else if (name.find("MINERvA_NukeCC1pip_C_XSec_1D")  != std::string::npos) {
    fTarget = kC;
    target_str = "C";
  }
  else if (name.find("MINERvA_NukeCC1pip_H2O_XSec_1D")!= std::string::npos) {
    fTarget = kH2O;
    target_str = "H2O";
  }
  else if (name.find("MINERvA_NukeCC1pip_Pb_XSec_1D") != std::string::npos) {
    fTarget = kPb;
    target_str = "Pb";
  }
  else if (name.find("MINERvA_NukeCC1pip_Fe_XSec_1D") != std::string::npos) {
    fTarget = kFe;
    target_str = "Fe";
  }
  else {
    std::cerr << "Could not find appropriate target for " << name << std::endl;
  }

  std::string distribution_str = "";
  std::string titles = "";
  // Find the distribution
  if (name.find("_XSec_1Dpmu_nu")       != std::string::npos) {
    fDistribution = kPmu;
    distribution_str = "pmu";
    titles    = "p_{#mu} (GeV);d#sigma/dp_{#mu} (cm^{2}/nucleon/GeV)";
  } 
  else if (name.find("_XSec_1Dthmu_nu") != std::string::npos) {
    fDistribution = kThmu;
    distribution_str = "thmu";
    titles    = "#theta_{#mu} (deg);d#sigma/d#theta_{#mu} (cm^{2}/nucleon/deg)";
  }
  else if (name.find("_XSec_1Dplmu_nu") != std::string::npos) {
    fDistribution = kPlmu;
    distribution_str = "plmu";
    titles    = "p_{#mu,||} (GeV);d#sigma/dp_{#mu,||} (cm^{2}/nucleon/GeV)";
  }
  else if (name.find("_XSec_1Dptmu_nu") != std::string::npos) {
    fDistribution = kPtmu;
    distribution_str = "ptmu";
    titles    = "p_{#mu,T} (GeV);d#sigma/dp_{#mu,T} (cm^{2}/nucleon/GeV)";
  }
  else if (name.find("_XSec_1DQ2_nu")   != std::string::npos) {
    fDistribution = kQ2;
    distribution_str = "Q2";
    titles    = "Q^{2} (GeV^{2});d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})";
  } 
  else if (name.find("_XSec_1DWexp_nu") != std::string::npos) {
    fDistribution = kWexp;
    distribution_str = "Wexp";
    titles    = "W_{exp} (GeV/c^{2});d#sigma/dW_{exp} (cm^{2}/nucleon/(GeV/c^{2}))";
  }
  else if (name.find("_XSec_1DTpi_nu")  != std::string::npos) {
    fDistribution = kTpi;
    distribution_str = "Tpi";
    titles    = "T_{#pi} (GeV);d#sigma/dT_{#pi} (cm^{2}/nucleon/GeV)";
  }
  else if (name.find("_XSec_1Dthpi_nu") != std::string::npos) {
    fDistribution = kThpi;
    distribution_str = "thpi";
    titles    = "#theta_{#pi} (deg);d#sigma/d#theta_{#pi} (cm^{2}/nucleon/deg)";
  } 
  else {
    std::cerr << "Could not find appropriate distribution for " << name << std::endl;
  }
  std::cout << name << " " << target_str << " " << distribution_str << std::endl;

  std::string distdescript = "";
  std::string datafile = "NukeCC1pip_"+target_str+"_"+distribution_str;

  // Now setup each data distribution and description.
  std::string descrip =  "Target: " + target_str + "\n" + "Flux: MINERvA Forward Horn Current numu ONLY \n" +
                         "Signal: Any event with 1 muon, and 1pi+ in FS, 1.5<pmu<20 GeV/c, thmu<13degree, 35<Tpi<350 MeV,  Wexp < 1.4";

  fSettings.SetDescription(descrip);

  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCC1pip/" + datafile + ".txt" );
  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCC1pip/" + datafile + "_cov_total.txt" );
  fSettings.SetXTitle(GeneralUtils::ParseToStr(titles,";")[0]);
  fSettings.SetYTitle(GeneralUtils::ParseToStr(titles,";")[1]);
}

//********************************************************************
MINERvA_NukeCC1pip_XSec_1D_nu::MINERvA_NukeCC1pip_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  
  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0., 20.0);
  fSettings.DefineAllowedTargets("C,H,Pb,Fe,H2O"); // TODO Does nothing for now?
  fSettings.DefineAllowedSpecies("numu");
  SetupDataSettings();

  FinaliseSampleSettings();
  // Scaling Setup ---------------------------------------------------
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------

  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile( fSettings.GetCovarInput() );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_NukeCC1pip_XSec_1D_nu::FillEventVariables(FitEvent *event) {
//********************************************************************

  fXVar = -999.9;
  if (event->NumFSParticle(PhysConst::pdg_charged_pions) == 0 ||
      event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;

  // Pz is the neutrino-direction component of muon 3-momentum
  Double_t pz_mu = Pmu.Vect().Dot( Pnu.Vect().Unit() ); // MeV
  TVector3 pt_mu = Pmu.Vect() - pz_mu * Pnu.Vect().Unit(); // MeV

  float Q2_true = -1 * (Pmu - Pnu).Mag2()/1E6;
  double Tpi = (Ppip.E() - Ppip.Mag())/1E3; // GeV

  switch (fDistribution){
    case kPmu:  fXVar = Pmu.Vect().Mag()/1.E3; break;
    case kThmu: fXVar = (180.0/M_PI)*FitUtils::th(Pnu, Pmu); break;
    case kPlmu: fXVar = pz_mu/1000.; break;
    case kPtmu: fXVar = pt_mu.Mag()/1000.; break;
    case kQ2:   fXVar = Q2_true; break;
    case kWexp: fXVar = FitUtils::Wrec(Pnu, Pmu)/1000.; break;
    case kTpi:  fXVar = Tpi; break;
    case kThpi: fXVar = (180.0/M_PI)*FitUtils::th(Pnu, Ppip); break;
    default: NUIS_ABORT("DIST NOT FOUND : " << fDistribution);
  }

};

//********************************************************************
bool MINERvA_NukeCC1pip_XSec_1D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  // Only seem to release full phase space

  bool IsSignal = SignalDef::isNukeCC1pip_MINERvA(event, EnuMin, EnuMax);

  return IsSignal;

  // Below for COH and H
  //bool IsH = event->GetTargetA()==1;
  //bool IsCOH = event->Mode==16;
  //return IsSignal && (IsCOH||IsH);
}
