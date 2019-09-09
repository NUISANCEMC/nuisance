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

#include "MiniBooNE_NCpi0_XSec_1Dppi0_nu.h"

// The constructor
MiniBooNE_NCpi0_XSec_1Dppi0_nu::MiniBooNE_NCpi0_XSec_1Dppi0_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile, Double_t *anuBins)
    : isComb(false) {
  if (anuBins != NULL)
    isComb = true;
  isComb = false;

  // Needs Updating

  // // Set pointer to the reweighting engine
  // rw_engine = rw;
  // this->fBeamDistance = 0.541;

  // // Define the energy region
  // this->EnuMin = 0.;
  // this->EnuMax = 4.;

  // // In future read most of these from a card file
  // this->inFile = inputfile;
  // this->fName = "MB_NCpi0_XSec_numu_1Dppi0";
  // this->fPlotTitles = "; p_{#pi^{0}} (GeV/c); d#sigma/dp_{#pi^{0}}
  // (cm^{2}/(GeV/c)/nucleon)";
  // this->SetCovarMatrix(FitPar::GetDataBase()+"/MiniBooNE/nc1pi0/nuppi0xsecerrormatrix.txt",
  // 11);
  // this->SetDataValues(FitPar::GetDataBase()+"/MiniBooNE/nc1pi0/nuppi0xsec_edit.txt");
  // this->fNormError=0.107;

  // if (isComb) {
  //   fName += "_comb";
  //   this->fNDataPointsX = 11;
  //   this->fXBins = anuBins;
  // }

  // this->fMCHist = new TH1D((this->fName+"_MC").c_str(),
  // (this->fName+this->fPlotTitles).c_str(), this->fNDataPointsX-1,
  // this->fXBins); this->fMCFine = new TH1D((this->fName+"_MC_FINE").c_str(),
  // (this->fName+this->fPlotTitles).c_str(), (this->fNDataPointsX - 1)*10,
  // this->fXBins[0], this->fXBins[this->fNDataPointsX -1]);

  // this->ReadEventFile();

  //  // Different generators require slightly different rescaling factors.
  // if      (this->fEventType == 0) this->fScaleFactor =
  // (GetEventHistogram()->Integral("width")*1E-38/(fNEvents+0.))*14.08/14.0/this->TotalIntegratedFlux();
  // // NEUT else if (this->fEventType == 1) this->fScaleFactor =
  // (GetEventHistogram()->Integral()*1E-38/(fNEvents+0.))*14.08*6.0/14./GetFluxHistogram()->Integral();
  // // NUWRO else if (this->fEventType == 5) this->fScaleFactor =
  // (GetEventHistogram()->Integral()*1E-38/(fNEvents+0.))*14.08*6.0/14./GetFluxHistogram()->Integral();
  // // GENIE
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
  QLOG(SAM, this->fName << "Setting data for " << this->fName);
  QLOG(SAM, this->fName << "From: " << dataFile);
  QLOG(SAM, this->fName << "Reading error from covariance");

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
  QLOG(SAM, this->fName << "===============");
  QLOG(SAM, this->fName << "Reading covariance: " << this->fName);
  QLOG(SAM, this->fName << "From: " << covarFile);
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
