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

#include "MINERvA_CCQE_XSec_1DQ2_joint.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
MINERvA_CCQE_XSec_1DQ2_joint::MINERvA_CCQE_XSec_1DQ2_joint(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCQE_XSec_1DQ2_joint sample. \n"
                        "Target: CH \n"
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n"
                        "Signal: Any event with 1 electron, any nucleons, and "
                        "no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle("d#sigma/dQ_{QE}^{2} (cm^{2}/GeV^{2})");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  isFluxFix = !fSettings.Found("name", "_oldflux");
  fullphasespace = !fSettings.Found("name", "_20deg");
  nBins = 16;
  fIsRatio = false;
  fIsSummed = false;
  fSaveSubMeas = true;

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CCQE_XSec_1DQ2_joint");

  std::string basedir = FitPar::GetDataBase() + "/MINERvA/CCQE/";
  std::string datafilename = "";
  std::string covarfilename = "";
  std::string neutrinoclass = "";
  std::string antineutrinoclass = "";

  // Full Phase Space
  if (fullphasespace) {

    if (isFluxFix) {
      if (fIsShape) {
        NUIS_ERR(WRN,
               "SHAPE likelihood comparison not available for MINERvA "
                   << "datasets with fixed flux information. NUISANCE will "
                      "scale MC to match "
                   << "data normalization but full covariance will be used. ");
      }

      datafilename = "Q2QE_joint_data_fluxfix.txt";
      covarfilename = "Q2QE_joint_covar_fluxfix.txt";
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_newflux";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_newflux";

    } else {
      if (fIsShape) {
        datafilename = "Q2QE_joint_dataa_SHAPE-extracted.txt";
        covarfilename = "Q2QE_joint_covara_SHAPE-extracted.txt";
      } else {
        datafilename = "Q2QE_joint_data.txt";
        covarfilename = "Q2QE_joint_covar.txt";
      }
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu";
    }

    // Restricted Phase Space
  } else {

    if (isFluxFix) {
      if (fIsShape) {
        NUIS_ERR(WRN,
               "SHAPE likelihood comparison not available for MINERvA "
                   << "datasets with fixed flux information. NUISANCE will "
                      "scale MC to match "
                   << "data normalization but full covariance will be used. ");
      }

      datafilename = "20deg_Q2QE_joint_data_fluxfix.txt";
      covarfilename = "20deg_Q2QE_joint_covar_fluxfix.txt";
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_20deg_newflux";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_20deg_newflux";

    } else {
      if (fIsShape) {
        datafilename = "20deg_Q2QE_joint_dataa_SHAPE-extracted.txt";
        covarfilename = "20deg_Q2QE_joint_covara_SHAPE-extracted.txt";
      } else {
        datafilename = "20deg_Q2QE_joint_data.txt";
        covarfilename = "20deg_Q2QE_joint_covar.txt";
      }
      neutrinoclass = "MINERvA_CCQE_XSec_1DQ2_nu_20deg";
      antineutrinoclass = "MINERvA_CCQE_XSec_1DQ2_antinu_20deg";
    }
  }

  fSettings.SetDataInput(basedir + datafilename);
  fSettings.SetCovarInput(basedir + covarfilename);
  fSettings.DefineAllowedSpecies("numu,numub");

  std::cout << "Finalising sample settings for joint fit = " << fIsJoint
            << std::endl;
  FinaliseSampleSettings();

  // Get parsed input files
  if (fSubInFiles.size() != 2) {
    NUIS_ABORT("MINERvA Joint requires input files in format: antinu;nu");
  }
  std::string inFileAntineutrino = fSubInFiles.at(0);
  std::string inFileNeutrino = fSubInFiles.at(1);

  // Push classes back into list for processing loop
  fSubChain.push_back(MIN_anu);
  fSubChain.push_back(MIN_nu);

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile(fSettings.GetDataInput());
  if (fullphasespace and isFluxFix)
    SetCovarFromTextFile(fSettings.GetCovarInput());
  else {
    SetCorrelationFromTextFile(fSettings.GetCovarInput());
  }

  // Setup Sub classes
  nuiskey antinukey = Config::CreateKey("sample");
  antinukey.SetS("name", antineutrinoclass);
  antinukey.SetS("input", inFileAntineutrino);
  antinukey.SetS("type", fSettings.GetS("type"));
  MIN_anu = new MINERvA_CCQE_XSec_1DQ2_antinu(antinukey);

  nuiskey nukey = Config::CreateKey("sample");
  nukey.SetS("name", neutrinoclass);
  nukey.SetS("input", inFileNeutrino);
  nukey.SetS("type", fSettings.GetS("type"));
  MIN_nu = new MINERvA_CCQE_XSec_1DQ2_nu(nukey);

  // Add to chain for processing
  this->fSubChain.clear();
  this->fSubChain.push_back(MIN_anu);
  this->fSubChain.push_back(MIN_nu);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();
};

//********************************************************************
void MINERvA_CCQE_XSec_1DQ2_joint::MakePlots() {
  //********************************************************************

  UInt_t sample = 0;
  for (std::vector<MeasurementBase *>::const_iterator expIter =
           fSubChain.begin();
       expIter != fSubChain.end(); expIter++) {
    MeasurementBase *exp = static_cast<MeasurementBase *>(*expIter);

    if (sample == 0) {

      MIN_anu = static_cast<MINERvA_CCQE_XSec_1DQ2_antinu *>(exp);
      TH1D *MIN_anu_mc = (TH1D *)MIN_anu->GetMCList().at(0);
      for (int i = 0; i < 8; i++) {
        fMCHist->SetBinContent(i + 1, MIN_anu_mc->GetBinContent(i + 1));
        fMCHist->SetBinError(i + 1, MIN_anu_mc->GetBinError(i + 1));
      }
    } else if (sample == 1) {

      MIN_nu = static_cast<MINERvA_CCQE_XSec_1DQ2_nu *>(exp);
      TH1D *MIN_nu_mc = (TH1D *)MIN_nu->GetMCList().at(0);
      for (int i = 0; i < 8; i++) {
        fMCHist->SetBinContent(i + 1 + 8, MIN_nu_mc->GetBinContent(i + 1));
        fMCHist->SetBinError(i + 1 + 8, MIN_nu_mc->GetBinError(i + 1));
      }
    }
    sample++;
  }

  return;
}
