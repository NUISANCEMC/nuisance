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

#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"

//********************************************************************
// The constructor
MINERvA_CCNpip_XSec_1DTpi_nu::MINERvA_CCNpip_XSec_1DTpi_nu(
    std::string name, std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile) {
//********************************************************************

  fName = name;
  fPlotTitles =
      "; T_{#pi} (MeV); (1/T#Phi) dN_{#pi}/dT_{#pi} (cm^{2}/MeV/nucleon)";
  fFullPhaseSpace = fName.find("_20deg") == std::string::npos;
  fUpdatedData = fName.find("2015") == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;

  // Reserve length 3 for the number of pions
  TpiVect.reserve(3);

  // Lots of good data from MINERvA, thanks!
  // Full Phase Space
  if (fFullPhaseSpace) {
    // 2016 release
    if (fUpdatedData) {
      this->SetDataValues(
          GeneralUtils::GetTopLevelDir() +
          "/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-pion-kinetic-energy.csv");

      // MINERvA has the error quoted as a percentage of the cross-section
      // Need to make this into an absolute error before we go from correlation
      // matrix -> covariance matrix since it depends on the error in the ith
      // bin
      for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
        fDataHist->SetBinError(i + 1,
                               fDataHist->GetBinContent(i + 1) *
                                   (fDataHist->GetBinError(i + 1) / 100.));
      }

      // This is a correlation matrix, not covariance matrix, so needs to be
      // converted
      this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir() +
                                           "/data/MINERvA/CCNpip/2016/"
                                           "nu-ccNpi+-correlation-pion-kinetic-"
                                           "energy.csv",
                                       fDataHist->GetNbinsX());

      // 2015 release
    } else {
      // If we're doing shape only
      if (fIsShape) {
        fName += "_shape";
        this->SetDataValues(
            GeneralUtils::GetTopLevelDir() +
            "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_shape.txt");
        this->SetCovarMatrixFromCorrText(
            GeneralUtils::GetTopLevelDir() +
                "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_shape_cov.txt",
            fDataHist->GetNbinsX());
        // If we're doing full cross-section
      } else {
        this->SetDataValues(GeneralUtils::GetTopLevelDir() +
                            "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi.txt");
        this->SetCovarMatrixFromCorrText(
            GeneralUtils::GetTopLevelDir() +
                "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_cov.txt",
            fDataHist->GetNbinsX());
      }

      // Adjust MINERvA data to flux correction; roughly a 11% normalisation
      // increase in data
      for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
        fDataHist->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) * 1.11);
      }
    }

    // Restricted Phase Space
  } else {
    // Only 2015 data released restricted muon phase space cross-section
    // unfortunately
    if (fUpdatedData) {
      LOG(SAM) << fName << " has no updated 2016 data for restricted phase "
                           "space! Using 2015 data."
               << std::endl;
      fUpdatedData = false;
    }

    // If we're using the shape only data
    if (fIsShape) {
      this->SetDataValues(
          GeneralUtils::GetTopLevelDir() +
          "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape.txt");
      this->SetCovarMatrixFromCorrText(
          GeneralUtils::GetTopLevelDir() +
              "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape_cov.txt",
          fDataHist->GetNbinsX());
      // Or total cross-section
    } else {
      this->SetDataValues(
          GeneralUtils::GetTopLevelDir() +
          "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg.txt");
      this->SetCovarMatrixFromCorrText(
          GeneralUtils::GetTopLevelDir() +
              "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_cov.txt",
          fDataHist->GetNbinsX());
    }

    // Adjust 2015 MINERvA data to account for flux correction; roughly a 11%
    // normalisation increase in data
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) * 1.11);
    }
  }

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  Measurement1D::SetupDefaultHist();

  onePions = (TH1D *)(fDataHist->Clone());
  twoPions = (TH1D *)(fDataHist->Clone());
  threePions = (TH1D *)(fDataHist->Clone());
  morePions = (TH1D *)(fDataHist->Clone());

  onePions->SetNameTitle((fName + "_1pions").c_str(),
                         (fName + "_1pions" + fPlotTitles).c_str());
  twoPions->SetNameTitle((fName + "_2pions").c_str(),
                         (fName + "_2pions;" + fPlotTitles).c_str());
  threePions->SetNameTitle((fName + "_3pions").c_str(),
                           (fName + "_3pions" + fPlotTitles).c_str());
  morePions->SetNameTitle((fName + "_4pions").c_str(),
                          (fName + "_4pions" + fPlotTitles).c_str());

  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) /
                 double(fNEvents) / TotalIntegratedFlux("width");
};

//********************************************************************
// Here we have to fill for every pion we find in the event
void MINERvA_CCNpip_XSec_1DTpi_nu::FillEventVariables(FitEvent *event) {
  //********************************************************************

  if (event->NumFSParticle(211) == 0 && event->NumFSParticle(-211) == 0) return;
  if (event->NumFSParticle(13) == 0) return;

  // Clear out the vectors
  TpiVect.clear();
  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);

  if (hadMass < 1800) {

    // Loop over the particle stack
    for (unsigned int j = 2; j < event->Npart(); ++j) {
      // Only include alive particles
      if (!(event->PartInfo(j))->fIsAlive &&
          (event->PartInfo(j))->fNEUTStatusCode != 0)
        continue;

      int PID = (event->PartInfo(j))->fPID;
      // Pick up the charged pions in the event
      if (abs(PID) == 211) {
        double ppi = FitUtils::T(event->PartInfo(j)->fP) * 1000.;
        TpiVect.push_back(ppi);
      }
    }
  }

  fXVar = 0;

  return;
};

//********************************************************************
// The last bool refers to if we're using restricted phase space or not
bool MINERvA_CCNpip_XSec_1DTpi_nu::isSignal(FitEvent *event) {
  //********************************************************************
  // Last false refers to that this is NOT the restricted MINERvA phase space,
  // in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, EnuMin, EnuMax, !fFullPhaseSpace);
}

//********************************************************************
// Need to override FillHistograms() here because we fill the histogram N_pion
// times
void MINERvA_CCNpip_XSec_1DTpi_nu::FillHistograms() {
  //********************************************************************

  if (Signal) {
    unsigned int nPions = TpiVect.size();

    // Need to loop over all the pions in the sample
    for (size_t k = 0; k < nPions; ++k) {
      double tpi = TpiVect[k];
      this->fMCHist->Fill(tpi, Weight);
      this->fMCFine->Fill(tpi, Weight);
      this->fMCStat->Fill(tpi, 1.0);

      if (nPions == 1) {
        onePions->Fill(tpi, Weight);
      } else if (nPions == 2) {
        twoPions->Fill(tpi, Weight);
      } else if (nPions == 3) {
        threePions->Fill(tpi, Weight);
      } else if (nPions > 3) {
        morePions->Fill(tpi, Weight);
      }

      PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, tpi, Weight);
    }
  }
}

//********************************************************************
void MINERvA_CCNpip_XSec_1DTpi_nu::ScaleEvents() {
  //********************************************************************
  Measurement1D::ScaleEvents();

  onePions->Scale(this->fScaleFactor, "width");
  twoPions->Scale(this->fScaleFactor, "width");
  threePions->Scale(this->fScaleFactor, "width");
  morePions->Scale(this->fScaleFactor, "width");

  return;
}

//********************************************************************
void MINERvA_CCNpip_XSec_1DTpi_nu::Write(std::string drawOpts) {
  //********************************************************************
  Measurement1D::Write(drawOpts);

  // Draw the npions stack
  onePions->SetTitle("1#pi");
  onePions->SetLineColor(kBlack);
  // onePions->SetFillStyle(0);
  onePions->SetFillColor(onePions->GetLineColor());

  twoPions->SetTitle("2#pi");
  twoPions->SetLineColor(kRed);
  // twoPions->SetFillStyle(0);
  twoPions->SetFillColor(twoPions->GetLineColor());

  threePions->SetTitle("3#pi");
  threePions->SetLineColor(kGreen);
  // threePions->SetFillStyle(0);
  threePions->SetFillColor(threePions->GetLineColor());

  morePions->SetTitle(">3#pi");
  morePions->SetLineColor(kBlue);
  // morePions->SetFillStyle(0);
  morePions->SetFillColor(morePions->GetLineColor());

  THStack pionStack =
    THStack((fName + "_pionStack").c_str(), (fName + "_pionStack").c_str());

  pionStack.Add(onePions);
  pionStack.Add(twoPions);
  pionStack.Add(threePions);
  pionStack.Add(morePions);

  pionStack.Write();

  return;
}
