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

#include "T2K_CC0pi_XSec_2DPcos_nu_nonuniform.h"



//********************************************************************
T2K_CC0pi_XSec_2DPcos_nu_nonuniform::T2K_CC0pi_XSec_2DPcos_nu_nonuniform(std::string name,
									  std::string inputfile,
									  FitWeight *rw,
									  std::string type){
//********************************************************************

  fName = name;
  fAnalysis = 2;

  forwardgoing = (type.find("REST") != std::string::npos);
  EnuMin = 0;
  EnuMax = 10.0;
  fBeamDistance = 0.280;
  fDefaultTypes = "FIX";
  fAllowedTypes = "DIAG,FULL/FREE,SHAPE,FIX/SYSTCOV/STATCOV";
  //  fNDataPointsX = 12;
  //  fNDataPointsY = 10;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);
  //  fIsSystCov = type.find("SYSTCOV") != std::string::npos;
  //  fIsStatCov = type.find("STATCOV") != std::string::npos;
  //  fIsNormCov = type.find("NORMCOV") != std::string::npos;

  fPlotTitles = "; P_{#mu} (GeV); cos#theta_{#mu}; d^{2}#sigma/dP_{#mu}dcos#theta_{#mu} (cm^{2}/GeV)";
  SetHistograms();
  SetupDefaultHist();

  // Diagonal covar setup
  if (!fIsShape) fAddNormPen = true;
  fNormError = 0.089; // Set from covar mat instead...

  // Get Scaling
  fScaleFactor = ((GetEventHistogram()->Integral("width")/(fNEvents+0.)) * 1E-38 /
		  (TotalIntegratedFlux()));

};

bool T2K_CC0pi_XSec_2DPcos_nu_nonuniform::isSignal(FitEvent *event){
  return SignalDef::isT2K_CC0pi(event, EnuMin, EnuMax, forwardgoing);
};

void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::FillEventVariables(FitEvent* event){

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

void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::FillHistograms(){

  //  Measurement1D::FillHistograms();
  if (Signal){
    FillMCSlice( fXVar, fYVar, Weight );
  }

}


// Modification is needed after the full reconfigure to move bins around
// Otherwise this would need to be replaced by a TH2Poly which is too awkward.
void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::ConvertEventRates(){

  // Do standard conversion.
  Measurement1D::ConvertEventRates();

  // First scale MC slices also by their width in Y
  const double vals[] = {1.00, 0.60, 0.10, 0.10, 0.05, 0.05, 0.04, 0.04, 0.02};
  for (int i = 0; i < 9; i++){
    std::cout << "Scaling width i " << i << " : " << vals[i] << std::endl;
    fMCHist_Slices[i]->Scale(fScaleFactor / vals[i], "width");
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_SlicesPDG[i], fScaleFactor / vals[i], "width");
  }

  // Now Convert into 1D list
  fMCHist->Reset();
  int bincount = 0;
  for (int i = 0; i < 9; i++){
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fMCHist->SetBinContent(bincount+1, fMCHist_Slices[i]->GetBinContent(j+1));
      bincount++;
    }
  }

  return;
}

void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::FillMCSlice(double x, double y, double w){

  int index = -1;
  if (y >= -1.0 and y < 0.0) index = 0;
  else if (y >= 0.0 and y < 0.6) index = 1;
  else if (y >= 0.6 and y < 0.7) index = 2;
  else if (y >= 0.7 and y < 0.8) index = 3;
  else if (y >= 0.8 and y < 0.85) index = 4;
  else if (y >= 0.85 and y < 0.90) index = 5;
  else if (y >= 0.90 and y < 0.94) index = 6;
  else if (y >= 0.94 and y < 0.98) index = 7;
  else if (y >= 0.98 and y <= 1.00) index = 8;

  if (index != -1){
    fMCHist_Slices[index]->Fill(x,w);
    PlotUtils::FillNeutModeArray((TH1D**)fMCHist_SlicesPDG[index], (int)Mode, fXVar, Weight);
  }

}


void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::Write(std::string drawOpt){

  Measurement1D::Write(drawOpt);
  for (int i = 0; i < 9; i++){
    fDataHist_Slices[i]->Write();
    fMCHist_Slices[i]->Write();

    for (int j = 0; j < 61; j++){
      if (!fMCHist_SlicesPDG[i][j] ) continue;
      TH1D* hist =  (TH1D*)fMCHist_SlicesPDG[i][j];
      double xlow  = hist->GetXaxis()->GetBinLowEdge(1);
      double xhigh = hist->GetXaxis()->GetBinLowEdge( hist->GetNbinsX()+1 );
      hist->GetXaxis()->SetRangeUser( xlow, xhigh );
    }

    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack( (std::string(fMCHist_Slices[i]->GetName()) + "_PDG").c_str(), (TH1**)fMCHist_SlicesPDG[i], 0);
    combo_fMCHist_PDG.Write();

  }

}

void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::ResetAll(){

  Measurement1D::ResetAll();
  for (int i = 0; i < 9; i++){    
    fMCHist_Slices[i]->Reset();
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_SlicesPDG[i]);
  }

}

void T2K_CC0pi_XSec_2DPcos_nu_nonuniform::SetHistograms(){

  // Read in 1D Data Histograms
  fInputFile = new TFile( (FitPar::GetDataBase() + "/T2K/CC0pi/T2K_CC0PI_2DPmuCosmu_Data.root").c_str(),"READ");
  fInputFile->ls();
  
  // Read in 1D Data
  fDataHist = (TH1D*) fInputFile->Get("datahist");
  fDataHist->SetNameTitle("T2K_CC0pi_XSec_2DPcos_nu_nonuniform_data",
			  "T2K_CC0pi_XSec_2DPcos_nu_nonuniform_data");


  TH2D* tempcov = (TH2D*) fInputFile->Get("analysis1_totcov");

  fFullCovar = new TMatrixDSym(fDataHist->GetNbinsX());
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){
    for (int j = 0; j < fDataHist->GetNbinsX(); j++){
      (*fFullCovar)(i,j) = tempcov->GetBinContent(i+1, j+1);
    }
  }
  covar = StatUtils::GetInvert(fFullCovar);
  fDecomp = StatUtils::GetDecomp(fFullCovar);
  
  fDataHist->Reset();

  // Read in 2D Data Slices and Make MC Slices
  int bincount = 0;
  for (int i = 0; i < 9; i++){
  
    // Get Data Histogram
    fInputFile->ls();
    fDataHist_Slices.push_back((TH1D*)fInputFile->Get(Form("dataslice_%i",i))->Clone());
    fDataHist_Slices[i]->SetNameTitle(Form("T2K_CC0pi_XSec_2DPcos_nu_nonuniform_data_Slice%i",i),
				      (Form("T2K_CC0pi_XSec_2DPcos_nu_nonuniform_data_Slice%i",i)));

    // Loop over nbins and set errors from covar
    for (int j = 0; j < fDataHist_Slices[i]->GetNbinsX(); j++){
      fDataHist_Slices[i]->SetBinError(j+1, sqrt((*fFullCovar)(bincount,bincount)) * 1E-38);

      std::cout << "Setting data hist " <<  fDataHist_Slices[i]->GetBinContent(j+1) << " " << fDataHist_Slices[i]->GetBinError(j+1) << std::endl;
      fDataHist->SetBinContent(bincount+1, fDataHist_Slices[i]->GetBinContent(j+1) );
      fDataHist->SetBinError(bincount+1, fDataHist_Slices[i]->GetBinError(j+1) );

      bincount++;
    }

    // Make MC Clones
    fMCHist_Slices.push_back((TH1D*) fDataHist_Slices[i]->Clone());
    fMCHist_Slices[i]->SetNameTitle(Form("T2K_CC0pi_XSec_2DPcos_nu_nonuniform_MC_Slice%i",i),
                                      (Form("T2K_CC0pi_XSec_2DPcos_nu_nonuniform_MC_Slice%i",i)));

    fMCHist_Slices[i]->Reset();

    PlotUtils::CreateNeutModeArray((TH1D*)fMCHist_Slices[i], (TH1**)fMCHist_SlicesPDG[i]);
    PlotUtils::ResetNeutModeArray((TH1**)fMCHist_SlicesPDG[i]);
    
  }
  
  return;
};
