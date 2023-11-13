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
  if(distribution_str=="Tpi"){
    fDistribution = kTpi;
    titles    = "T_{#pi} (GeV);d#sigma/dT_{#pi} (10^{-42}cm^{2}/nucleon/GeV)";
  }
  else if(distribution_str=="ptmu"){
    fDistribution = kptmu;
    titles    = "p_{T,#mu} (GeV);d#sigma/p_{T,#mu} (10^{-42}cm^{2}/nucleon/GeV)";
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
  //fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E4) / double(fNEvents) / TotalIntegratedFlux("width");
  //fCovScaleFactor = 1.;

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

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Tpi     = (Ppip.E() - Ppip.Mag())/1.E3; // GeV
  double th      = (180./M_PI)*FitUtils::th(Pnu, Ppip);
  double pmu     = Pmu.Vect().Mag()/1.E3; // GeV
  double thmu    = (180.0/M_PI)*FitUtils::th(Pnu, Pmu);
  double Q2      = fabs((Pmu - Pnu).Mag2()) / 1.E6;  // Using true here?
  double Enu     = Pnu.E() / 1.E3;

  // Pz is the neutrino-direction component of muon 3-momentum
  Double_t pz_mu = Pmu.Vect().Dot( Pnu.Vect().Unit() ); // MeV
  TVector3 pt_mu = Pmu.Vect() - pz_mu * Pnu.Vect().Unit(); // MeV


  switch(fDistribution){
    case kTpi:  fXVar = Tpi;  break;
    case kptmu: fXVar = pt_mu.Mag()/1000.;   break;
    default: NUIS_ABORT("DIST NOT FOUND : " << fDistribution);
  }

};

//********************************************************************
bool MINERvA_NukeCC1pip_XSec_1D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  // Only seem to release full phase space
  return SignalDef::isNukeCC1pip_MINERvA(event, EnuMin, EnuMax);
}
