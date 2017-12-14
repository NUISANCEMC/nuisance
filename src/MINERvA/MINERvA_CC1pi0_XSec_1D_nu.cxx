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
#include "MINERvA_CC1pi0_XSec_1D_nu.h"

// Implementation of 2017 MINERvA numu CC1pi0
// arxiv:1708.03723v1 hep-ex
// c.wret14@imperial.ac.uk

//********************************************************************
void MINERvA_CC1pi0_XSec_1D_nu::SetupDataSettings(){
//********************************************************************

  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  if      (!name.compare("MINERvA_CC1pi0_XSec_1DTpi_nu"))  fDist = kTpi;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1Dth_nu"))   fDist= kth;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1Dpmu_nu"))  fDist= kpmu;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1Dthmu_nu")) fDist= kthmu;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DQ2_nu"))   fDist= kQ2;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DEnu_nu"))  fDist= kEnu;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DWexp_nu")) fDist= kWexp;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DPPi0Mass_nu")) fDist= kPPi0Mass;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DPPi0MassDelta_nu")) fDist= kPPi0MassDelta;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DCosAdler_nu")) fDist= kCosAdler;
  else if (!name.compare("MINERvA_CC1pi0_XSec_1DPhiAdler_nu")) fDist= kPhiAdler;

  // Define what files to use from the dist
  std::string datafile = "";
  std::string corrfile = "";
  std::string titles = "";
  std::string distdescript = "";
  // Set the default to essentially not be a cut on proton kinetic energy
  // The Adler angles and reconstructed p,pi0 invariant mass have cuts on these
  ProtonCut = 100;
  // W exp is 1.8 GeV or lower (dealt with below)
  WexpCut = 1.8;

  // Load up the data
  switch (fDist) {

    case (kTpi):
      datafile  = "data/XSec_Table_pi0_KE_xsec.csv";
      corrfile = "corr/Correlation_Table_pi0_KE_xsec.csv";
      titles    = "CC1#pi^{0};T_{#pi} (GeV);d#sigma/dT_{#pi} (cm^{2}/nucleon/GeV)";
      break;

    case (kth):
      datafile  = "data/XSec_Table_pi0_theta_xsec.csv";
      corrfile = "corr/Correlation_Table_pi0_theta_xsec.csv";
      titles    = "CC1#pi^{0};#theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/nucleon/degree)";
      break;

    case (kpmu):
      datafile  = "data/XSec_Table_muon_P_xsec.csv";
      corrfile = "corr/Correlation_Table_muon_P_xsec.csv";
      titles    = "CC1#pi^{0};p_{#mu} (GeV);d#sigma/dp_{#mu} (cm^{2}/nucleon/GeV)";
      break;

    case (kthmu):
      datafile  = "data/XSec_Table_muon_theta_xsec.csv";
      corrfile = "corr/Correlation_Table_muon_theta_xsec.csv";
      titles    = "CC1#pi^{0};#theta_{#mu} (degrees);d#sigma/d#theta_{#mu} (cm^{2}/nucleon/degree)";
      break;

    case (kQ2):
      datafile  = "data/XSec_Table_QSq_xsec.csv";
      corrfile = "corr/Correlation_Table_QSq_xsec.csv";
      titles    = "CC1#pi^{0};Q^{2} (GeV^{2});d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})";
      break;

    case (kEnu):
      datafile  = "data/XSec_Table_Enu_xsec.csv";
      corrfile = "corr/Correlation_Table_Enu_xsec.csv";
      titles    = "CC1#pi^{0};E_{#nu} (GeV);#sigma(E_#nu) (cm^{2}/nucleon)";
      break;

    case (kWexp):
      datafile  = "data/XSec_Table_W_xsec.csv";
      corrfile = "corr/Correlation_Table_W_xsec.csv";
      titles    = "CC1#pi^{0};W_{exp} (GeV);d#sigma/dW_{exp} (cm^{2}/nucleon/GeV)";
      break;

    case (kPPi0Mass):
      datafile  = "data/XSec_Table_deltaInvMass_xsec.csv";
      corrfile = "corr/Correlation_Table_deltaInvMass_xsec.csv";
      titles    = "CC1#pi^{0}; M_{p#pi^{0}} (GeV); d#sigma/dM_{p#pi^{0}} (cm^{2}/nucleon/GeV)";
      break;

    case (kPPi0MassDelta):
      datafile  = "data/XSec_Table_deltaInvMass_xsec_DeltaRich.csv";
      corrfile = "corr/Correlation_Table_deltaInvMass_xsec_DeltaRich.csv";
      titles    = "CC1#pi^{0}; M_{p#pi^{0}} W_{exp} < 1.4 (GeV); d#sigma/dM_{p#pi^{0}} (cm^{2}/nucleon/GeV)";
      break;

    case (kCosAdler):
      datafile  = "data/XSec_Table_Delta_pi_theta_xsec.csv";
      corrfile = "corr/Correlation_Table_Delta_pi_theta_xsec.csv";
      titles    = "CC1#pi^{0}; cos#theta_{Adler}; d#sigma/dcos#theta_{Adler} (cm^{2}/nucleon/0.1)";
      break;

    case (kPhiAdler):
      datafile  = "data/XSec_Table_Delta_pi_phi_xsec.csv";
      corrfile = "corr/Correlation_Table_Delta_pi_phi_xsec.csv";
      titles    = "CC1#pi^{0}; #phi_{Adler} (degrees); d#sigma/d#phi_{Adler} (cm^{2}/nucleon/degree)";
      break;

    default:
      THROW("Unknown Analysis Distribution : " << fDist);
  }

  // Set the Wexp and proton kinetic energy cuts depending on sample
  // for Ppi0Mass distributions and Adler angles we require a proton of at least 100 MeV kinetic energy
  if (fDist >= kPPi0Mass) {
    ProtonCut = 0.1;
    // 0.1 GeV proton kinetic energy cut
    // Some distributions have a Wexp cut at 1.4 GeV which attempts to isolate delta production
    if (fDist >= kPPi0MassDelta) {
      WexpCut = 1.4;
    }
  }

  // Only have xsec covariance (not shape only)

  // Now setup each data distribution and description.
  std::string descrip =  distdescript + \
                         "Target: CH \n"				       \
                         "Flux: MINERvA Forward Horn Current numu ONLY \n"  \
                         "Signal: Any event with 1 muon, and 1pi0 in FS, no mesons, any nucleon(s). W < 1.8" \
                         "Alt Signal: Add in requirement of 1 proton with 100 MeV and sometimes W < 1.4";

  fSettings.SetDescription(descrip);
  // Specify the data
  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2017_nu/" + datafile);
  // And the correlations
  fSettings.SetCovarInput(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0/2017_nu/" + corrfile);
  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );

  return;
}

//********************************************************************
MINERvA_CC1pi0_XSec_1D_nu::MINERvA_CC1pi0_XSec_1D_nu(nuiskey samplekey) {
  //********************************************************************

  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  // From 1.5 to 20 GeV Enu
  fSettings.SetEnuRange(1.5, 20.0);
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
  // The errors come as a percent of the cross-section for this measurement so need rescaling
  // Have set the fDataHist above so just loop over and set new errors
  for (int i = 0; i < fDataHist->GetNbinsX(); ++i) {
    fDataHist->SetBinError(i+1, (fDataHist->GetBinError(i+1)/100.0)*fDataHist->GetBinContent(i+1));
  }
  // And finally all the numbers are in units of 1E-40 so scale the histograms as such
  fDataHist->Scale(1.0E-40);
  // This measurement gives us a correlation matrix, so should set it up as such
  SetCorrelationFromTextFile( fSettings.GetCovarInput() );

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CC1pi0_XSec_1D_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  fXVar = -999.9;
  // Need a neutral pion and a muon
  if (event->NumFSParticle(111) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  // Get the TLorentzVectors from the event
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  // Pion kinetic energy
  double Tpi     = (Ppi0.E() - Ppi0.Mag())/1.E3;
  // Pion-neutrino angle
  double th      = (180./M_PI)*FitUtils::th(Pnu, Ppi0);
  // Muon momentum
  double pmu     = Pmu.Vect().Mag()/1.E3;
  // Muon-neutrino angle
  double thmu    = (180.0/M_PI)*FitUtils::th(Pnu, Pmu);
  // True Q2
  double Q2      = fabs((Pmu - Pnu).Mag2()) / 1.E6;
  // True Enu
  double Enu     = Pnu.E() / 1.E3;
  // Wexp (derived from "kinematic quantities" but uses EnuTrue)
  double Wexp    = FitUtils::Wrec(Pnu, Pmu)/1.E3;

  // Wexp cut of 1.8 GeV in signal definition
  // N.B. the Adler angles and PPi0 mass requires this to be 1400
  if (Wexp > WexpCut) return;

  // Some distributions require the final state proton: check that it exists
  if (fDist >= kPPi0Mass && event->NumFSParticle(2212) == 0) return;

  // Fill the variables depending on the enums
  switch (fDist) {
    case kTpi:
      fXVar = Tpi;
      break;
    case kth:
      fXVar = th;
      break;
    case kpmu:
      // Pmu has a theta_mu < 25degree cut
      if (thmu > 25) return;
      else fXVar = pmu;
      break;
    case kthmu:
      // thmu has a theta_mu < 25degree cut
      if (thmu > 25) return;
      else fXVar = pmu;
      fXVar = thmu;
      break;
    case kQ2:
      fXVar = Q2;
      break;
    case kEnu:
      fXVar = Enu;
      break;
    case kWexp:
      fXVar = Wexp;
      break;
    // p, pi0 invariant mass with Wexp < 1.8 or Wexp < 1.4: already checked these above
    case kPPi0Mass:
    case kPPi0MassDelta:
      {
      // Get the proton
      TLorentzVector Pprot = event->GetHMFSParticle(2212)->fP;
      double Ppi0Mass = (Ppi0+Pprot).Mag()/1.E3;
      fXVar = Ppi0Mass;
      break;
      }
    // Cos theta Adler angle
    case kCosAdler:
      {
      TLorentzVector Pprot = event->GetHMFSParticle(2212)->fP;
      double CosThAdler = FitUtils::CosThAdler(Pnu, Pmu, Ppi0, Pprot);
      fXVar = CosThAdler;
      break;
      }
    // Phi Adler angle
    case kPhiAdler:
      {
      TLorentzVector Pprot = event->GetHMFSParticle(2212)->fP;
      double PhiAdler = FitUtils::PhiAdler(Pnu, Pmu, Ppi0, Pprot);
      fXVar = PhiAdler;
      break;
      }
    default:
      THROW("DIST NOT FOUND : " << fDist);
      break;
  }

};

//********************************************************************
bool MINERvA_CC1pi0_XSec_1D_nu::isSignal(FitEvent *event) {
  //********************************************************************
  // Some of the distributions require a proton with at least 100 MeV KE
  if (fDist >= kPPi0Mass) {
    // First of needs a proton in the final state
    if (event->NumFSParticle(2212) == 0) return false;

    // Needs to pass CC1pi0 signal definition
    bool pass_cc1pi0 = SignalDef::isCC1pi0_MINERvA_nu(event, EnuMin, EnuMax);
    if (!pass_cc1pi0) return false;

    // And the proton needs at least 100 MeV kinetic energy
    TLorentzVector Pprot = event->GetHMFSParticle(2212)->fP;
    double ke = (Pprot.E() - Pprot.Mag())/1.E3;
    if (pass_cc1pi0 && ke > ProtonCut) {
      return true;
    } else {
      return false;
    }
  // The other distributions ahve a more generic "1mu, 1pi0, no mesons, any nucleon(s)"
  // The W cut is instead made in FillEventVariables
  } else {
    return SignalDef::isCC1pi0_MINERvA_nu(event, EnuMin, EnuMax);
  }
}

