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
#include "MINERvA_CCNpi0Mp_XSec_1DSTV_nu.h"

// Implementation of 2020 MINERvA numu CCNpi0Mp STV
// arxiv:2002.05812 hep-ex
// Phys.Rev.D 102 (2020) 7, 072007, Phys.Rev.D 110 (2024) 5, 059903 (erratum)
// D. Coplowe's (Oxford) thesis says 1.5 < Emu < 20 (https://lss.fnal.gov/archive/thesis/2000/fermilab-thesis-2018-38.pdf)
// clarence.wret@imperial.ac.uk
// Final state is 1mu-, 1p, 1pi0, X, where X is N possible addtional protons and neutral pions, but not other mesons; so basically CCNpi0Mp where N and M > 0

//********************************************************************
void MINERvA_CCNpi0Mp_XSec_1DSTV_nu::SetupDataSettings(){
//********************************************************************

  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  if      (!name.compare("MINERvA_CCNpi0Mp_XSec_1DdaT_nu"))  fDist = kdaT;
  else if (!name.compare("MINERvA_CCNpi0Mp_XSec_1DdpTT_nu")) fDist = kdpTT;
  else if (!name.compare("MINERvA_CCNpi0Mp_XSec_1DpN_nu"))   fDist = kpN;

  // All the data is in the same file, but are inside different TLists
  std::string dataname = "";
  std::string titles = "";
  std::string distdescript = "";

  // Load up the data
  switch (fDist) {
    case (kdaT):
      {
      dataname = "dalphat";
      titles    = "MINERvA CCN#pi^{0}Mp;" \
                  "#delta#alpha_{T} (degrees);" \
                  "d#sigma/d#delta#alpha_{T} (cm^{2}/nucleon/degree)";
      break;
      }
    case (kdpTT):
      {
      dataname = "dpTT";
      titles    = "MINERvA CCN#pi^{0}Mp;" \
                  "#delta p_{TT} (MeV/c);" \
                  "d#sigma/d#delta p_{TT} (cm^{2}/nucleon/(MeV/c))";
      break;
      }
    case (kpN):
      {
      dataname = "neutronmomentum";
      titles    = "MINERvA CCN#pi^{0}Mp;" \
                  "p_{N} (MeV/c);" \
                  "d#sigma/dp_{N} (cm^{2}/nucleon/(MeV/c))";
      break;
      }
    default:
      NUIS_ABORT("Unknown Analysis Distribution : " << fDist);
  }

  // Now setup each data distribution and description.
  std::string descrip =  distdescript + \
                         "Target: CH \n"				       \
                         "Flux: MINERvA Forward Horn Current numu ONLY \n"  \
                         "Signal: Any event with 1mu-, N pi0, M p (N,M>0), with \n" \
                         "#theta_{#mu,#nu} < 25 degrees \n"             \
                         "1.5 < p_{#mu} < 20 GeV/c \n"                  \
                         "450 < p_{p} MeV/c \n"                         \
                         "Ref: arxiv:2002.05812 hep-ex"                 \
                         "Phys.Rev.D 102 (2020) 7, 072007, Phys.Rev.D 110 (2024) 5, 059903 (erratum)" \
                         "D. Coplowe's (Oxford) thesis (https://lss.fnal.gov/archive/thesis/2000/fermilab-thesis-2018-38.pdf)";

  fSettings.SetDescription(descrip);

  fSettings.SetTitle( GeneralUtils::ParseToStr(titles,";")[0]);
  fSettings.SetXTitle(GeneralUtils::ParseToStr(titles,";")[1]);
  fSettings.SetYTitle(GeneralUtils::ParseToStr(titles,";")[2]);

  // Specify the data
  fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0Np/SupplementalMaterial2.root");
  fSettings.SetCovarInput(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pi0Np/SupplementalMaterial2.root");

  // Data format is in a TList, so write our own way to get to the histogram and covariance
  TFile *input = new TFile(fSettings.GetDataInput().c_str(), "read");
  TList *li = (TList*)input->Get(dataname.c_str());
  // Data is always zero entry
  fDataHist = (TH1D*)li->At(0);
  fDataHist->SetDirectory(0);
  fDataHist->SetNameTitle((fSettings.GetName() + "_data").c_str(), (fSettings.GetFullTitles()).c_str());
  // Covariance matrix is always 2 entry
  TMatrixD *tempmat = (TMatrixD*)(li->At(2)->Clone());
  // The first and last entry in the cov matrix is empty... basically just padding
  TMatrixDSym *newmat = new TMatrixDSym(tempmat->GetNrows()-2);
  for (int i = 0; i < newmat->GetNrows(); i++) {
    for (int j = 0; j < newmat->GetNrows(); j++) {
      (*newmat)(i, j) = (*tempmat)(i+1, j+1)*1E76; // Scale by 1E76 to get into NUISANCE units
    }
  }
  delete tempmat;
  fFullCovar = newmat;
  input->Close();
  delete input;

  covar = StatUtils::GetInvert(fFullCovar, true);
  fDecomp = StatUtils::GetDecomp(fFullCovar);

}

//********************************************************************
MINERvA_CCNpi0Mp_XSec_1DSTV_nu::MINERvA_CCNpi0Mp_XSec_1DSTV_nu(nuiskey samplekey) {
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
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CCNpi0Mp_XSec_1DSTV_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Basically the same as the T2K measurement, but look for pi0 and proton
  fXVar = -999.9;
  if (event->NumFSParticle(13) == 0 || 
      event->NumFSParticle(111) == 0 ||
      event->NumFSParticle(2212) == 0) {
    return;
  }

  // Get the TLorentzVectors from the event
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;
  TLorentzVector Ppi0 = event->GetHMFSParticle(111)->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;

  // Make the z vector (cross between nu and mu vectors)
  // Make it unit length
  TVector3 z = (Pnu.Vect().Cross(Pmu.Vect())).Unit();

  // first make projection along neutrino direction
  double plmu = Pmu.Vect().Dot(  Pnu.Vect().Unit());
  // Hadronic projection (proton and pion)
  double plhad = (Ppi0.Vect()+Pp.Vect()).Dot(Pnu.Vect().Unit());

  // Muon vector in the non-neutrino direction
  TVector3 ptmuvec = Pmu.Vect() - plmu*(Pnu.Vect().Unit());
  // Hadron vector in the non-neutrino direction
  TVector3 pthadvec = (Ppi0.Vect()+Pp.Vect())-plhad*(Pnu.Vect().Unit());
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
      double ptt_pi = Ppi0.Vect().Dot(z);
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
      double ppil = Ppi0.Vect().Dot(Pnu.Vect().Unit());
      double ppl = Pp.Vect().Dot(Pnu.Vect().Unit());
      const double eps = 26.1; // MeV, mean proton excitation energy in Carbon, table 8 of arxiv 1801.07975
      const double mn = PhysConst::mass_neutron*1E3; // neutron mass
      const double mp = PhysConst::mass_proton*1E3;  // proton mass
      const double MA = 6*mn+6*mp-92.16;  // target mass (E is from PhysRevC.95.065501)
      const double MAP = MA-mn+28.7; // remnant mass, but... binding energy?, but MINERvA paper say 28.7 MeV...
      const double temp = MA+pmul+ppil+ppl-Pmu.E()-Ppi0.E()-Pp.E();
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
bool MINERvA_CCNpi0Mp_XSec_1DSTV_nu::isSignal(FitEvent *event) {
  //********************************************************************
  return SignalDef::isCCNpi0Mp_MINERvA_STV(event);
}

