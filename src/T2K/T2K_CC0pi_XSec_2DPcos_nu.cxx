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
  fSettings.DefineAllowedSpecies("numu");

  forwardgoing = (fSettings.GetS("type").find("REST") != std::string::npos);

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 / (TotalIntegratedFlux()));

  // Setup Histograms
  SetHistograms();

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

void T2K_CC0pi_XSec_2DPcos_nu::SetHistograms(){

  fIsSystCov = fSettings.GetS("type").find("SYSTCOV") != std::string::npos;
  fIsStatCov = fSettings.GetS("type").find("STATCOV") != std::string::npos;
  fIsNormCov = fSettings.GetS("type").find("NORMCOV") != std::string::npos;
  fNDataPointsX = 12;
  fNDataPointsY = 10;

  // Open file
  std::string infile = FitPar::GetDataBase()+"/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root";
  TFile* rootfile = new TFile(infile.c_str(), "READ");
  TH2D* tempcov = NULL;

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
  }


  if (!tempcov){
    ERR(FTL) << "TEMPCOV NOT SET" << std::endl;
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
