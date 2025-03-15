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
#include "T2K_SignalDef.h"
#include "T2K_CC1pipNp_CH_XSec_1DSTV_nu.h"

/*
Phys. Rev. D 103, 112009
https://doi.org/10.1103/PhysRevD.103.112009
Data release from https://t2k-experiment.org/results/2021_tki_numucc1pinp/

Signal definition: single mu-, single pi+, at least one p
Cuts on particle acceptance:
mu-:  250 MeV/c - 7000 MeV/c, theta < 70 degrees
pi+:  150 MeV/c - 1200 MeV/c, theta < 70 degrees
p:    450 MeV/c - 1200 MeV/c, theta < 70 degrees

For T2K collaborators: TN 383
*/

//********************************************************************
void T2K_CC1pipNp_CH_XSec_1DSTV_nu::SetupData() {
//********************************************************************

  // Set data distribution and covariance matrix
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  if      (!name.compare("T2K_CC1pipNp_CH_XSec_1DdaT_nu"))  fDist = kdaT;
  else if (!name.compare("T2K_CC1pipNp_CH_XSec_1DdpTT_nu")) fDist = kdpTT;
  else if (!name.compare("T2K_CC1pipNp_CH_XSec_1DpN_nu"))   fDist = kpN;

  // Define what files to use from the dist
  std::string titles = "";
  std::string distdescript = "";

  // Specify the data, depending on distribution
  std::string datalocation = GeneralUtils::GetTopLevelDir()+"/data/T2K/CC1pipNp_STV/";

  switch (fDist) {
    case (kdaT):
      {
      datalocation += "xsec_daT.root";
      titles    = "T2K CC1#pi^{+}Np;" \
                  "#delta#alpha_{T} (degrees);" \
                  "d#sigma/d#delta#alpha_{T} (cm^{2}/nucleon/degree)";
      break;
      }
    case (kdpTT):
      {
      datalocation += "xsec_dpTT.root";
      titles    = "T2K CC1#pi^{+}Np;" \
                  "#delta p_{TT} (MeV/c);" \
                  "d#sigma/d#delta p_{TT} (cm^{2}/nucleon/(MeV/c))";
      break;
      }
    case (kpN):
      {
      datalocation += "xsec_pN.root";
      titles    = "T2K CC1#pi^{+}Np;" \
                  "p_{N} (MeV/c);" \
                  "d#sigma/dp_{N} (cm^{2}/nucleon/(MeV/c))";
      break;
      }
    default:
      NUIS_ABORT("Unknown Analysis Distribution : " << fDist);
  }

  // Only have xsec covariance (not shape only)

  // Now setup each data distribution and description.
  std::string descrip =  distdescript +                                   \
                         "Target: CH \n"                                  \
                         "Flux: T2K Forward Horn Current numu ONLY \n"    \
                         "Signal: Any event with 1mu-, 1pi+, Np (N>0), with \n" \
                         "#theta_{#mu,#nu}<70 degrees \n"                 \
                         "#theta_{#mu,#pi}<70 degrees \n"                 \
                         "#theta_{#mu,p}<70 degrees \n"                   \
                         "250 < p_{#mu} < 7000 MeV/c \n"                  \
                         "150 < p_{#pi} < 1200 MeV/c \n"                  \
                         "450 < p_{p} < 1200 MeV/c \n"                    \
                         "Ref: Phys. Rev. D 103, 112009 \n"               \
                         "arXiv:2102.03346 [hep-ex] \n"                   \
                         "https://t2k-experiment.org/results/2021_tki_numucc1pinp/";

  fSettings.SetDescription(descrip);

  fSettings.SetDataInput(datalocation);
  fSettings.SetCovarInput(datalocation);

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );

  return;
}

//********************************************************************
T2K_CC1pipNp_CH_XSec_1DSTV_nu::T2K_CC1pipNp_CH_XSec_1DSTV_nu(nuiskey samplekey) {
  //********************************************************************

  // Define Sample Settings common to all data distributions
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  // No energy cut
  fSettings.SetEnuRange(0, 100);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  SetupData();
  FinaliseSampleSettings();

  // Plot Setup -------------------------------------------------------
  SetDataFromRootFile(fSettings.GetDataInput(), "xsec_best_fit");

  // This measurement gives us a correlation matrix, so should set it up as such
  // Correlation matrix comes in absolute units
  SetCovarFromDataFile(fSettings.GetCovarInput(), "xsec_cov", true);

  // The data histogram doesn't have errors on, so set from diagonal
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, 1E-38, false);

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = (GetEventHistogram()->Integral("width") * 1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void T2K_CC1pipNp_CH_XSec_1DSTV_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  fXVar = -999.9;
  if (event->NumFSParticle(13) == 0 || 
      event->NumFSParticle(211) == 0 ||
      event->NumFSParticle(2212) == 0) {
    return;
  }

  // Get the TLorentzVectors from the event
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;

  // Proton is a bit trickier (allows for multiple protons)
  std::vector<FitParticle*> protons = event->GetAllFSProton();
  const double protlo = 450;
  const double prothi = 1200;
  const double angular = 70.*M_PI/180.; // 70 degree cut
  int protindex = 0;
  for (int i = 0; i < protons.size(); ++i) {
    if (protons[i]->fP.Vect().Mag() > protlo &&
        protons[i]->fP.Vect().Mag() < prothi &&
        protons[i]->fP.Vect().Angle(Pnu.Vect()) < angular) {
      if (protindex == 0) {
        protindex = i;
      } else if (protons[i]->fP.Vect().Mag() > 
                 protons[protindex]->fP.Vect().Mag()) {
        protindex = i;
      }
    }
  }
  TLorentzVector Pp = protons[protindex]->fP;

  // Make the z vector (cross between nu and mu vectors)
  // Make it unit length
  TVector3 z = (Pnu.Vect().Cross(Pmu.Vect())).Unit();

  // first make projection along neutrino direction
  double plmu = Pmu.Vect().Dot(  Pnu.Vect().Unit());
  double plpi = Ppip.Vect().Dot( Pnu.Vect().Unit());
  double plp = Pp.Vect().Dot(    Pnu.Vect().Unit());
  // Hadronic projection (proton and pion)
  double plhad = (Ppip.Vect()+Pp.Vect()).Dot(Pnu.Vect().Unit());

  // Muon vector in the non-neutrino direction
  TVector3 ptmuvec = Pmu.Vect() - plmu*(Pnu.Vect().Unit());
  // Hadron vector in the non-neutrino direction
  TVector3 pthadvec = (Ppip.Vect()+Pp.Vect())-plhad*(Pnu.Vect().Unit());
  // Sum
  TVector3 ptvec = ptmuvec + pthadvec;

  // Fill the variables depending on the enums
  switch (fDist) {
    case kdaT:
      {
      fXVar = acos((-1*ptmuvec.Dot(ptvec))/(ptmuvec.Mag()*ptvec.Mag()))*180./M_PI;
      break;
      }
    case kdpTT:
      {
      // Project the pion three vector along the z direction
      double ptt_pi = Ppip.Vect().Dot(z);
      // And the proton
      double ptt_p = Pp.Vect().Dot(z);
      double dptt = ptt_pi+ptt_p;
      fXVar = dptt;
      break;
      }
    case kpN:
      {
      // Get the longitudinal (neutrino direction) projection
      double pmul = Pmu.Vect().Dot(Pnu.Vect().Unit());
      double ppil = Ppip.Vect().Dot(Pnu.Vect().Unit());
      double ppl = Pp.Vect().Dot(Pnu.Vect().Unit());
      const double eps = 26.1; // MeV, mean proton excitation energy in Carbon, table 8 of arxiv 1801.07975
      const double mn = PhysConst::mass_neutron*1E3; // neutron mass
      const double mp = PhysConst::mass_proton*1E3;  // proton mass
      const double MA = 6*mn+6*mp-92.16;  // target mass (E is from PhysRevC.95.065501)
      const double MAP = MA-mn+27.13; // remnant mass, but... binding energy?, but MINERvA paper say 28.7 MeV...
      const double temp = MA+pmul+ppil+ppl-Pmu.E()-Ppip.E()-Pp.E();
      double pl = 0.5*temp;
      pl -= 0.5*(ptvec.Mag2()+MAP*MAP)/temp;
      double pn = sqrt(ptvec.Mag2()+pl*pl);
      fXVar = pn;
      break;
      }
    default:
      NUIS_ABORT("DIST NOT FOUND : " << fDist);
      break;
  }

};

//********************************************************************
bool T2K_CC1pipNp_CH_XSec_1DSTV_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isT2K_CC1pipNp_STV(event);
}

