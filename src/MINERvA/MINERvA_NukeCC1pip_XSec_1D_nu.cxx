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

  // Define a regular expression pattern to match the format
  std::regex pattern("MINERvA_NukeCC1pip_(.*?)_XSec_1D(.*?)_nu");

  std::string target_str;
  std::string distribution_str;
  std::smatch matches;
  if(std::regex_search(name, matches, pattern)) {
    if(matches.size() == 3) {
      target_str = matches[1];
      distribution_str = matches[2];
    }
  }
  else{
    NUIS_ABORT("Name should be MINERvA_NukeCC1pip_<Target>_XSec_1D<Distribution>_nu, but " << name << " is given");
  }

  if(target_str=="CH") fTarget = CH;
  else if(target_str=="C") fTarget = C;
  else if(target_str=="H2O") fTarget = H2O;
  else if(target_str=="Fe") fTarget = Fe;
  else if(target_str=="Pb") fTarget = Pb;
  else{
    NUIS_ABORT("Unknown target: " << target_str);
  }

  std::string titles = "";
  if(distribution_str=="pmu"){
    fDistribution = kpmu;
    titles    = "p_{#mu} (GeV);d#sigma/dp_{#mu} (cm^{2}/nucleon/GeV)";
  }
  else if(distribution_str=="thmu"){
    fDistribution = kthmu;
    titles    = "#theta_{#mu} (deg);d#sigma/d#theta_{#mu} (cm^{2}/nucleon/deg)";
  }
  else if(distribution_str=="plmu"){
    fDistribution = kplmu;
    titles    = "p_{#mu,||} (GeV);d#sigma/dp_{#mu,||} (cm^{2}/nucleon/GeV)";
  }
  else if(distribution_str=="ptmu"){
    fDistribution = kptmu;
    titles    = "p_{#mu,T} (GeV);d#sigma/dp_{#mu,T} (cm^{2}/nucleon/GeV)";
  }
  else if(distribution_str=="Q2"){
    fDistribution = kQ2;
    titles    = "Q^{2} (GeV^{2});d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})";
  }
  else if(distribution_str=="Wexp"){
    fDistribution = kWexp;
    titles    = "W_{exp} (GeV/c^{2});d#sigma/dW_{exp} (cm^{2}/nucleon/(GeV/c^{2}))";
  }
  else if(distribution_str=="Tpi"){
    fDistribution = kTpi;
    titles    = "T_{#pi} (GeV);d#sigma/dT_{#pi} (cm^{2}/nucleon/GeV)";
  }
  else if(distribution_str=="thpi"){
    fDistribution = kthpi;
    titles    = "#theta_{#pi} (deg);d#sigma/d#theta_{#pi} (cm^{2}/nucleon/deg)";
  }
  else{
    NUIS_ABORT("Unknown distribution: " << distribution_str);
  }


  std::string distdescript = "";
  std::string datafile = "NukeCC1pip_"+target_str+"_"+distribution_str;

  // Now setup each data distribution and description.
  std::string descrip =  "Target: " + target_str + "\n" + "Flux: MINERvA Forward Horn Current numu ONLY \n" +
                         "Signal: Any event with 1 muon, and 1pi+ in FS, 1.5<pmu<20 GeV/c, thmu<13degree, 35<Tpi<350 MeV,  Wexp < 1.4";

  fSettings.SetDescription(descrip);

  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCC1pip/" + datafile + ".txt" );
  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/NukeCC1pip/" + datafile + "_cov_total.txt" );
  fSettings.SetXTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[1] );

  return;
}

//********************************************************************
MINERvA_NukeCC1pip_XSec_1D_nu::MINERvA_NukeCC1pip_XSec_1D_nu(nuiskey samplekey) {
//********************************************************************
  
  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0., 20.0);
  fSettings.DefineAllowedTargets("C,H"); // TODO Does nothing for now?
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
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetHMISParticle(14)->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;

  // Pz is the neutrino-direction component of muon 3-momentum
  Double_t pz_mu = Pmu.Vect().Dot( Pnu.Vect().Unit() ); // MeV
  TVector3 pt_mu = Pmu.Vect() - pz_mu * Pnu.Vect().Unit(); // MeV

  float Q2_true = -1 * (Pmu - Pnu).Mag2()/1E6;
  double Tpi = (Ppip.E() - Ppip.Mag())/1E3; // GeV

  switch(fDistribution){
    case kpmu:  fXVar = Pmu.Vect().Mag()/1.E3; break;
    case kthmu: fXVar = (180.0/M_PI)*FitUtils::th(Pnu, Pmu); break;
    case kplmu: fXVar = pz_mu/1000.; break;
    case kptmu: fXVar = pt_mu.Mag()/1000.; break;
    case kQ2:   fXVar = Q2_true; break;
    case kWexp: fXVar = FitUtils::Wrec(Pnu, Pmu)/1000.; break;
    case kTpi:  fXVar = Tpi; break;
    case kthpi: fXVar = (180.0/M_PI)*FitUtils::th(Pnu, Ppip); break;
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