#include "MINERvA_CCNpip_XSec_1Dth_nu.h"

// The constructor
MINERvA_CCNpip_XSec_1Dth_nu::MINERvA_CCNpip_XSec_1Dth_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "MINERvA_CCNpip_XSec_1Dth_nu";
  plotTitles = "; #theta_{#pi} (degrees); d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)";
  EnuMin = 1.5;
  EnuMax = 10;
  isDiag = false;
  allowed_types += "NEW";

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Reserve length 3 for the number of pions
  // We fill once per pion found in the event, so can fill multiple times for one event
  piIndex.reserve(3);
  thVect.reserve(3);

  if (type.find("NEW") != std::string::npos) {
    measurementName += "_2016";
    isNew = true;

    //this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016_upd/ccnpip_thpi.txt");
    this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-xsec-pion-angle.csv");

    // MINERvA has the error quoted as a percentage of the cross-section
    // Need to make this into an absolute error before we go from correlation matrix -> covariance matrix since it depends on the error in the ith bin
    for (int i = 0; i < dataHist->GetNbinsX()+1; i++) {
      dataHist->SetBinError(i+1, dataHist->GetBinContent(i+1)*(dataHist->GetBinError(i+1)/100.));
    }

    // This is a correlation matrix! but it's all fixed in SetCovarMatrixFromText
    this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2016/nu-ccNpi+-correlation-pion-angle.csv", dataHist->GetNbinsX());

  } else {
    isNew = false;

    if (isShape) {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_th_shape.txt");
      this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_th_shape_cov.txt", dataHist->GetNbinsX());
    } else {
      this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_th.txt");
      this->SetCovarMatrixFromCorrText(std::string(std::getenv("EXT_FIT"))+"/data/MINERvA/CCNpip/2015/MINERvA_CCNpi_th_cov.txt", dataHist->GetNbinsX());
    }

    // Adjust MINERvA data to flux correction; roughly a 11% normalisation increase in data
    // Please change when MINERvA releases new data!
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

void MINERvA_CCNpip_XSec_1Dth_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pmu;
  TLorentzVector Ppip;

  piIndex.clear();
  thVect.clear();

  // Loop over the particle stack
  for (unsigned int j = 2; j < event->Npart(); ++j) {

    // Only include alive particles
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;

    int PID = (event->PartInfo(j))->fPID;
    // Select highest momentum (energy) charged pion
    if (abs(PID) == 211) {
      piIndex.push_back(j);
    // Find muon
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double th;

  // If hadronic mass passes signal, loop over the pions
  if (hadMass > 100 && hadMass < 1800  && piIndex.size() > 0) {

    // Loop over surviving pions and pick up their kinetic energy
    for (unsigned int k = 0; k < piIndex.size(); ++k) {
      Ppip = (event->PartInfo(piIndex[k]))->fP;
      th = (180./M_PI)*FitUtils::th(Pnu, Ppip);
      thVect.push_back(th);
    }

  } else {
    th = -999;
  }

  this->X_VAR = th;

  return;
};

//******************************************************************** 
bool MINERvA_CCNpip_XSec_1Dth_nu::isSignal(FitEvent *event) {
//******************************************************************** 
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCCNpip_MINERvA(event, nPions, EnuMin, EnuMax, false);
}

//******************************************************************** 
void MINERvA_CCNpip_XSec_1Dth_nu::ScaleEvents() {
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
// Need to override FillHistograms() here because we fill the histogram N_pion times
void MINERvA_CCNpip_XSec_1Dth_nu::FillHistograms() {
//******************************************************************** 

  if (Signal){

    // Need to loop over all the pions in the sample
    for (size_t k = 0; k < thVect.size(); ++k) {
      double th = thVect[k];
      this->mcHist->Fill(th, Weight);
      this->mcFine->Fill(th, Weight);
      this->mcStat->Fill(th, 1.0);

      if (nPions == 1) {
        onePions->Fill(th, Weight);
      } else if (nPions == 2) {
        twoPions->Fill(th, Weight);
      } else if (nPions == 3) { 
        threePions->Fill(th, Weight);
      } else if (nPions > 3) {
        morePions->Fill(th, Weight);
      }

      PlotUtils::FillNeutModeArray(mcHist_PDG, Mode, th, Weight);
    }
    hnPions->Fill(nPions);
  }

  return;
}


//******************************************************************** 
void MINERvA_CCNpip_XSec_1Dth_nu::Write(std::string drawOpts) {
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
