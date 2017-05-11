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
#include "MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu.h"


//********************************************************************
MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu::MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu sample. \n" \
                        "Target: Target;CH (2 INPUTS)\n" \
                        "Flux: MINERvA Forward Horn Current Numu \n" \
    "Signal: Any event with 1 muon, 1 proton p>450, no pions";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2}_{QE} (GeV^{2})");
  fSettings.SetYTitle(" d#sigma/dQ^{2}_{QE} (cm^{2}/GeV^{2}/nucleon)");
  fSettings.SetAllowedTypes("FIX/DIAG,FULL/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 100.0);

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu");
  
  fIsRatio = true;
  nBins = 5;

  target  = "";
  if (fSettings.Found("name", "TgtRatioFe")) target =   "Fe";
  else if (fSettings.Found("name", "TgtRatioPb")) target =   "Pb";
  else if      (fSettings.Found("name", "TgtRatioC")) target =   "C";
  else {
    ERR(FTL) << "target " << target << " was not found!" << std::endl;
    exit(-1);
  }

  std::string basedir = FitPar::GetDataBase() + "/MINERvA/CC0pi/";
  fSettings.SetDataInput(  basedir + "Q2_TgtRatio_" + target + "_data.txt");
  fSettings.SetCovarInput( basedir + "Q2_TgtRatio_" + target + "_covar.txt");
  FinaliseSampleSettings();

  // Get parsed input files
  if (fSubInFiles.size() != 2) ERR(FTL) << "MINERvA CC0pi ratio requires input files in format: NUMERATOR;DENOMINATOR" << std::endl;
  std::string inFileNUM = fSubInFiles.at(0);
  std::string inFileDEN = fSubInFiles.at(1);

  // Scaling Setup ---------------------------------------------------
  // Ratio of sub classes so non needed

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile(fSettings.GetCovarInput());


  // Setup Experiments  -------------------------------------------------------
  std::string type = samplekey.GetS("type");

  nuiskey samplekey_num = Config::CreateKey("sample");
  samplekey_num.AddS("name", "MINERvA_CC0pi_XSec_1DQ2_Tgt" + target + "_nu");
  samplekey_num.AddS("input", inFileNUM);
  samplekey_num.AddS("type", type);

  nuiskey samplekey_den = Config::CreateKey("sample");
  samplekey_den.AddS("name", "MINERvA_CC0pi_XSec_1DQ2_TgtCH_nu");
  samplekey_den.AddS("input", inFileDEN);
  samplekey_den.AddS("type", type);

  NUM  = new MINERvA_CC0pi_XSec_1DQ2_Tgt_nu(samplekey_num);
  DEN  = new MINERvA_CC0pi_XSec_1DQ2_Tgt_nu(samplekey_den);
  NUM  ->SetNoData();
  DEN  ->SetNoData();

  // Add to chain for processing
  this->fSubChain.clear();
  this->fSubChain.push_back(NUM);
  this->fSubChain.push_back(DEN);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

//********************************************************************
void MINERvA_CC0pi_XSec_1DQ2_TgtRatio_nu::MakePlots() {
//********************************************************************

  UInt_t sample = 0;
  for (std::vector<MeasurementBase*>::const_iterator expIter = this->fSubChain.begin(); expIter != this->fSubChain.end(); expIter++) {
    MeasurementBase* exp = static_cast<MeasurementBase*>(*expIter);

    if      (sample == 0) this->NUM = static_cast<MINERvA_CC0pi_XSec_1DQ2_Tgt_nu*>(exp);
    else if (sample == 1) this->DEN = static_cast<MINERvA_CC0pi_XSec_1DQ2_Tgt_nu*>(exp);
    else break;
    sample++;
  }

  // Now make the ratio histogram
  TH1D* NUM_MC = (TH1D*)this->NUM->GetMCList().at(0)->Clone();
  TH1D* DEN_MC = (TH1D*)this->DEN->GetMCList().at(0)->Clone();

  for (int i = 0; i < nBins; ++i) {
    double binVal = 0;
    double binErr = 0;

    if (DEN_MC->GetBinContent(i + 1) && NUM_MC->GetBinContent(i + 1)) {
      binVal = NUM_MC->GetBinContent(i + 1) / DEN_MC->GetBinContent(i + 1);
      double fractErrNUM = NUM_MC->GetBinError(i + 1) / NUM_MC->GetBinContent(i + 1);
      double fractErrDEN = DEN_MC->GetBinError(i + 1) / DEN_MC->GetBinContent(i + 1);
      binErr = binVal * sqrt(fractErrNUM * fractErrNUM + fractErrDEN * fractErrDEN);
    }

    this->fMCHist->SetBinContent(i + 1, binVal);
    this->fMCHist->SetBinError(i + 1, binErr);
  }

  return;
}

