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

#include "MCStudy_CCQEHistograms.h"

#include "T2K_SignalDef.h"
#include "MINERvA_SignalDef.h"

//********************************************************************
/// @brief Class to perform MC Studies on a custom measurement
MCStudy_CCQEHistograms::MCStudy_CCQEHistograms(std::string name, std::string inputfile,
                                       FitWeight *rw, std::string type,
                                       std::string fakeDataFile) {
  //********************************************************************

  // Measurement Details
  fName = name;
  fEventTree = NULL;

  // Define our energy range for flux calcs
  EnuMin = 0.;
  EnuMax = 100.;  // Arbritrarily high energy limit

  // Set default fitter flags
  fIsDiag = true;
  fIsShape = false;
  fIsRawEvents = false;

  // This function will sort out the input files automatically and parse all the
  // inputs,flags,etc.
  // There may be complex cases where you have to do this by hand, but usually
  // this will do.
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  fEventTree = NULL;

  // Setup fDataHist as a placeholder
  this->fDataHist = new TH1D(("approximate_data"), ("kaon_data"), 5, 1.0, 6.0);

  this->SetupDefaultHist();
  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar = StatUtils::GetInvert(fFullCovar);

  // 1. The generator is organised in SetupMeasurement so it gives the
  // cross-section in "per nucleon" units.
  //    So some extra scaling for a specific measurement may be required. For
  //    Example to get a "per neutron" measurement on carbon
  //    which we do here, we have to multiple by the number of nucleons 12 and
  //    divide by the number of neutrons 6.
  this->fScaleFactor = (this->fEventHist->Integral("width") * 1E-38 / (fNEvents + 0.)) /
    this->TotalIntegratedFlux();


  hist_Enu = new TH1D("MCStudy_CCQE_Enu","MCStudy_CCQE_Enu",30,0.0,2.0);
  hist_TLep = new TH1D("MCStudy_CCQE_TLep","MCStudy_CCQE_TLep",30,0.0,4.0);
  hist_CosLep = new TH1D("MCStudy_CCQE_CosLep","MCStudy_CCQE_CosLep",30,-1.0,1.0);
  hist_Q2 = new TH1D("MCStudy_CCQE_Q2;Q^{2} (GeV^{2});d#sigma/dQ^{2} (cm^{2}/nucleon/GeV^{2})","MCStudy_CCQE_Q2",30,0.0,3.0);
  hist_Q2QE = new TH1D("MCStudy_CCQE_Q2QE","MCStudy_CCQE_Q2QE",30,0.0,3.0);
  hist_EQE = new TH1D("MCStudy_CCQE_EQE","MCStudy_CCQE_EQE",30,0.0,5.0);
  hist_q0 = new TH1D("MCStudy_CCQE_q0","MCStudy_CCQE_q0",30,0.0,2.0);
  hist_q3 = new TH1D("MCStudy_CCQE_q3","MCStudy_CCQE_q3",30,0.0,2.0);
  hist_TLepCosLep = new TH2D("MCStudy_CCQE_TLepCosLep","MCStudy_CCQE_TLepCosLep",15,0.0,5.0,15,-1.0,1.0);
  hist_Total = new TH1D("MCStudy_CCQE_TotalXSec","MXStudy_CCQE_TotalXSec",1,0.0,1.0);

  hist_q0q3 = new TH2D("MCStudy_CCQE_q0q3","MCStudy_CCQE_q0q3;q_{3} (GeV); q_{0} (GeV); d#sigma/dq_{0}dq_{3} (cm^{2}/nucleon/GeV^{2})",40,0.0,2.0,40,0.0,2.0);

  return;
}

//********************************************************************
void MCStudy_CCQEHistograms::FillEventVariables(FitEvent *event) {
//********************************************************************

//  std::cout << "Event fBound = " << event->fBound << " " << event->Mode << std::endl;
//  if (event->fBound > 0) return;
  if (abs(event->Mode) != 1) return;
  //  std::cout << "Event fBound = " << event->fBound << " " << event->Mode << "-> Signal " << std::endl;


  FitParticle* muon = NULL;
  FitParticle* nu   = event->GetNeutrinoIn();
  bool IsNuMu = event->PDGnu() > 0;

  if (IsNuMu)  muon = event->GetHMFSParticle(13);
  else muon = event->GetHMFSParticle(-13);

  // Reset Variables
  Enu    = -999.9;
  TLep   = -999.9;
  CosLep = -999.9;
  Q2     = -999.9;
  Q2QE   = -999.9;
  EQE    = -999.9;
  q0     = -999.9;
  q3     = -999.9;

  // Fill Variables
  if (muon){

    Enu = event->Enu() / 1.E3;

    TLep   = (muon->fP.E() - muon->fP.Mag()) / 1.E3;
    CosLep = cos(muon->fP.Vect().Angle( nu->fP.Vect() ));

    Q2   = fabs((muon->fP - nu->fP).Mag2() / 1.E6);

    Q2QE = FitUtils::Q2QErec(muon->fP, CosLep, 34., IsNuMu);
    EQE  = FitUtils::EnuQErec(muon->fP, CosLep, 34., IsNuMu);

    q0 = fabs((muon->fP - nu->fP).E()) / 1.E3;
    q3 = fabs((muon->fP - nu->fP).Vect().Mag()) / 1.E3;

    LocalRWWeight = event->RWWeight;
    LocalInputWeight = event->InputWeight;

  }

  // Fill Tree
  if (abs(Mode) == 1 and Signal){
    hist_Enu->Fill(Enu,event->Weight);
    hist_TLep->Fill(TLep,event->Weight);
    hist_CosLep->Fill(CosLep,event->Weight);
    hist_Q2->Fill(Q2,event->Weight);
    hist_Q2QE->Fill(Q2QE,event->Weight);
    hist_EQE->Fill(EQE,event->Weight);
    hist_q0->Fill(q0,event->Weight);
    hist_q3->Fill(q3,event->Weight);
    hist_TLepCosLep->Fill(TLep,CosLep,event->Weight);
    hist_q0q3->Fill(q3,q0,event->Weight);
    hist_Total->Fill(0.5,event->Weight);

    fXVar = Q2;
  }

  return;
};

//********************************************************************
void MCStudy_CCQEHistograms::Write(std::string drawOpt) {
//********************************************************************
//  Measurement1D::Write(drawOpt);

  LOG(FIT) << "Writing MCStudy_CCQEHistograms " << std::endl;
  //  Config::Get().out->cd();
  hist_Enu->Write();
  hist_TLep->Write();
  hist_CosLep->Write();
  hist_Q2->Write();
  hist_Q2QE->Write();
  hist_EQE->Write();
  hist_q0->Write();
  hist_q3->Write();
  hist_TLepCosLep->Write();
  hist_q0q3->Write();
  hist_Total->Write();

  return;
}

//********************************************************************
void MCStudy_CCQEHistograms::ResetAll(){
  //********************************************************************
  hist_Enu->Reset();
  hist_TLep->Reset();
  hist_CosLep->Reset();
  hist_Q2->Reset();
  hist_Q2QE->Reset();
  hist_EQE->Reset();
  hist_q0->Reset();
  hist_q3->Reset();
  hist_q0q3->Reset();
  hist_TLepCosLep->Reset();
  hist_Total->Reset();

  return;
}


//********************************************************************
void MCStudy_CCQEHistograms::ScaleEvents(){
//********************************************************************

  hist_Enu->Scale(fScaleFactor,"width");
  hist_TLep->Scale(fScaleFactor,"width");
  hist_CosLep->Scale(fScaleFactor,"width");
  hist_Q2->Scale(fScaleFactor,"width");
  hist_Q2QE->Scale(fScaleFactor,"width");
  hist_EQE->Scale(fScaleFactor,"width");
  hist_q0->Scale(fScaleFactor,"width");
  hist_q3->Scale(fScaleFactor,"width");
  hist_q0q3->Scale(fScaleFactor,"width");
  hist_TLepCosLep->Scale(fScaleFactor,"width");
  hist_Total->Scale(fScaleFactor,"width");

  return;
}



//********************************************************************
/// Select only events with final state Muons
bool MCStudy_CCQEHistograms::isSignal(FitEvent *event) {
//********************************************************************


  if (abs(event->Mode) != 1) return false;
  //if (event->fBound > 0) return false;
  //  if (!event->HasFSMuon()) return false;

  // Do we want any other signal?
  return true;
};



