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

// Implementation of 2018 MINERvA numu CC0pi STV analysis
// arxiv 1805.05486.pdf
// Clarence Wret
// cwret@fnal.gov
// Stephen Dolan
// Stephen.Dolan@llr.in2p3.fr

#include "MINERvA_CC0pinp_STV_XSec_1D_nu.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
void MINERvA_CC0pinp_STV_XSec_1D_nu::SetupDataSettings() {
  //********************************************************************
  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");
  if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dpmu_nu"))
    fDist = kMuonMom;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dthmu_nu"))
    fDist = kMuonTh;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dpprot_nu"))
    fDist = kPrMom;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dthprot_nu"))
    fDist = kPrTh;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Dpnreco_nu"))
    fDist = kNeMom;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Ddalphat_nu"))
    fDist = kDalphaT;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Ddpt_nu"))
    fDist = kDpT;
  else if (!name.compare("MINERvA_CC0pinp_STV_XSec_1Ddphit_nu"))
    fDist = kDphiT;

  // Location of data, correlation matrices and the titles
  std::string titles = "MINERvA_CC0pinp_STV_XSec_1D";
  std::string foldername;
  std::string distdescript;

  // Data release is a single file
  std::string rootfile = "MINERvA_1805.05486.root";

  fMin = -999;
  fMax = 999;

  switch (fDist) {
  case (kMuonMom):
    titles += "pmu";
    foldername = "muonmomentum";
    distdescript = "Muon momentum in lab frame";
    /*
    fMin = 2.0;
    fMax = 6.0;
    */
    fMin = 1.5;
    fMax = 10.0;
    break;
  case (kMuonTh):
    titles += "thmu";
    foldername = "muontheta";
    distdescript = "Muon angle relative neutrino in lab frame";
    fMin = 0.0;
    fMax = 20.0;
    break;
  case (kPrMom):
    titles += "pprot";
    foldername = "protonmomentum";
    distdescript = "Proton momentum in lab frame";
    // fMin = 0.5;
    fMin = 0.45;
    fMax = 1.2;
    break;
  case (kPrTh):
    titles += "thprot";
    foldername = "protontheta";
    distdescript = "Proton angle relative neutrino in lab frame";
    fMin = 0.0;
    fMax = 70.0;
    break;
  case (kNeMom):
    titles += "pnreco";
    foldername = "neutronmomentum";
    distdescript = "Neutron momentum in lab frame";
    fMin = 0.0;
    // fMax = 0.9;
    fMax = 2.0;
    break;
  case (kDalphaT):
    foldername = "dalphat";
    titles += foldername;
    distdescript = "Delta Alpha_T";
    fMin = 0.0;
    // fMax = 170;
    fMax = 180;
    break;
  case (kDpT):
    foldername = "dpt";
    titles += foldername;
    distdescript = "Delta p_T";
    fMin = 0.0;
    fMax = 2.0;
    break;
  case (kDphiT):
    foldername = "dphit";
    titles += foldername;
    distdescript = "Delta phi_T";
    fMin = 0.0;
    // fMax = 60.0;
    fMax = 180.0;
    break;
  default:
    NUIS_ERR(FTL,
           "Did not find your specified distribution implemented, exiting");
    NUIS_ABORT("You gave " << fName);
  }

  titles += "_nu";

  // All have the same name
  std::string dataname = foldername;

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript +
                        "Target: CH \n"
                        "Flux: MINERvA Forward Horn Current numu ONLY \n"
                        "Signal: Any event with 1 muon, and 0pi0 in FS, no "
                        "mesons, at least one proton with: \n"
                        "1.5GeV < p_mu < 10 GeV\n"
                        "theta_mu < 20 degrees\n"
                        "0.45GeV < p_prot < 1.2 GeV\n"
                        "theta_prot < 70 degrees\n"
                        "arXiv 1805.05486";
  fSettings.SetDescription(descrip);

  std::string filename =
      GeneralUtils::GetTopLevelDir() +
      "/data/MINERvA/CC0pi/CC0pi_STV/MINERvA_1805.05486.root";
  // Specify the data
  fSettings.SetDataInput(filename + ";" + dataname);
  // And the correlations
  fSettings.SetCovarInput(filename + ";" + dataname);

  // Set titles
  fSettings.SetTitle(titles);
};

//********************************************************************
MINERvA_CC0pinp_STV_XSec_1D_nu::MINERvA_CC0pinp_STV_XSec_1D_nu(
    nuiskey samplekey) {
  //********************************************************************

  // A few different distributinos
  // Muon momentum, muon angle, proton momentum, proton angle, neutron momentum,
  // dalphat, dpt, dphit


  // Hard-code the cuts
  ProtonMinCut = 450; // MeV
  ProtonMaxCut = 1200; // MeV
  ProtonThetaCut = 70; // degrees

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);

  // Load up the data paths and sample descriptions
  SetupDataSettings();

  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  // No Enu cut
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  // Finalise the settings
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) /
                 (double(fNEvents) * TotalIntegratedFlux("width"));

  // Set the data and covariance matrix
  SetDataFromRootFile(fSettings.GetDataInput());
  SetCovarianceFromRootFile(fSettings.GetCovarInput());

  fSettings.SetXTitle(fDataHist->GetXaxis()->GetTitle());
  fSettings.SetYTitle(fDataHist->GetYaxis()->GetTitle());

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
// Data comes in a TList
// Some bins need stripping out because of zero bin content. Why oh why
void MINERvA_CC0pinp_STV_XSec_1D_nu::SetDataFromRootFile(std::string filename) {
  //********************************************************************
  std::vector<std::string> tempfile = GeneralUtils::ParseToStr(filename, ";");
  TFile *File = new TFile(tempfile[0].c_str(), "READ");
  // First object is the data
  TH1D *temp = (TH1D *)(((TList *)(File->Get(tempfile[1].c_str())))->At(0));

  // Garh, some of the data points are zero in the TH1D (WHY?!) so messes with
  // the covariance entries to data bins check Skim through the data and check
  // for zero bins
  std::vector<double> CrossSection;
  std::vector<double> Error;
  std::vector<double> BinEdges;
  int lastbin = 0;
  startbin = 0;
  for (int i = 0; i < temp->GetXaxis()->GetNbins() + 2; ++i) {
    if (temp->GetBinContent(i + 1) > 0 && temp->GetBinLowEdge(i + 1) >= fMin &&
        temp->GetBinLowEdge(i + 1) <= fMax) {
      if (startbin == 0)
        startbin = i;
      lastbin = i;
      CrossSection.push_back(temp->GetBinContent(i + 1));
      BinEdges.push_back(temp->GetXaxis()->GetBinLowEdge(i + 1));
      Error.push_back(temp->GetBinError(i + 1));
    }
  }
  BinEdges.push_back(temp->GetXaxis()->GetBinLowEdge(lastbin + 2));

  fDataHist = new TH1D((fSettings.GetName() + "_data").c_str(),
                       (fSettings.GetFullTitles()).c_str(), BinEdges.size() - 1,
                       &BinEdges[0]);
  fDataHist->SetDirectory(0);
  for (unsigned int i = 0; i < BinEdges.size() - 1; ++i) {
    fDataHist->SetBinContent(i + 1, CrossSection[i]);
    fDataHist->SetBinError(i + 1, Error[i]);
  }
  fDataHist->GetXaxis()->SetTitle(temp->GetXaxis()->GetTitle());
  fDataHist->GetYaxis()->SetTitle(temp->GetYaxis()->GetTitle());
  fDataHist->SetTitle(temp->GetTitle());

  File->Close();
}

//********************************************************************
// Covariance also needs stripping out
// There's padding (two bins...) and overflow (last bin before the two empty
// bins)
void MINERvA_CC0pinp_STV_XSec_1D_nu::SetCovarianceFromRootFile(
    std::string filename) {
  //********************************************************************
  std::vector<std::string> tempfile = GeneralUtils::ParseToStr(filename, ";");
  TFile *File = new TFile(tempfile[0].c_str(), "READ");
  // First object is the data, second is data with statistical error only, third
  // is the covariance matrix
  TMatrixDSym *tempcov =
      (TMatrixDSym *)((TList *)File->Get(tempfile[1].c_str()))->At(2);
  // Count the number of zero entries
  int ngood = 0;
  int nstart = -1;
  int nend = -1;
  // Scan through the middle bin and look for entries
  int middle = tempcov->GetNrows() / 2;
  int nbinsdata = fDataHist->GetXaxis()->GetNbins();

  for (int j = 0; j < tempcov->GetNrows(); ++j) {
    if ((*tempcov)(middle, j) > 0 && ngood < nbinsdata) {
      ngood++;
      if (nstart == -1)
        nstart = j;
      if (j > nend)
        nend = j;
    }
  }

  fFullCovar = new TMatrixDSym(ngood);
  for (int i = 0; i < fFullCovar->GetNrows(); ++i) {
    for (int j = 0; j < fFullCovar->GetNrows(); ++j) {
      (*fFullCovar)(i, j) =
          (*tempcov)(i + nstart + startbin - 1, j + nstart + startbin - 1);
    }
  }
  (*fFullCovar) *= 1E38 * 1E38;

  File->Close();

  // Fill other covars.
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
}

void MINERvA_CC0pinp_STV_XSec_1D_nu::FillEventVariables(FitEvent *event) {

  fXVar = -999.99;
  // Need a proton and a muon
  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;
  // Find the highest momentum proton in the event between 450 and 1200 MeV with
  // theta_p < 70
  TLorentzVector Pprot = FitUtils::GetProtonInRange(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI));

  switch (fDist) {
  case (kMuonMom):
    fXVar = Pmu.Vect().Mag() / 1000.0;
    break;
  case (kMuonTh):
    fXVar = Pmu.Vect().Angle(Pnu.Vect()) * (180.0 / M_PI);
    break;
  case (kPrMom):
    fXVar = Pprot.Vect().Mag() / 1000.0;
    break;
  case (kPrTh):
    fXVar = Pprot.Vect().Angle(Pnu.Vect()) * (180.0 / M_PI);
    break;
    // Use Stephen's validated functions
  case (kNeMom):
    fXVar = FitUtils::Get_pn_reco_C_protonps(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true);
    break;
  case (kDalphaT):
    fXVar = FitUtils::Get_STV_dalphat_protonps(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) * (180.0 / M_PI);
    break;
  case (kDpT):
    fXVar = FitUtils::Get_STV_dpt_protonps(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) / 1000.0;
    break;
  case (kDphiT):
    fXVar = FitUtils::Get_STV_dphit_protonps(event, ProtonMinCut, ProtonMaxCut, cos(ProtonThetaCut/180.0*M_PI), 14, true) * (180.0 / M_PI);
    break;
  }
  return;
};

//********************************************************************
bool MINERvA_CC0pinp_STV_XSec_1D_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isCC0piNp_MINERvA_STV(event, EnuMin, EnuMax);
}
