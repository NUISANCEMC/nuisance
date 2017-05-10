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
#include "MINERvA_CCinc_XSec_1DEnu_ratio.h"


//********************************************************************
MINERvA_CCinc_XSec_1DEnu_ratio::MINERvA_CCinc_XSec_1DEnu_ratio(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CCinc_XSec_1DEnu_ratio sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("E_{#nu} (GeV)");
  fSettings.SetYTitle(" d#sigma/dE_{#nu} (cm^{2}/GeV/nucleon)");
  fSettings.SetAllowedTypes("FIX/DIAG,FULL/MASK", "FIX/FULL");
  fSettings.SetEnuRange(0.0, 20.0);

  // CCQELike plot information
  fSettings.SetTitle("MINERvA_CCinc_XSec_1DEnu_ratio");
  
  fIsRatio = true;
  nBins = 8;

  target  = "";
  if      (fSettings.Found("name", "C12")) target =   "C12";
  else if (fSettings.Found("name", "Fe56")) target =   "Fe56";
  else if (fSettings.Found("name", "Pb208")) target =   "Pb208";
  else {
    ERR(FTL) << "target " << target << " was not found!" << std::endl;
    exit(-1);
  }

  std::string basedir = FitPar::GetDataBase() + "/MINERvA/CCinc/";
  fSettings.SetDataInput(  basedir + "CCinc_" + target + "_CH_ratio_Enu_data.csv" );
  fSettings.SetCovarInput( basedir + "CCinc_" + target + "_CH_ratio_Enu_covar.csv" );
  FinaliseSampleSettings();

  // Get parsed input files
  if (fSubInFiles.size() != 2) ERR(FTL) << "MINERvA CCinc ratio requires input files in format: NUMERATOR;DENOMINATOR" << std::endl;
  std::string inFileNUM = fSubInFiles.at(0);
  std::string inFileDEN = fSubInFiles.at(1);

  // Scaling Setup ---------------------------------------------------
  // Ratio of sub classes so non needed

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCovarFromTextFile(fSettings.GetCovarInput());


  // Setup Experiments  -------------------------------------------------------
  std::string type = samplekey.GetS("type");
  NUM  = new MINERvA_CCinc_XSec_1DEnu_nu("MINERvA_CCinc_XSec_1DEnu_" + target + "_CH_NUM", inFileNUM, type);
  DEN  = new MINERvA_CCinc_XSec_1DEnu_nu("MINERvA_CCinc_XSec_1DEnu_" + target + "_CH_DEN", inFileDEN, type);
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
void MINERvA_CCinc_XSec_1DEnu_ratio::MakePlots() {
//********************************************************************

  UInt_t sample = 0;
  for (std::vector<MeasurementBase*>::const_iterator expIter = this->fSubChain.begin(); expIter != this->fSubChain.end(); expIter++) {
    MeasurementBase* exp = static_cast<MeasurementBase*>(*expIter);

    if      (sample == 0) this->NUM = static_cast<MINERvA_CCinc_XSec_1DEnu_nu*>(exp);
    else if (sample == 1) this->DEN = static_cast<MINERvA_CCinc_XSec_1DEnu_nu*>(exp);
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


//********************************************************************
void MINERvA_CCinc_XSec_1DEnu_ratio::SetCovarMatrixFromText(std::string covarFile, int dim) {
//********************************************************************

  // WARNING this reads in the data CORRELATIONS
  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(), ifstream::in);

  this->fFullCovar = new TMatrixDSym(dim);
  if (covar.is_open()) LOG(SAM) << "Reading covariance matrix from file: " << covarFile << std::endl;
  else ERR(FTL) << "Covariance matrix provided is incorrect: " << covarFile << std::endl;

  while (std::getline(covar >> std::ws, line, '\n')) {
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {

      double val = (*iter) * this->fDataHist->GetBinError(row + 1) * this->fDataHist->GetBinError(column + 1);

      (*this->fFullCovar)(row, column) = val;
      column++;
    }
    row++;
  }

  // Robust matrix inversion method
  this->covar = StatUtils::GetInvert(fFullCovar);

  return;
};


