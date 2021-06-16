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

#include "MiniBooNE_NCpi0_XSec_1Dppi0_nu.h"

// The constructor
MiniBooNE_NCpi0_XSec_1Dppi0_nu::MiniBooNE_NCpi0_XSec_1Dppi0_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile, Double_t *anuBins)
    : isComb(false) {
  if (anuBins != NULL)
    isComb = true;
  isComb = false;

};

void MiniBooNE_NCpi0_XSec_1Dppi0_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppi0;

  double EHad = 0;
  pi0Cnt = 0;
  bad_particle = false;

  for (UInt_t j = 2; j < event->Npart(); ++j) {
    if (!((event->PartInfo(j))->fIsAlive) &&
        (event->PartInfo(j))->fNEUTStatusCode != 0)
      continue;
    int PID = (event->PartInfo(j))->fPID;
    double KE = (event->PartInfo(j))->fP.E() - (event->PartInfo(j))->fMass;
    if (PID == 111) {
      Ppi0 = event->PartInfo(j)->fP;
      EHad += KE;
    } else if (PID == 2112 || PID == 2212)
      EHad += KE;
    else if (PID == -13)
      Pmu = event->PartInfo(j)->fP;

    if (abs(PID) >= 113 && abs(PID) <= 557)
      bad_particle = true;
    else if (abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 15 ||
             abs(PID) == 17)
      bad_particle = true;
    else if (PID == 111)
      pi0Cnt++;
  }
  double bind = 34.0;
  if (isComb)
    bind = 30.0;

  // double hadMass = FitUtils::Wrec(Pnu, Pmu, Ppi0);
  double ppi0 = Ppi0.Vect().Mag() / 1000.0;
  fXVar = ppi0;

  return;
};

bool MiniBooNE_NCpi0_XSec_1Dppi0_nu::isSignal(FitEvent *event) {
  return SignalDef::isNC1pi(event, 14, 111, EnuMin, EnuMax);
};

void MiniBooNE_NCpi0_XSec_1Dppi0_nu::SetDataValues(std::string dataFile) {
  NUIS_LOG(SAM, this->fName << "Setting data for " << this->fName);
  NUIS_LOG(SAM, this->fName << "From: " << dataFile);
  NUIS_LOG(SAM, this->fName << "Reading error from covariance");

  TGraph *gr = new TGraph(dataFile.c_str());
  this->fXBins = gr->GetX();
  this->fDataValues = gr->GetY();
  this->fNDataPointsX = gr->GetN();

  // get the diagonal elements
  int rows = (this->tempCovar)->GetNrows();
  Double_t errors[rows + 1];
  for (int i = 0; i < rows; i++)
    errors[i] = sqrt((*this->tempCovar)(i, i) * 1E-81);
  errors[rows] = 0.;
  this->fDataErrors = errors;

  this->fDataHist = new TH1D((this->fName + "_data").c_str(),
                             (this->fName + this->fPlotTitles).c_str(),
                             this->fNDataPointsX - 1, this->fXBins);

  for (int i = 0; i < this->fNDataPointsX; ++i) {
    this->fDataHist->SetBinContent(i + 1, this->fDataValues[i]);
    this->fDataHist->SetBinError(i + 1, this->fDataErrors[i]);
  }
  return;
}

void MiniBooNE_NCpi0_XSec_1Dppi0_nu::SetCovarMatrix(std::string covarFile,
                                                    int dim) {
  NUIS_LOG(SAM, this->fName << "===============");
  NUIS_LOG(SAM, this->fName << "Reading covariance: " << this->fName);
  NUIS_LOG(SAM, this->fName << "From: " << covarFile);
  // tracks line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(), ifstream::in);

  this->tempCovar = new TMatrixDSym(dim);

  // while we're on a line in covar
  while (std::getline(covar >> std::ws, line, '\n')) {
    std::istringstream stream(line);
    // this is the netry we're reading!
    double entry;
    // this is the column counter!
    int column = 0;

    while (stream >> entry) {
      // get the covariance entry.
      // 1E-81 from the data release listing this unit
      double val = entry; // * 1E-81;
      // then fill the covariance matrix's row and column with this value,
      (*this->tempCovar)(row, column) = val;
      column++;
    }
    row++;
  }
  this->covar = (TMatrixDSym *)this->tempCovar->Clone();
  TDecompChol LU = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(dim, LU.Invert().GetMatrixArray(), "");
  (*this->covar) *= 1E81 * 1E-76;

  return;
};
