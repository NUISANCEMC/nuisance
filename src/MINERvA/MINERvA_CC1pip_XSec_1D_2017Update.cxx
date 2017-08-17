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
#include "MINERvA_SignalDef.h"
#include "MINERvA_CC1pip_XSec_1D_2017Update.h"

//********************************************************************
void MINERvA_CC1pip_XSec_1D_2017Update::SetupDataSettings(){
//********************************************************************

  // Set Distribution
  std::string name = fSettings.GetS("name");
  if      (!name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_2017"))  fDist = kTpi;
  else if (!name.compare("MINERvA_CC1pip_XSec_1Dth_nu_2017"))   fDist= kth;
  else if (!name.compare("MINERvA_CC1pip_XSec_1Dpmu_nu_2017"))  fDist= kpmu;
  else if (!name.compare("MINERvA_CC1pip_XSec_1Dthmu_nu_2017")) fDist= kthmu;
  else if (!name.compare("MINERvA_CC1pip_XSec_1DQ2_nu_2017"))   fDist= kQ2;
  else if (!name.compare("MINERvA_CC1pip_XSec_1DEnu_nu_2017"))  fDist= kEnu;
  
  // Define what files to use from the dist
  std::string datafile = "";
  std::string covarfile = "";
  std::string titles = "";
  std::string distdescript = "";
  switch(fDist){

  case (kTpi):
    datafile  = "cc1pip_updated_1DTpi";
    covarfile = "cc1pip_updated_1DTpi";
    titles    = "CC1#pi Updated;T_{#pi} (MeV);d#sigma/dT_{#pi} (cm^{2}/nucleon/MeV)";
    break;

  case (kth):
    datafile  = "cc1pip_updated_1Dth";
    covarfile = "cc1pip_updated_1Dth";
    titles    = "CC1#pi Updated;#theta_{#pi};d#sigma/d#theta_{#pi} (cm^{2}/nucleon)";
    break;

  case (kpmu):
    datafile  = "cc1pip_updated_1Dpmu";
    covarfile = "cc1pip_updated_1Dpmu";
    titles    = "CC1#pi Updated;p_{#mu} (GeV);d#sigma/dp_{#mu} (cm^{2}/nucleon/GeV)";
    break;

  case (kthmu):
    datafile  = "cc1pip_updated_1Dthmu";
    covarfile = "cc1pip_updated_1Dthmu";
    titles    ="CC1#pi Updated;#theta_{#mu};d#sigma/d#theta_{#mu} (cm^{2}/nucleon)";
    break;

  case (kQ2):
    datafile  = "cc1pip_updated_1DQ2";
    covarfile = "cc1pip_updated_1DQ2";
    titles    ="CC1#pi Updated;Q^{2} (GeV^{2});d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})";
    break;

  case (kEnu):
    datafile  = "cc1pip_updated_1DEnu";
    covarfile = "cc1pip_updated_1DEnu";
    titles    ="CC1#pi Updated;E_{#nu} (GeV);#sigma(E_#nu) (cm^{2}/nucleon)";
    break;

  default:
    THROW("Unknown Analysis Distribution : " << fDist);
  }

  // Choose shape or rate covariance
  fIsShape = fSettings.Found("type","SHAPE");
  std::string covid = fIsShape ? "_shapecov.txt" : "_ratecov.txt";

  // Now setup each data distribution and description.
  std::string descrip =  distdescript + \
    "Target: CH \n"				       \
    "Flux: MINERvA Forward Horn Current numu ONLY \n"  \
    "Signal: Any event with 1 muon, and 1pi+ or 1pi- in FS. W < 1.4";

  fSettings.SetDescription(descrip);
  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/070717/" + datafile + "_data.txt" );
  fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/070717/" + covarfile + covid );
  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );

  return;
}

//********************************************************************
MINERvA_CC1pip_XSec_1D_2017Update::MINERvA_CC1pip_XSec_1D_2017Update(nuiskey samplekey) {
//********************************************************************
  
  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  SetupDataSettings();
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // If Enu setup scale factor for Enu Unfolded, otherwise use differential
  if (fDist == kEnu) fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);
  else fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCorrelationFromTextFile( fSettings.GetCovarInput() );
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CC1pip_XSec_1D_2017Update::FillEventVariables(FitEvent *event) {
//********************************************************************

  fXVar = -999.9;
  if (event->NumFSParticle(PhysConst::pdg_charged_pions) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double Tpi     = Ppip.E() - Ppip.Mag();
  double th      = (180./M_PI)*FitUtils::th(Pnu, Ppip);
  double pmu     = Pmu.Vect().Mag()/1.E3; // GeV
  double thmu    = (180.0/M_PI)*FitUtils::th(Pnu, Pmu);
  double Q2      = fabs((Pmu - Pnu).Mag2()) / 1.E6;  // Using true here?
  double Enu     = Pnu.E() / 1.E3;


  switch(fDist){
  case kTpi:  fXVar = Tpi;  break;
  case kth:   fXVar = th;   break;
  case kpmu:  fXVar = pmu;  break;
  case kthmu: fXVar = thmu; break;
  case kQ2:   fXVar = Q2;   break;
  case kEnu:  fXVar = Enu;  break;
  default:
    THROW("DIST NOT FOUND : " << fDist);
  }
  
  return;
};

//********************************************************************
bool MINERvA_CC1pip_XSec_1D_2017Update::isSignal(FitEvent *event) {
//********************************************************************
  // Only seem to release full phase space
  return SignalDef::isCC1pip_MINERvA_2017(event, EnuMin, EnuMax);
}
