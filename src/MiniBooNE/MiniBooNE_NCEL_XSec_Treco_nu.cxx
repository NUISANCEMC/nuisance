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

#include "MiniBooNE_NCEL_XSec_Treco_nu.h"
#include "TLorentzVector.h"

//********************************************************************
MiniBooNE_NCEL_XSec_Treco_nu::MiniBooNE_NCEL_XSec_Treco_nu(nuiskey samplekey) {
  //********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MiniBooNE_NCEL_XSec_Treco_nu sample. \n"
                        "Target: CH \n"
                        "Flux: MiniBooNE Numu Flux \n"
                        "Signal: Any event with True NCEL modes \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("T_{reco} (MeV)");
  fSettings.SetYTitle("Events/(12 MeV)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/FULL,DIAG/NORM/MASK",
                            "FIX/FULL,DIAG");
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  fSettings.SetEnuRange(0.0, 10.0);

  double arr_treco[52] = {
      40.0,  52.0,  63.9,  75.9,  87.8,  99.8,  111.8, 123.7, 135.7,
      147.6, 159.6, 171.6, 183.5, 195.5, 207.5, 219.4, 231.4, 243.3,
      255.3, 267.3, 279.2, 291.2, 303.1, 315.1, 327.1, 339.0, 351.0,
      362.9, 374.9, 386.9, 398.8, 410.8, 422.7, 434.7, 446.7, 458.6,
      470.6, 482.5, 494.5, 506.5, 518.4, 530.4, 542.4, 554.3, 566.3,
      578.2, 590.2, 602.2, 614.1, 626.1, 638.0, 650.0};

  SetDataValues(FitPar::GetDataBase() + "/MiniBooNE/ncqe/input_data.txt",
                arr_treco);
  SetCovarMatrix(FitPar::GetDataBase() + "/MiniBooNE/ncqe/ErrorMatrix.tab", 51);
  SetResponseMatrix(FitPar::GetDataBase() + "/MiniBooNE/ncqe/response_mat.txt",
                    51, arr_treco);
  SetFluxHistogram(FitPar::GetDataBase() + "/MiniBooNE/ncqe/flux.txt");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // The scale factor is quite complicated because MB didn't divide by number of
  // targets. nMolMB is the number of CH_2 molecules in the MB FV (610.6 cm
  // radius sphere) and 0.845 is the published density of the mineral oil.
  // UPDATE: nMolMB is the number of CH_2 molecules in the MB FV (500 cm radius
  // sphere) and 0.845 is the published density of the mineral oil. UPDATE
  // UPDATE: They didn't account for fiducial cut, so neither do we.
  double nMolMB = 6.023E+23 * 0.845 * 4.0 * M_PI * 610.6 * 610.6 * 610.6 / 3.0;
  double POT = 6.46e20;

  // Need to update scalefactor to reflect actual flux used
  fScaleFactor = (this->newFluxHist->Integral("") * POT *
                  (GetEventHistogram()->Integral("width")) * 1E-38 * 14.08 /
                  (fNEvents + 0.)) *
                 nMolMB;
  fScaleFactor /= GetFluxHistogram()->Integral("width");

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

  this->fPlotTitles = fSettings.GetFullTitles();

  // Usually the MCFine histogram is a finer binned version of MC Hist.
  // In this case we need to use it to save the true distribution before
  // smearing.
  if (fMCFine)
    delete fMCFine;
  fMCFine = new TH1D((this->fName + "_Ttrue").c_str(),
                     (this->fName + this->fPlotTitles).c_str(), 50, 0, 900);
};

void MiniBooNE_NCEL_XSec_Treco_nu::Write(std::string arg) {
  // newFluxHist->Write("MB_NCEL_newFlux");
  response_mat->Write("MB_NCEL_response_matrix");

  Measurement1D::Write(arg);
  return;
}

void MiniBooNE_NCEL_XSec_Treco_nu::FillEventVariables(FitEvent *event) {

  double t_raw = 0.0;

  // Loop and add all Tnucleon
  for (UInt_t i = 0; i < event->Npart(); i++) {
    if (event->PartInfo(i)->Status() != kFinalState)
      continue;

    int pdg = event->PartInfo(i)->fPID;
    if (pdg == 2212 || pdg == 2112) {
      t_raw += FitUtils::T(event->PartInfo(i)->fP) * 1.E3;
    }
  }

  fXVar = t_raw;
}

void MiniBooNE_NCEL_XSec_Treco_nu::ScaleEvents() {

  // Now convert Ttrue to Treco...
  for (int treco = 0; treco < 51; ++treco) {
    double total = 0.;
    for (int ttrue = 0; ttrue < 50; ++ttrue)
      total += fMCFine->GetBinContent(ttrue + 1) *
               response_mat->GetBinContent(ttrue + 1, treco + 1);
    fMCHist->SetBinContent(treco + 1, total);
  }

  // Scale
  this->fMCHist->Scale(this->fScaleFactor, "width");
  this->fMCFine->Scale(this->fScaleFactor, "width");
  PlotUtils::ScaleNeutModeArray((TH1 **)fMCHist_PDG, fScaleFactor, "width");

  // Add in the backgrounds...
  for (int treco = 0; treco < 51; ++treco) {
    double total = this->fMCHist->GetBinContent(treco + 1) +
                   this->BKGD_other->GetBinContent(treco + 1) +
                   this->BKGD_irrid->GetBinContent(treco + 1);
    this->fMCHist->SetBinContent(treco + 1, total);
  }
}

bool MiniBooNE_NCEL_XSec_Treco_nu::isSignal(FitEvent *event) {

  // Should put in MB SignalDef eventually
  if (event->Mode != 51 && event->Mode != 52)
    return false;

  // Numu or nue
  if (event->PDGnu() != 14 && event->PDGnu() != 12)
    return false;

  // Enu
  if (event->Enu() < EnuMin * 1000.0 || event->Enu() > EnuMax * 1000.0)
    return false;

  return true;
};

void MiniBooNE_NCEL_XSec_Treco_nu::SetFluxHistogram(std::string dataFile) {
  this->newFluxHist = PlotUtils::GetTH1DFromFile(
      dataFile.c_str(), (this->fName + "Real Flux Hist"), "idgaf");
  return;
}
// Read in the covariance matrix from the file specified in the constructor
void MiniBooNE_NCEL_XSec_Treco_nu::SetCovarMatrix(std::string covarFile,
                                                  int dim) {

  // Use Utils

  // // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(), std::ifstream::in);

  this->covar = new TMatrixDSym(dim);

  if (covar.is_open())
    NUIS_LOG(DEB, "Reading covariance matrix from file: " << covarFile);

  while (std::getline(covar >> std::ws, line, '\n')) {
    std::istringstream stream(line);
    double entry;
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation
    // matrix
    while (stream >> entry) {
      (*this->covar)(row, column) = entry;
      if (row == column)
        this->fDataHist->SetBinError(row + 1, sqrt(entry));
      column++;
    }
    row++;
  }

  // // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");

  return;
};

// Override the usual function in the base class because this is more
// complicated for the NCEL sample...
void MiniBooNE_NCEL_XSec_Treco_nu::SetDataValues(std::string inputFile,
                                                 double *arr_treco) {

  std::string line;
  std::ifstream input(inputFile.c_str(), std::ifstream::in);

  if (input.is_open()) {
    NUIS_LOG(DEB, "Reading data from file: " << inputFile);
  }
  this->fDataHist =
      new TH1D((this->fName + "_data").c_str(),
               (this->fName + this->fPlotTitles).c_str(), 51, arr_treco);
  this->BKGD_other =
      new TH1D((this->fName + "_BKGD_other").c_str(),
               (this->fName + this->fPlotTitles).c_str(), 51, arr_treco);
  this->BKGD_irrid =
      new TH1D((this->fName + "_BKGD_irrid").c_str(),
               (this->fName + this->fPlotTitles).c_str(), 51, arr_treco);
  // To get the nDOF correct...
  this->fNDataPointsX = 52;

  double entry = 0;
  int xBin = 0;

  //    First line is the MB data
  std::getline(input >> std::ws, line, '\n');
  std::istringstream stream1(line);

  while (stream1 >> entry) {
    this->fDataHist->SetBinContent(xBin + 1, entry);
    xBin++;
  }

  //    Second line is "other" backgrounds
  std::getline(input >> std::ws, line, '\n');
  std::istringstream stream2(line);
  entry = 0;
  xBin = 0;
  while (stream2 >> entry) {
    this->BKGD_other->SetBinContent(xBin + 1, entry);
    xBin++;
  }

  //  Third line is the irreducible background
  std::getline(input >> std::ws, line, '\n');
  std::istringstream stream3(line);
  entry = 0;
  xBin = 0;
  while (stream3 >> entry) {
    this->BKGD_irrid->SetBinContent(xBin + 1, entry);
    xBin++;
  }
};

// Read in the response matrix -- thus far, a response matrix is unique to the
// NCEL sample
void MiniBooNE_NCEL_XSec_Treco_nu::SetResponseMatrix(std::string responseFile,
                                                     int dim,
                                                     double *arr_treco) {
  // Make a counter to track the line number
  int xBin = 0;

  std::string line;
  std::ifstream response(responseFile.c_str(), std::ifstream::in);

  // Response matrix: x axis is Ttrue, y axis is Treco
  this->response_mat = new TH2D((this->fName + "_RESPONSE_MATRIX").c_str(),
                                (this->fName + this->fPlotTitles).c_str(), 50,
                                0, 900, 51, arr_treco);

  if (response.is_open()) {
    NUIS_LOG(DEB, "Reading in the response matrix from file: " << responseFile);
  }
  while (std::getline(response, line, '\n')) {
    std::istringstream stream(line);
    double entry;
    int yBin = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation
    // matrix
    while (stream >> entry) {
      this->response_mat->SetBinContent(xBin + 1, yBin + 1, entry);
      yBin++;
    }
    xBin++;
  }
};
