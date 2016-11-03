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

// The constructor
MINERvA_CCNpip_XSec_1DTpi_nu::MINERvA_CCNpip_XSec_1DTpi_nu(std::string name, std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = name;
  fPlotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  fFullPhaseSpace = fName.find("_20deg") == std::string::npos;
  fUpdatedData = fName.find("2015") == std::string::npos;
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  TpiVect.reserve(3);

  // Full Phase Space
  if (fFullPhaseSpace){
    if (fUpdatedData){

      //this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi.txt");
      this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-pion-kinetic-energy.csv");

      // MINERvA has the error quoted as a percentage of the cross-section
      // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
      for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
        fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*(fDataHist->GetBinError(i+1)/100.));
      }

      //this->SetCovarMatrixFromText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi_corr.txt", fDataHist->GetNbinsX());
      // This is a correlation matrix
      this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-pion-kinetic-energy.csv", fDataHist->GetNbinsX());

    } else {
      isNew = false;

      if (fIsShape) {
        this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_shape.txt");
        this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_shape_cov.txt", fDataHist->GetNbinsX());
      } else {
        this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi.txt");
        this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_cov.txt", fDataHist->GetNbinsX());
      }

      // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
      for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
        fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
      }
    }

  // Restricted Phase Space
  } else {

    if (fUpdatedData){
      LOG(SAM) << fName << " has no updated 2016 data for restricted phase space! Using 2015 data." << std::endl; 
      fUpdatedData = false;
    } 

    if (fIsShape) {
      this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape.txt");
      this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_shape_cov.txt", fDataHist->GetNbinsX());
    } else {
      this->SetDataValues(GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg.txt");
      this->SetCovarMatrixFromCorrText(GeneralUtils::GetTopLevelDir()+
        "/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_Tpi_20deg_cov.txt", fDataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1.11);
    }    

  }

  SetupDefaultHist();

  // Make some auxillary helper plots
  hnPions = new TH1I((fName+"_Npions").c_str(), (fName+"_Npions; Number of pions; Counts").c_str(), 11, -1, 10);
  onePions  = (TH1D*)(fDataHist->Clone());
  twoPions  = (TH1D*)(fDataHist->Clone());
  threePions = (TH1D*)(fDataHist->Clone());
  morePions = (TH1D*)(fDataHist->Clone());

  onePions->SetNameTitle((fName+"_1pions").c_str(), (fName+"_1pions"+fPlotTitles).c_str());
  twoPions->SetNameTitle((fName+"_2pions").c_str(), (fName+"_2pions;"+fPlotTitles).c_str());
  threePions->SetNameTitle((fName+"_3pions").c_str(), (fName+"_3pions"+fPlotTitles).c_str());
  morePions->SetNameTitle((fName+"_4pions").c_str(), (fName+"_4pions"+fPlotTitles).c_str());

  fScaleFactor = fEventHist->Integral("width")*double(1E-38)/double(fNEvents)/TotalIntegratedFlux("width");

};

void MINERvA_CCNpip_XSec_1DTpi_nu::FillEventVariables(FitEvent *event) {

  piIndex.clear();
  TpiVect.clear();

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {

    // Only include alive particles
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fNEUTStatusCode != 0) continue;

    int PID = (event->PartInfo(j))->fPID;
    // Pick up the charged pions
    if (abs(PID) == 211) {
      piIndex.push_back(j);
    // Find muon
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;
    }
  }

  // Hadronic mass cut in true
  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double Tpi = -999;

  // If hadronic mass passes signal, loop over the pions
  if (hadMass > 100 && hadMass < 1800 && piIndex.size() > 0) {

    // Loop over surviving pions and pick up their kinetic energy
    for (size_t k = 0; k < piIndex.size(); ++k) {
      TLorentzVector Ppip = event->PartInfo(piIndex[k])->fP;
      Tpi = FitUtils::T(Ppip)*1000.;
      TpiVect.push_back(Tpi);
    }

  } else {
    Tpi = -999;
  }

  fXVar = Tpi;

  return;
};

//********************************************************************
// Need to override FillHistograms() here because we fill the histogram N_pion times
void MINERvA_CCNpip_XSec_1DTpi_nu::FillHistograms() {
//********************************************************************

  if (Signal){

    // Need to loop over all the pions in the sample
    for (int k = 0; k < nPions; ++k) {
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

      PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, TpiVect[k], Weight);
    }
    hnPions->Fill(nPions);
  }

}

//********************************************************************
bool MINERvA_CCNpip_XSec_1DTpi_nu::isSignal(FitEvent *event) {
//********************************************************************
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, !fFullPhaseSpace);
}

//********************************************************************
void MINERvA_CCNpip_XSec_1DTpi_nu::ScaleEvents() {
//********************************************************************
  Measurement1D::ScaleEvents();

  onePions->Scale(this->fScaleFactor, "width");
  twoPions->Scale(this->fScaleFactor, "width");
  threePions->Scale(this->fScaleFactor, "width");
  morePions->Scale(this->fScaleFactor, "width");
  hnPions->Scale(this->fScaleFactor, "width");

  return;
}

//********************************************************************
void MINERvA_CCNpip_XSec_1DTpi_nu::Write(std::string drawOpts) {
//********************************************************************
  Measurement1D::Write(drawOpts);

  hnPions->Write();

  // Draw the npions stack
  onePions->SetTitle("1#pi");
  onePions->SetLineColor(kBlack);
  //onePions->SetFillStyle(0);
  onePions->SetFillColor(onePions->GetLineColor());

  twoPions->SetTitle("2#pi");
  twoPions->SetLineColor(kRed);
  //twoPions->SetFillStyle(0);
  twoPions->SetFillColor(twoPions->GetLineColor());

  threePions->SetTitle("3#pi");
  threePions->SetLineColor(kGreen);
  //threePions->SetFillStyle(0);
  threePions->SetFillColor(threePions->GetLineColor());

  morePions->SetTitle(">3#pi");
  morePions->SetLineColor(kBlue);
  //morePions->SetFillStyle(0);
  morePions->SetFillColor(morePions->GetLineColor());

  THStack pionStack = THStack((fName+"_pionStack").c_str(), (fName+"_pionStack").c_str());

  pionStack.Add(onePions);
  pionStack.Add(twoPions);
  pionStack.Add(threePions);
  pionStack.Add(morePions);

  pionStack.Write();

  return;
}
