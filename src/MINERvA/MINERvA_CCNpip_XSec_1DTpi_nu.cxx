#include "MINERvA_CCNpip_XSec_1DTpi_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1DTpi_nu::MINERvA_CCNpip_XSec_1DTpi_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  fName = "MINERvA_CCNpip_XSec_1DTpi_nu";
  fPlotTitles = "; T_{#pi} (MeV); d#sigma/dT_{#pi} (cm^{2}/MeV/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  fIsDiag = false;
  fAllowedTypes += "NEW";
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  piIndex.reserve(3);
  TpiVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    fName += "_2016";
    isNew = true;

    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi.txt");

    // MINERvA mucked up the scaling in the data-release where everything was bin-width normalised to the first bin, not the nth bin
    double binOneWidth = fDataHist->GetBinWidth(1);
    for (int i = 0; i < fDataHist->GetNbinsX()+1; i++) {
      double binNWidth = fDataHist->GetBinWidth(i+1);
      // First scale to 1E-40
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*1E-40);
      // Then set the error as absolute error, not the percent error (which the datafile is listed with)
      fDataHist->SetBinError(i+1, fDataHist->GetBinContent(i+1)*fDataHist->GetBinError(i+1)/100.);
      // Scale the bin content
      fDataHist->SetBinContent(i+1, fDataHist->GetBinContent(i+1)*binOneWidth/binNWidth);
      fDataHist->SetBinError(i+1, fDataHist->GetBinError(i+1)*binOneWidth/binNWidth);
    }

    // This is a correlation matrix
    this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_tpi_corr.txt", fDataHist->GetNbinsX());

  } else {
    isNew = false;

    if (fIsShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_shape_cov.txt", fDataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi.txt");
      this->SetCovarMatrixFromText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/MINERvA_CCNpi_Tpi_cov.txt", fDataHist->GetNbinsX());
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
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, false);
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
