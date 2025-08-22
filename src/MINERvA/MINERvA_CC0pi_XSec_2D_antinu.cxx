// Adrian Orea
// I used the file MINERvA_CCinc_XSec_2DEavq3_nu.cxx as a template
// Also, I am fully aware of the naming typo (should be ptpz), but Everything is already named the same way so...

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
  Author : Adrian Orea
           Clarence Wret Cleanup 2019: Data was missing
                                       Signal definition wrong (missing 120 MeV KE cut, missing pz and pt cut)
                                       Not fully implemented
                                       Assumed generator send neutrinos along z-axis

                                       Added ME implementation
*/

#include "MINERvA_SignalDef.h"
#include "MINERvA_CC0pi_XSec_2D_antinu.h"


//********************************************************************  
void MINERvA_CC0pi_XSec_2D_antinu::SetupDataSettings() {
//********************************************************************  

  // Set Distribution
  // See header file for enum and some descriptions
  std::string name = fSettings.GetS("name");

  // Has user requested LE or ME
  IsME = true;
  if (name.find("MINERvA_CC0pi_XSec_2D") != std::string::npos) {
    if (name.find("_ME_") != std::string::npos) IsME = true;
    else if (name.find("_LE_") != std::string::npos) IsME = true;
  }

  if (IsME) {
    std::cout << "Medium energy implementation does not support covariance" << std::endl;
  }

  // We're lucky to have three different MINERvA CC0pi anti-numu 2D distributions, only for LE
  if      (!name.compare("MINERvA_CC0pi_XSec_2Dptpz_antinu"))        fDist = kPtPz;
  else if (!name.compare("MINERvA_CC0pi_XSec_2DQ2QEEnuQE_antinu"))   fDist = kQ2QEEnuQE;
  else if (!name.compare("MINERvA_CC0pi_XSec_2DQ2QEEnuTrue_antinu")) fDist = kQ2QEEnuTrue;

  if (IsME && fDist != kPtPz) {
    std::cerr << "Unsupported measurement for " << name << std::endl;
    std::cerr << "ME implementation only available for ptpz, you chose " << fDist << std::endl;
    throw;
  }

  // Define what files to use from the dist
  std::string basedir = "MINERvA/CC0pi_2D_antinu/";
  std::string datafile = basedir;
  std::string covfile = basedir;
  std::string titles = "";
  std::string distdescript = "";
  std::string histname = "";
  std::string xbinning = basedir;
  std::string ybinning = basedir;

  // N.B. fScaleFactor also needs to be set dependent on the distribution. The EnuQE and EnuTrue distributions flux integrate in the Enu dimension and flux average in the Q2 dimension
  switch (fDist) {
    case (kPtPz):
      distdescript = "MINERvA_CC0pi_XSec_2Dptpz_antinu sample";
      if (IsME) {
        distdescript += " ME";
        datafile += "data_release_pzpt.root";
        titles    = "MINERvA CC0#pi ME #bar{#nu}_{#mu} p_{z} p_{t};p_{z} (GeV);p_{t} (GeV);d^{2}#sigma/dp_{z}dp_{t} (cm^{2}/GeV^{2}/nucleon)";
      } else {
        distdescript += " LE";
      datafile += "cross_sections_muonpz_muonpt_lowangleqelike_minerva_2d.csv";
      covfile += "cross_sections_muonpz_muonpt_lowangleqelike_minerva_covariance.csv";
      xbinning += "cross_sections_muonpt_lowangleqelike_minerva_intmuonpz_bins_1d.csv";
      ybinning += "cross_sections_muonpz_lowangleqelike_minerva_intmuonpt_bins_1d.csv";
      titles    = "MINERvA CC0#pi LE #bar{#nu}_{#mu} p_{t} p_{z};p_{t} (GeV);p_{z} (GeV);d^{2}#sigma/dp_{t}dp_{z} (cm^{2}/GeV^{2}/nucleon)";
      }
      fScaleFactor  = (GetEventHistogram()->Integral("width") * 1E-38 / (fNEvents + 0.)) / this->TotalIntegratedFlux();
      break;
    case (kQ2QEEnuQE):
      datafile += "cross_sections_enuqe_qsq_lowangleqelike_minerva_2d.csv";
      covfile += "cross_sections_enuqe_qsq_lowangleqelike_minerva_covariance.csv";
      xbinning += "cross_sections_qsq_lowangleqelike_minerva_intenuqe_bins_1d.csv";
      ybinning += "cross_sections_enuqe_lowangleqelike_minerva_intqsq_bins_1d.csv";
      titles    = "MINERvA CC0#pi #bar{#nu}_{#mu} Q^{2}_{QE} E^{#nu}_{QE};Q^{2}_{QE} (GeV);E^{#nu}_{QE} (GeV);d#sigma/dQ^{2}_{QE} (cm^{2}/GeV^{2}/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_2DQ2QEEnuQE_antinu sample";
      fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);
      break;
    case (kQ2QEEnuTrue):
      datafile += "cross_sections_enu_qsq_lowangleqelike_minerva_2d.csv";
      covfile += "cross_sections_enu_qsq_lowangleqelike_minerva_covariance.csv";
      xbinning += "cross_sections_qsq_lowangleqelike_minerva_intenuqe_bins_1d.csv";
      ybinning += "cross_sections_enuqe_lowangleqelike_minerva_intqsq_bins_1d.csv";
      titles    = "MINERvA CC0#pi #bar{#nu}_{#mu} Q^{2}_{QE} E^{#nu}_{True};Q^{2}_{QE} (GeV);E^{#nu}_{True} (GeV);d#sigma/dQ^{2}_{QE} (cm^{2}/GeV^{2}/nucleon)";
      distdescript = "MINERvA_CC0pi_XSec_2DQ2QEEnuTrue_antinu sample";
      fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents);
      break;
    default:
      NUIS_ABORT("Unknown Analysis Distribution : " << name);
  }

  fSettings.SetTitle(  GeneralUtils::ParseToStr(titles,";")[0] );
  fSettings.SetXTitle( GeneralUtils::ParseToStr(titles,";")[1] );
  fSettings.SetYTitle( GeneralUtils::ParseToStr(titles,";")[2] );
  fSettings.SetZTitle( GeneralUtils::ParseToStr(titles,";")[3] );

  // Sample overview ---------------------------------------------------
  std::string descrip = distdescript + "\n"\
                        "Target: CH \n";
  
  if (IsME) {
    descrip += "Flux: MINERvA Medium Energy RHC anti-numu  \n";
  } else {
    descrip += "Flux: MINERvA Low Energy RHC anti-numu  \n";
  }
  descrip += "Signal: CC-0pi \n";
  fSettings.SetDescription(descrip);

  // The input ROOT file in the fSettings
  fSettings.SetDataInput(FitPar::GetDataBase() + datafile);

  if (IsME) SetDataValues(fSettings.GetDataInput(), "data_xsection_with_totErr_norm");
  else {
    fSettings.SetCovarInput(FitPar::GetDataBase() + covfile);
    // Save the binning used in the sample settings
    fSettings.SetS("xbins", FitPar::GetDataBase() + xbinning);
    fSettings.SetS("ybins", FitPar::GetDataBase() + ybinning);

    // Sets up the data from the data file, x binning and y binning
    SetDataFromTextFile(fSettings.GetDataInput(), 
        fSettings.GetS("xbins"),
        fSettings.GetS("ybins"));

    // The data comes in units of 1E-41
    fDataHist->Scale(1E-41);

    // Setup the covariance matrix
    SetCovarFromTextFile(fSettings.GetCovarInput(), fDataHist->GetNbinsX()*fDataHist->GetNbinsY());

    // Set the error on the data from the covariance matrix
    StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, (TH2I*)NULL, 1.E-41, true);

    // In NUISANCE we assume the covar scale is 1E76 (or cross-section in 1E-38)
    // For this measurement it's actually 1E41*1E41=1E82 so need to multiply 1E82/1E76=1E6
    double ScalingFactor = 1E-3*1E-3;
    (*fFullCovar) *= ScalingFactor;
    (*covar) *= 1./ScalingFactor;
    (*fDecomp) *= 1./ScalingFactor;
  }
};

//********************************************************************
MINERvA_CC0pi_XSec_2D_antinu::MINERvA_CC0pi_XSec_2D_antinu(nuiskey samplekey) {
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
void MINERvA_CC0pi_XSec_2D_antinu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  // Checking to see if there is a Muon
  if (event->NumFSParticle(-13) == 0) return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(-13)->fP;

  switch (fDist) {
    case (kPtPz):
      {
        Double_t px = Pmu.X()/1.E3;
        Double_t py = Pmu.Y()/1.E3;
        Double_t pt = sqrt(px*px+py*py);

        // Don't want to assume the event generators all have neutrino coming along z
        // pz is muon momentum projected onto the neutrino direction
        Double_t pz = Pmu.Vect().Dot(Pnu.Vect()*(1.0/Pnu.Vect().Mag()))/1.E3;

        // Funnily, x and y-axis are flipped for ME and LE
        if (IsME) {
          fYVar = pt;
          fXVar = pz;
        } else {
          fYVar = pz;
          fXVar = pt;
        }
        break;
      }
    case (kQ2QEEnuQE):
      {
        double Q2qeRec = FitUtils::Q2QErec(Pmu, Pnu, 30, false);
        double EnuQErec = FitUtils::EnuQErec(Pmu, Pnu, 30, false);
        fXVar = Q2qeRec;
        fYVar = EnuQErec;
        break;
      }
    case (kQ2QEEnuTrue):
      {
        double Q2qeRec = FitUtils::Q2QErec(Pmu, Pnu, 30, false);
        double EnuTrue = Pnu.E()/1.E3;
        fXVar = Q2qeRec;
        fYVar = EnuTrue;
        break;
      }
  }
};

//********************************************************************
bool MINERvA_CC0pi_XSec_2D_antinu::isSignal(FitEvent *event) {
  //********************************************************************
  if (IsME) {
    return SignalDef::isCC0pi_anti_MINERvAPTPZ_ME(event, -14, EnuMin, EnuMax);
  }
  return SignalDef::isCC0pi_anti_MINERvAPTPZ(event, -14, EnuMin, EnuMax);
};
