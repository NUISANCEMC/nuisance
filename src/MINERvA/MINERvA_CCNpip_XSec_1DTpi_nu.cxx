#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DTpi_nu::MINERvA_CCNpip_XSec_1DTpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CCNpip_XSec_1DTpi_nu";
  plotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  allowed_types += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  TpiVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    measurementName += "_2016";
    isNew = true;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = dataHist->GetBinWidth(1);
    for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
      double binNWidth = dataHist->GetBinWidth(i+1);
      // First scale to 1E-40
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1E-40);
      // Then set the error as absolute error, not the percent error (which the datafile is listed with)
      dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*dataHist->GetBinError(i+1)/100.);
      // Scale the bin content
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      dataHist->SetBinError(i+1, dataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }

    // This is a correlation matrix
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi_corr.txt", dataHist->GetNbinsX());

  } else {
    isNew = false;

    if (isShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape_cov.txt", dataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_cov.txt", dataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    for (int i = 0; i < dataHist->GetNbinsX() + 1; i++) {
      dataHist->SetBinContent(i+1, dataHist->GetBinContent(i+1)*1.11);
    }
  }

  this->SetupDefaultHist();

  // Make some auxillary helper plots
  hnPions = new TH1I((measurementName+"_Npions").c_str(), (measurementName+"_Npions; Number of pions; Counts").c_str(), 11, -1, 10);
  onePions  = (TH1D*)(dataHist->Clone());
  twoPions  = (TH1D*)(dataHist->Clone());
  threePions = (TH1D*)(dataHist->Clone());
  morePions = (TH1D*)(dataHist->Clone());

  onePions->SetNameTitle((measurementName+"_1pions").c_str(), (measurementName+"_1pions"+plotTitles).c_str());
  twoPions->SetNameTitle((measurementName+"_2pions").c_str(), (measurementName+"_2pions;"+plotTitles).c_str());
  threePions->SetNameTitle((measurementName+"_3pions").c_str(), (measurementName+"_3pions"+plotTitles).c_str());
  morePions->SetNameTitle((measurementName+"_4pions").c_str(), (measurementName+"_4pions"+plotTitles).c_str());

  scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)/TotalIntegratedFlux("width");
};

void MINERvA_CCNpip_XSec_1DTpi_nu::FillEventVariables(FitEvent *event) {

  piIndex.clear();
  TpiVect.clear();

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {

    // Only include alive particles
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;

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

  this->X_VAR = Tpi;

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
      this->mcHist->Fill(tpi, Weight);
      this->mcFine->Fill(tpi, Weight);
      this->mcStat->Fill(tpi, 1.0);

      if (nPions == 1) {
        onePions->Fill(tpi, Weight);
      } else if (nPions == 2) {
        twoPions->Fill(tpi, Weight);
      } else if (nPions == 3) { 
        threePions->Fill(tpi, Weight);
      } else if (nPions > 3) {
        morePions->Fill(tpi, Weight);
      }

      PlotUtils::FillNeutModeArray(mcHist_PDG, Mode, TpiVect[k], Weight);
    }
    hnPions->Fill(nPions);
  }

}

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1DTpi_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, false);
}

//******************************************************************** 
void MINERvA_CCNpip_XSec_1DTpi_nu::ScaleEvents() {
//******************************************************************** 
  Measurement1D::ScaleEvents();

  onePions->Scale(this->scaleFactor, "width");
  twoPions->Scale(this->scaleFactor, "width");
  threePions->Scale(this->scaleFactor, "width");
  morePions->Scale(this->scaleFactor, "width");
  hnPions->Scale(this->scaleFactor, "width");

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

  THStack pionStack = THStack((measurementName+"_pionStack").c_str(), (measurementName+"_pionStack").c_str());

  pionStack.Add(onePions);
  pionStack.Add(twoPions);
  pionStack.Add(threePions);
  pionStack.Add(morePions);

  pionStack.Write();

  return;
}
