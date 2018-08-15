#include <iomanip>

#include "T2K_SignalDef.h"
#include "T2K_CC1pip_CH_XSec_1Dppi_nu.h"


//********************************************************************
T2K_CC1pip_CH_XSec_1Dppi_nu::T2K_CC1pip_CH_XSec_1Dppi_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC1pip_CH_XSec_1Dppi_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: T2k Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetTitle("T2K_CC1pip_CH_XSec_1Dppi_nu");
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("p_{#pi} (GeV/c)");
  fSettings.SetYTitle("d#sigma/dW_{rec} (cm^{2}/(GeV/c)/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 100.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");
  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor =  (GetEventHistogram()->Integral("width") * 1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  if (fSettings.GetS("type").find("Michel") != std::string::npos) {
    useMichel = true;
    fName += "_Michel";
    SetDataValues(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Ppi.root");
    SetCovarMatrix(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Ppi.root");
  } else {
    useMichel = false;
    //    fName += "_kin";
    SetDataValues(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Ppi_noME.root");
    SetCovarMatrix(GeneralUtils::GetTopLevelDir() + "/data/T2K/CC1pip/CH/Ppi_noME.root");
  }
  
  SetShapeCovar();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dppi_nu::SetDataValues(std::string fileLocation) {
  LOG(DEB) << "Reading: " << this->fName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  // Don't want the last bin of dataCopy
  TH1D *dataCopy = (TH1D*)(dataFile->Get("hResult_sliced_0_1"))->Clone();
  LOG(DEB) << dataCopy->GetNbinsX() << std::endl;

  double *binEdges = new double[dataCopy->GetNbinsX() - 1];
  for (int i = 0; i < dataCopy->GetNbinsX() - 1; i++) {
    binEdges[i] = dataCopy->GetBinLowEdge(i + 1);
  }
  binEdges[dataCopy->GetNbinsX() - 1] = dataCopy->GetBinLowEdge(dataCopy->GetNbinsX());

  fDataHist = new TH1D((fName + "_data").c_str(), (fName + "_data" + fPlotTitles).c_str(), dataCopy->GetNbinsX() - 2, binEdges);

  for (int i = 0; i < fDataHist->GetNbinsX(); i++) {
    fDataHist->SetBinContent(i + 1, dataCopy->GetBinContent(i + 1) * 1E-38);
    fDataHist->SetBinError(i + 1, dataCopy->GetBinError(i + 1) * 1E-38);
    LOG(DEB) << fDataHist->GetBinLowEdge(i + 1) << " " << fDataHist->GetBinContent(i + 1) << std::endl;
  }

  fDataHist->SetDirectory(0); //should disassociate fDataHist with dataFile
  fDataHist->SetNameTitle((fName + "_data").c_str(), (fName + "_MC" + fPlotTitles).c_str());


  dataFile->Close();
};

// Override this for now
// Should really have Measurement1D do this properly though
void T2K_CC1pip_CH_XSec_1Dppi_nu::SetCovarMatrix(std::string fileLocation) {
  LOG(DEB) << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("TMatrixDBase;1"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) ERR(WRN) << "covariance matrix not square!" << std::endl;

  this->fFullCovar = new TMatrixDSym(nBinsX - 3);

  // First two entries are BS
  // Last entry is BS
  for (int i = 2; i < nBinsX-1; i++) {
    for (int j = 2; j < nBinsY-1; j++) {
      LOG(DEB) << "(" << i << ", " << j << ") = " << covarMatrix->GetBinContent(i + 1, j + 1) << std::endl;
      (*this->fFullCovar)(i - 2, j - 2) = covarMatrix->GetBinContent(i, j); //adds syst+stat covariances
    }
  } //should now have set covariance, I hope

  this->fDecomp = StatUtils::GetDecomp(this->fFullCovar);
  this->covar   = StatUtils::GetInvert(this->fFullCovar);
  return;
};


void T2K_CC1pip_CH_XSec_1Dppi_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(13) == 0 ||
      event->NumFSParticle(211) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double ppip = FitUtils::p(Ppip);

  fXVar = ppip;

  return;
};

//********************************************************************
bool T2K_CC1pip_CH_XSec_1Dppi_nu::isSignal(FitEvent *event) {
//********************************************************************
// This distribution uses a somewhat different signal definition so might as well implement it separately here

  // If we use Michel tag sample we don't cut into the pion phase space, only the muon phase space
  // The last bool refers to if we have Michel e or not
  if (useMichel) {
    return SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);
  } else { // Custom signal definition if we aren't using Michel tag; cut on muon and cut only on pion angle

    // does the event pass the muon cut?
    bool muonPass = SignalDef::isCC1pip_T2K_CH(event, EnuMin, EnuMax, true);

    // If the event doesn't pass the muon cut return false
    if (!muonPass) {
      return false;
    }

    // does the event pass the pion angle cut?
    // we already know there's just one muon in the event if it passes muonPass so don't need to make an event loop rejection
    // Need the neutrino four-vector to get the angle between pion and neutrino
    TLorentzVector Pnu = event->PartInfo(0)->fP;
    TLorentzVector Ppip;
    for (unsigned int j = 2; j < event->Npart(); j++) {
      if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fNEUTStatusCode != 0) continue; //move on if NOT ALIVE and NOT NORMAL
      int PID = (event->PartInfo(j))->fPID;
      if (PID == 211) {
        Ppip = event->PartInfo(j)->fP; // Once the pion is found we can break
        break;
      }
    }

    double cos_th_pi = cos(FitUtils::th(Pnu, Ppip));
    // Now check the angle of the pion
    if (cos_th_pi <= 0.2) {
      return false;
    } else {
      return true;
    }
  }

  // Unnecessary default to false
  return false;
}

