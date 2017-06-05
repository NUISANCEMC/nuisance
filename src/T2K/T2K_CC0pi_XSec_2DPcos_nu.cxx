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

#include "T2K_SignalDef.h"

#include "T2K_CC0pi_XSec_2DPcos_nu.h"



//********************************************************************
T2K_CC0pi_XSec_2DPcos_nu::T2K_CC0pi_XSec_2DPcos_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pi_XSec_2DPcos_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Medium Energy FHC numu  \n" \
                        "Signal: CC-inclusive with theta < 20deg \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("P_{#mu} (GeV)");
  fSettings.SetYTitle("cos#theta_{#mu}");
  fSettings.SetZTitle("d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)");
  fSettings.SetAllowedTypes("DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV","FIX");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

 if (fName == "T2K_CC0pi_XSec_2DPcos_nu_I") fAnalysis = 1;
  else fAnalysis = 2;


  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pi_XSec_2DPcos_nu");

  fSettings.SetDataInput(  FitPar::GetDataBase() + "/MINERvA/CCEavq3/data_2D.txt" );
  fSettings.SetCovarInput( FitPar::GetDataBase() + "/MINERvA/CCEavq3/covar_2D.txt" );
  fSettings.SetMapInput( FitPar::GetDataBase() + "/MINERvA/CCEavq3/map_2D.txt" );
  fSettings.DefineAllowedSpecies("numu");

  forwardgoing = (fSettings.GetS("type").find("REST") != std::string::npos);

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  // Plot Setup -------------------------------------------------------
  Double_t binx[7] = {0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8};
  Double_t biny[17] = {0.0, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.20, 0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.80};
  CreateDataHistogram(7, binx, 17, biny);

  SetDataValuesFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-42);

  SetMapValuesFromText( fSettings.GetMapInput() );

  SetCholDecompFromTextFile( fSettings.GetCovarInput() );
  ScaleCovar(1E-16);

  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};


bool T2K_CC0pi_XSec_2DPcos_nu::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, forwardgoing);
};

void T2K_CC0pi_XSec_2DPcos_nu::FillEventVariables(FitEvent* event){

  if (event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu = event->GetNeutrinoIn()->fP;
  TLorentzVector Pmu = event->GetHMFSParticle(13)->fP;

  double pmu = Pmu.Vect().Mag()/1000.;
  double CosThetaMu = cos(Pnu.Vect().Angle(Pmu.Vect()));

  fXVar = pmu;
  fYVar = CosThetaMu;

  return;
};

// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi_XSec_2DPcos_nu::ConvertEventRates(){

  // Do standard conversion.
  Measurement2D::ConvertEventRates();

  if (fAnalysis == 1){

    // Following code handles weird ND280 Binning
    int nbins = this->fMCHist->GetNbinsX() + 1;
    double total = 0.0;

    // Y = 1
    total = 0.0;
    for (int i = 3; i < nbins; i++){

      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(1);
      total += this->fMCHist->GetBinContent(i, 1) * width;
      this->fMCHist->SetBinContent(i,1,0);
    }
    this->fMCHist->SetBinContent(3, 1, total / (1.0 * 29.6));

    // Y = 2
    total = 0.0;
    for (int i = 5; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(2);
      total += this->fMCHist->GetBinContent(i, 2)* width;
      this->fMCHist->SetBinContent(i,2,0);
    }
    this->fMCHist->SetBinContent(5, 2, total / (0.6 *29.4));

    // Y = 3
    total = 0.0;
    for (int i = 7; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(3);
      total += this->fMCHist->GetBinContent(i, 3)* width;
      this->fMCHist->SetBinContent(i, 3,0);
    }
    this->fMCHist->SetBinContent(7, 3, total/ (0.1 * 29.2));

    // Y = 4
    total = 0.0;
    for (int i = 7; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(4);
      total += this->fMCHist->GetBinContent(i, 4)* width;
      this->fMCHist->SetBinContent(i, 4,0);
    }
    this->fMCHist->SetBinContent(7, 4, total / (0.1 * 29.2));

    // Y = 5
    total = 0.0;
    for (int i = 8; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(5);
      total += this->fMCHist->GetBinContent(i, 5)* width;
      this->fMCHist->SetBinContent(i,5,0);
    }
    this->fMCHist->SetBinContent(8, 5, total / (0.05 * 29.0));

    // Y = 6
    total = 0.0;
    for (int i = 9; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(6);
      total += this->fMCHist->GetBinContent(i, 6)* width;
      this->fMCHist->SetBinContent(i, 6,0);
    }
    this->fMCHist->SetBinContent(9, 6, total / (0.05 * 28.5));

    // Y = 7
    total = 0.0;
    for (int i = 8; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(7);
      total += this->fMCHist->GetBinContent(i, 7)* width;
      this->fMCHist->SetBinContent(i, 7,0);
    }
    this->fMCHist->SetBinContent(8, 7, total/ (0.04 * 28.0));

    // Y = 8
    total = 0.0;
    for (int i = 11; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(8);
      total += this->fMCHist->GetBinContent(i, 8)* width;
      this->fMCHist->SetBinContent(i, 8,0);
    }
    this->fMCHist->SetBinContent(11, 8, total / (0.4 * 27.0));

    // Y = 9
    total = 0.0;
    for (int i = 9; i < nbins; i++){
      double width = this->fMCHist->GetXaxis()->GetBinWidth(i) * this->fMCHist->GetYaxis()->GetBinWidth(9);
      total += this->fMCHist->GetBinContent(i, 9)* width;
      this->fMCHist->SetBinContent(i,9,0);
    }
    this->fMCHist->SetBinContent(9, 9, total / (0.02 * 25.0));
  }

  return;
}


void T2K_CC0pi_XSec_2DPcos_nu::SetHistograms(){

  fIsSystCov = fSettings.GetS("type").find("SYSTCOV") != std::string::npos;
  fIsStatCov = fSettings.GetS("type").find("STATCOV") != std::string::npos;
  fIsNormCov = fSettings.GetS("type").find("NORMCOV") != std::string::npos;
  fNDataPointsX = 12;
  fNDataPointsY = 10;

  // Open file
  std::string infile = FitPar::GetDataBase()+"/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root";
  TFile* rootfile = new TFile(infile.c_str(), "READ");
  TH2D* tempcov;

  // ANALYSIS 2
  if (fAnalysis == 2){

    // Get Data
    fDataHist = (TH2D*) rootfile->Get("analysis2_data");
    fDataHist->SetDirectory(0);
    fDataHist->SetNameTitle((fName + "_data").c_str(),
			    (fName + "_data" + fPlotTitles).c_str());

    // Get Map
    fMapHist = (TH2I*) rootfile->Get("analysis2_map");
    fMapHist->SetDirectory(0);
    fMapHist->SetNameTitle((fName + "_map").c_str(),
			    (fName + "_map" + fPlotTitles).c_str());

    // Get Syst/Stat Covar
    TH2D* tempsyst = (TH2D*) rootfile->Get("analysis2_systcov");
    TH2D* tempstat = (TH2D*) rootfile->Get("analysis2_statcov");
    TH2D* tempnorm = (TH2D*) rootfile->Get("analysis2_normcov");

    // Create covar [Default is both]
    tempcov = (TH2D*) tempsyst->Clone();
    tempcov->Reset();

    if (fIsSystCov) tempcov->Add(tempsyst);
    if (fIsStatCov) tempcov->Add(tempstat);
    if (fIsNormCov) tempcov->Add(tempnorm);

    if (!fIsSystCov && !fIsStatCov && !fIsNormCov){
      tempcov->Add(tempsyst);
      tempcov->Add(tempstat);
      tempcov->Add(tempnorm);
    }

    // SARAS ANALYSIS
  } else if (fAnalysis == 1){

    //TODO (P.Stowell) Add a TH2Poly Measurement class
    ERR(FTL) << "T2K CC0Pi Analysis 1 is not yet available due to its awkward binning!" << std::endl;
    ERR(FTL) << "If you want to use it, add a TH2Poly Class!" << std::endl;
    throw;

  }


  // Setup Covar
  int nbins = tempcov->GetNbinsX();
  fFullCovar = new TMatrixDSym(nbins);

  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){

      (*fFullCovar)(i,j) = tempcov->GetBinContent(i+1,j+1);

    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(covar);

  // Set Data Errors
  StatUtils::SetDataErrorFromCov(fDataHist, fFullCovar, fMapHist, 1E-38);

  // Remove root file
  rootfile->Close();
  return;
};
