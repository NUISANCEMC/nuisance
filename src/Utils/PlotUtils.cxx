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

#include "PlotUtils.h"
#include "FitEvent.h"
#include "FitParameters.h"

// MOVE TO MB UTILS!
// This function is intended to be modified to enforce a consistent masking for all models.
TH2D* PlotUtils::SetMaskHist(std::string type, TH2D* data){
  
  TH2D *fMaskHist = (TH2D*)data->Clone("fMaskHist");  

  for (int xBin = 0; xBin < fMaskHist->GetNbinsX(); ++xBin){
    for (int yBin = 0; yBin < fMaskHist->GetNbinsY(); ++yBin){
      if (data->GetBinContent(xBin+1, yBin+1) == 0) fMaskHist->SetBinContent(xBin+1, yBin+1, 0);
      else fMaskHist->SetBinContent(xBin+1, yBin+1, 0.5);

      if (!type.compare("MB_numu_2D")){
	if (yBin == 19 && xBin < 8) fMaskHist->SetBinContent(xBin+1, yBin+1, 1.0);
      } else {
	if (yBin == 19 && xBin < 11) fMaskHist->SetBinContent(xBin+1, yBin+1, 1.0);
      }
      if (yBin == 18 && xBin < 3) fMaskHist->SetBinContent(xBin+1, yBin+1, 1.0);
      if (yBin == 17 && xBin < 2) fMaskHist->SetBinContent(xBin+1, yBin+1, 1.0);
      if (yBin == 16 && xBin < 1) fMaskHist->SetBinContent(xBin+1, yBin+1, 1.0);
    }
  }

  return fMaskHist;
};

// MOVE TO GENERAL UTILS?
bool PlotUtils::CheckObjectWithName(TFile *inFile, std::string substring){

  TIter nextkey(inFile->GetListOfKeys());
  TKey *key;

  while ( (key = (TKey*)nextkey()) ) {
    std::string test(key->GetName());
    if (test.find(substring) != std::string::npos) return true;
  }
  return false;
};

// MOVE TO GENERAL UTILS?
std::string PlotUtils::GetObjectWithName(TFile *inFile, std::string substring){

  TIter nextkey(inFile->GetListOfKeys());
  TKey *key;
  std::string output="NULL";

  while ( (key = (TKey*)nextkey()) ) {
    std::string test(key->GetName());
    if (test.find(substring) != std::string::npos)
      output = test;
  }
  
  return output;
};

void PlotUtils::CreateNeutModeArray(TH1* hist, TH1* neutarray[]){

  for (int i = 0; i < 60; i++){
    neutarray[i] = (TH1*)hist->Clone(Form("%s_NMODE_%i",hist->GetName(),i));
  }
  return;
};

void PlotUtils::DeleteNeutModeArray(TH1* neutarray[]){
  for (int i = 0; i < 60; i++){
    delete neutarray[i];
  }
  return;
};


void PlotUtils::FillNeutModeArray(TH1D* hist[], int mode, double xval, double weight){
  if (abs(mode) > 60) return;
  hist[abs(mode)]->Fill(xval, weight);
  return;
};

void PlotUtils::FillNeutModeArray(TH2D* hist[], int mode, double xval, double yval, double weight){
  if (abs(mode) > 60) return;
  hist[abs(mode)]->Fill(xval,yval,weight);
  return;
};


THStack PlotUtils::GetNeutModeStack(std::string title, TH1* ModeStack[], int option) {
  
  (void) option;
  THStack allmodes = THStack(title.c_str(),title.c_str());

  for (int i = 0; i < 60; i++){
    allmodes.Add(ModeStack[i]);
  }

  // Credit to Clarence for copying all this out.

  // CC 
  ModeStack[1]->SetTitle("CCQE");
  ModeStack[1]->SetFillColor(kBlue);
 // ModeStack[1]->SetFillStyle(3444);
  ModeStack[1]->SetLineColor(kBlue); 
  ModeStack[2]->SetTitle("2p/2h Nieves");
  ModeStack[2]->SetFillColor(kRed);
  //ModeStack[2]->SetFillStyle(3344);
  ModeStack[2]->SetLineColor(kRed); 

  //ModeStack[11]->SetTitle("#it{#nu + p #rightarrow l^{-} + p + #pi^{+}}");
  ModeStack[11]->SetTitle("CC1#pi^{+} on p");
  ModeStack[11]->SetFillColor(kGreen);
  //ModeStack[11]->SetFillStyle(3004);
  ModeStack[11]->SetLineColor(kGreen); 
  //ModeStack[12]->SetTitle("#it{#nu + n #rightarrow l^{-} + p + #pi^{0}}");
  ModeStack[12]->SetTitle("CC1#pi^{0} on n");
  ModeStack[12]->SetFillColor(kGreen+3);
  //ModeStack[12]->SetFillStyle(3005);
  ModeStack[12]->SetLineColor(kGreen); 
  //ModeStack[13]->SetTitle("#it{#nu + n #rightarrow l^{-} + n + #pi^{+}}");
  ModeStack[13]->SetTitle("CC1#pi^{+} on n");
  ModeStack[13]->SetFillColor(kGreen-2);
  //ModeStack[13]->SetFillStyle(3004);
  ModeStack[13]->SetLineColor(kGreen); 

  ModeStack[16]->SetTitle("CC coherent");
  ModeStack[16]->SetFillColor(kBlue);
  //ModeStack[16]->SetFillStyle(3644);
  ModeStack[16]->SetLineColor(kBlue); 

  //ModeStack[17]->SetTitle("#it{#nu + n #rightarrow l^{-} + p + #gamma}");
  ModeStack[17]->SetTitle("CC1#gamma");
  ModeStack[17]->SetFillColor(kMagenta);
  //ModeStack[17]->SetFillStyle(3001);
  ModeStack[17]->SetLineColor(kMagenta); 

  ModeStack[21]->SetTitle("Multi #pi (1.3 < W < 2.0)");
  ModeStack[21]->SetFillColor(kYellow);
  //ModeStack[21]->SetFillStyle(3005);
  ModeStack[21]->SetLineColor(kYellow); 

  //ModeStack[22]->SetTitle("#it{#nu + n #rightarrow l^{-} + p + #eta^{0}}");
  ModeStack[22]->SetTitle("CC1#eta^{0} on n");
  ModeStack[22]->SetFillColor(kYellow-2);
  //ModeStack[22]->SetFillStyle(3013);
  ModeStack[22]->SetLineColor(kYellow-2); 
  //ModeStack[23]->SetTitle("#it{#nu + n #rightarrow l^{-} + #Lambda + K^{+}}");
  ModeStack[23]->SetTitle("CC1#Labda1K^{+}");
  ModeStack[23]->SetFillColor(kYellow-6);
  //ModeStack[23]->SetFillStyle(3013);
  ModeStack[23]->SetLineColor(kYellow-6); 

  ModeStack[26]->SetTitle("DIS (W > 2.0)");
  ModeStack[26]->SetFillColor(kRed);
  //ModeStack[26]->SetFillStyle(3006);
  ModeStack[26]->SetLineColor(kRed); 

  // NC
  //ModeStack[31]->SetTitle("#it{#nu + n #rightarrow #nu + n + #pi^{0}}");
  ModeStack[31]->SetTitle("NC1#pi^{0} on n");
  ModeStack[31]->SetFillColor(kBlue);
  //ModeStack[31]->SetFillStyle(3004);
  ModeStack[31]->SetLineColor(kBlue); 
  //ModeStack[32]->SetTitle("#it{#nu + p #rightarrow #nu + p + #pi^{0}}");
  ModeStack[32]->SetTitle("NC1#pi^{0} on p");
  ModeStack[32]->SetFillColor(kBlue+3);
  //ModeStack[32]->SetFillStyle(3004);
  ModeStack[32]->SetLineColor(kBlue+3); 
  //ModeStack[33]->SetTitle("#it{#nu + n #rightarrow #nu + p + #pi^{-}}");
  ModeStack[33]->SetTitle("NC1#pi^{-} on n");
  ModeStack[33]->SetFillColor(kBlue-2);
  //ModeStack[33]->SetFillStyle(3005);
  ModeStack[33]->SetLineColor(kBlue-2); 
  //ModeStack[34]->SetTitle("#it{#nu + p #rightarrow #nu + n + #pi^{+}}");
  ModeStack[34]->SetTitle("NC1#pi^{+} on p");
  ModeStack[34]->SetFillColor(kBlue-8);
  //ModeStack[34]->SetFillStyle(3005);
  ModeStack[34]->SetLineColor(kBlue-8); 

  ModeStack[36]->SetTitle("NC Coherent");
  ModeStack[36]->SetFillColor(kBlue+8);
  //ModeStack[36]->SetFillStyle(3644);
  ModeStack[36]->SetLineColor(kBlue+8); 

  //ModeStack[38]->SetTitle("#it{#nu + n #rightarrow #nu + n + #gamma}");
  ModeStack[38]->SetTitle("NC1#gamma on n");
  ModeStack[38]->SetFillColor(kMagenta);
  //ModeStack[38]->SetFillStyle(3001);
  ModeStack[38]->SetLineColor(kMagenta); 
  //ModeStack[39]->SetTitle("#it{#nu + p #rightarrow #nu + p + #gamma}");
  ModeStack[39]->SetTitle("NC1#gamma on p");
  ModeStack[39]->SetFillColor(kMagenta-10);
  //ModeStack[39]->SetFillStyle(3001);
  ModeStack[39]->SetLineColor(kMagenta-10); 

  ModeStack[41]->SetTitle("Multi #pi (1.3 < W < 2.0)");
  ModeStack[41]->SetFillColor(kBlue-10);
  //ModeStack[41]->SetFillStyle(3005);
  ModeStack[41]->SetLineColor(kBlue-10); 

  //ModeStack[42]->SetTitle("#it{#nu + n #rightarrow #nu + n + #eta^{0}}");
  ModeStack[42]->SetTitle("NC1#eta^{0} on n");
  ModeStack[42]->SetFillColor(kYellow-2);
  //ModeStack[42]->SetFillStyle(3013);
  ModeStack[42]->SetLineColor(kYellow-2); 
  //ModeStack[43]->SetTitle("#it{#nu + p #rightarrow #nu + p + #eta^{0}}");
  ModeStack[43]->SetTitle("NC1#eta^{0} on p");
  ModeStack[43]->SetFillColor(kYellow-4);
  //ModeStack[43]->SetFillStyle(3013);
  ModeStack[43]->SetLineColor(kYellow-4); 

  //ModeStack[44]->SetTitle("#it{#nu + n #rightarrow #nu + #Lambda + K^{0}}");
  ModeStack[44]->SetTitle("NC1#Lambda1K^{0} on n");
  ModeStack[44]->SetFillColor(kYellow - 6);
  //ModeStack[44]->SetFillStyle(3014);
  ModeStack[44]->SetLineColor(kYellow - 6); 
  //ModeStack[45]->SetTitle("#it{#nu + p #rightarrow #nu + #Lambda + K^{+}}");
  ModeStack[45]->SetTitle("NC1#Lambda1K^{+}");
  ModeStack[45]->SetFillColor(kYellow - 10);
  //ModeStack[45]->SetFillStyle(3014);
  ModeStack[45]->SetLineColor(kYellow - 10); 

  ModeStack[46]->SetTitle("DIS (W > 2.0)");
  ModeStack[46]->SetFillColor(kRed);
  //ModeStack[46]->SetFillStyle(3006);
  ModeStack[46]->SetLineColor(kRed); 

  //ModeStack[51]->SetTitle("#it{#nu + p #rightarrow #nu + p}");
  ModeStack[51]->SetTitle("NC on p");
  ModeStack[51]->SetFillColor(kBlack);
  //ModeStack[51]->SetFillStyle(3444);
  ModeStack[51]->SetLineColor(kBlack); 
  //ModeStack[52]->SetTitle("#it{#nu + n #rightarrow #nu + n}");
  ModeStack[52]->SetTitle("NC on n");
  ModeStack[52]->SetFillColor(kGray);
  //ModeStack[52]->SetFillStyle(3444);
  ModeStack[52]->SetLineColor(kGray); 

  


  return allmodes;


};


TLegend PlotUtils::GenerateStackLegend(THStack stack, int xlow, int ylow, int xhigh, int yhigh){

  TLegend leg = TLegend(xlow,ylow,xhigh,yhigh);

  TObjArray* histarray = stack.GetStack();

  int nhist = histarray->GetEntries();
  for (int i = 0; i < nhist; i++){
    TH1* hist = (TH1*)(histarray->At(i));
    leg.AddEntry( (hist), ((TH1*)histarray->At(i))->GetTitle(), "fl");
  }

  leg.SetName(Form("%s_LEG",stack.GetName()));
 
  return leg;
  
};


void PlotUtils::ScaleNeutModeArray(TH1* hist[], double factor, std::string option){
  
  for (int i = 0; i < 60; i++){
    if (hist[i])
      hist[i]->Scale(factor,option.c_str());
  }  

  return;
};

void PlotUtils::ResetNeutModeArray(TH1* hist[]){
  
  for (int i = 0; i < 60; i++){
    if (hist[i])
      hist[i]->Reset();
  }
 
  return;
};


//********************************************************************
// This assumes the Enu axis is the x axis, as is the case for MiniBooNE 2D distributions
void PlotUtils::FluxUnfoldedScaling(TH2D* fMCHist, TH1D* fhist, TH1D* ehist, double scalefactor) {
//********************************************************************
  
  // Make clones to avoid changing stuff
  TH1D* eventhist = (TH1D*)ehist->Clone();
  TH1D* fFluxHist = (TH1D*)fhist->Clone();

  // Undo width integral in SF  
  fMCHist->Scale( scalefactor / eventhist->Integral(1,eventhist->GetNbinsX()+1,"width"));

  // Standardise The Flux       
  eventhist->Scale(1.0/fFluxHist->Integral());
  fFluxHist->Scale(1.0/fFluxHist->Integral());

  // Do interpolation for 2D plots?
  // fFluxHist = PlotUtils::InterpolateFineHistogram(fFluxHist,100,"width");
  // eventhist = PlotUtils::InterpolateFineHistogram(eventhist,100,"width");

  // eventhist->Scale(1.0/fFluxHist->Integral());
  // fFluxHist->Scale(1.0/fFluxHist->Integral());

  // Scale fMCHist by eventhist integral          
  fMCHist->Scale( eventhist->Integral(1,eventhist->GetNbinsX()+1) );

  // Now Get a flux PDF assuming X axis is Enu
  TH1D* pdfflux = (TH1D*) fMCHist->ProjectionX()->Clone();
  //  pdfflux->Write( (std::string(fMCHist->GetName()) + "_PROJX").c_str());
  pdfflux->Reset();

  // Awful MiniBooNE Check for the time being
  bool ismb = std::string(fMCHist->GetName()).find("MiniBooNE") != std::string::npos;

  for (int i = 0; i < pdfflux->GetNbinsX(); i++){

    double Ml = pdfflux->GetXaxis()->GetBinLowEdge(i+1);
    double Mh = pdfflux->GetXaxis()->GetBinLowEdge(i+2);
    //double Mc = pdfflux->GetXaxis()->GetBinCenter(i+1);
    //double Mw = pdfflux->GetBinWidth(i+1);
    double fluxint = 0.0;

    // Scaling to match flux for MB
    if (ismb){
      Ml /= 1.E3;
      Mh /= 1.E3;
      //  Mc /= 1.E3;
      //  Mw /= 1.E3;
    }

    for (int j = 0; j < fFluxHist->GetNbinsX(); j++){

      //double Fc = fFluxHist->GetXaxis()->GetBinCenter(j+1);
      double Fl = fFluxHist->GetXaxis()->GetBinLowEdge(j+1);
      double Fh = fFluxHist->GetXaxis()->GetBinLowEdge(j+2);
      double Fe = fFluxHist->GetBinContent(j+1);
      double Fw = fFluxHist->GetXaxis()->GetBinWidth(j+1);

      if (Fl >= Ml and Fh <= Mh){ fluxint += Fe; }
      else if (Fl < Ml and Fl < Mh and Fh > Ml and Fh < Mh){  fluxint += Fe * (Fh - Ml)/Fw;  }
      else if (Fh > Mh and Fl < Mh and Fh > Ml and Fl > Ml){  fluxint += Fe * (Mh - Fl)/Fw; }
      else if (Ml >= Fl and Mh <= Fh){  fluxint += Fe * (Mh - Ml)/Fw; }
      else { continue; }

    }

    pdfflux->SetBinContent(i+1, fluxint);
  }

  for (int i = 0; i < fMCHist->GetNbinsX(); i++) {
    for (int j = 0; j < fMCHist->GetNbinsY(); j++){ 

      if (pdfflux->GetBinContent(i+1) == 0.0) continue;

      double binWidth = fMCHist->GetYaxis()->GetBinLowEdge(j+2) - fMCHist->GetYaxis()->GetBinLowEdge(j+1);
      fMCHist->SetBinContent(i+1, j+1, fMCHist->GetBinContent(i+1,j+1) /pdfflux->GetBinContent(i+1) / binWidth);
      fMCHist->SetBinError(i+1, j+1, fMCHist->GetBinError(i+1,j+1) /pdfflux->GetBinContent(i+1) / binWidth);

    }
  }

  delete eventhist;
  delete fFluxHist;

  return;
};

TH1D* PlotUtils::InterpolateFineHistogram(TH1D* hist, int res, std::string opt){

  int nbins = hist->GetNbinsX();
  double elow  = hist->GetXaxis()->GetBinLowEdge(1);
  double ehigh = hist->GetXaxis()->GetBinLowEdge(nbins+1);
  bool width = true; //opt.find("width") != std::string::npos;

  TH1D* fine = new TH1D("fine","fine", nbins*res, elow, ehigh);

  TGraph* temp = new TGraph();

  for (int i = 0; i < nbins; i++){
      double E = hist->GetXaxis()->GetBinCenter(i+1);
      double C = hist->GetBinContent(i+1);
      double W = hist->GetXaxis()->GetBinWidth(i+1);
      if (!width) W = 1.0;

      if (W != 0.0) temp->SetPoint( temp->GetN(), E, C / W );
  }

  for (int i = 0; i < fine->GetNbinsX(); i++){
      double E = fine->GetXaxis()->GetBinCenter(i+1);
      double W = fine->GetBinWidth(i+1);
      if (!width) W = 1.0;

      fine->SetBinContent(i+1, temp->Eval(E,0,"S") * W);
  }

  fine->Scale(hist->Integral(1,hist->GetNbinsX()+1) / fine->Integral(1, fine->GetNbinsX()+1));
  std::cout << "Interpolation Difference = " << fine->Integral(1,fine->GetNbinsX()+1) << "/" << hist->Integral(1,hist->GetNbinsX()+1) << std::endl; 
	   
  return fine;
}




//******************************************************************** 
// This interpolates the flux by a TGraph instead of requiring the flux and MC flux to have the same binning
void PlotUtils::FluxUnfoldedScaling(TH1D* mcHist, TH1D* fhist, TH1D* ehist, double scalefactor, int nevents) {
//******************************************************************** 

  TH1D* eventhist = (TH1D*)ehist->Clone();
  TH1D* fFluxHist = (TH1D*)fhist->Clone();

  if (FitPar::Config().GetParB("save_flux_debug")){
    std::string name = std::string(mcHist->GetName());
    
    mcHist->Write((name + "_UNF_MC").c_str());
    fFluxHist->Write((name + "_UNF_FLUX").c_str());
    eventhist->Write((name + "_UNF_EVT").c_str());
    
    TH1D* scalehist = new TH1D("scalehist","scalehist",1,0.0,1.0);
    scalehist->SetBinContent(1,scalefactor);
    scalehist->SetBinContent(2,nevents);
  
    scalehist->Write((name + "_UNF_SCALE").c_str());  
  }

  // Undo width integral in SF
  mcHist->Scale( scalefactor / eventhist->Integral(1,eventhist->GetNbinsX()+1,"width"));
  
  // Standardise The Flux
  eventhist->Scale(1.0/fFluxHist->Integral());
  fFluxHist->Scale(1.0/fFluxHist->Integral());

  // Scale mcHist by eventhist integral
  mcHist->Scale( eventhist->Integral(1,eventhist->GetNbinsX()+1) );
  
  // Now Get a flux PDF
  TH1D* pdfflux = (TH1D*) mcHist->Clone();
  pdfflux->Reset();

  for (int i = 0; i < mcHist->GetNbinsX(); i++){

    double Ml = mcHist->GetXaxis()->GetBinLowEdge(i+1);
    double Mh = mcHist->GetXaxis()->GetBinLowEdge(i+2);
    //double Mc = mcHist->GetXaxis()->GetBinCenter(i+1);
    //double Me = mcHist->GetBinContent(i+1);
    //double Mw = mcHist->GetBinWidth(i+1);
    double fluxint = 0.0;

    for (int j = 0; j < fFluxHist->GetNbinsX(); j++){
      
      //double Fc = fFluxHist->GetXaxis()->GetBinCenter(j+1);
      double Fl = fFluxHist->GetXaxis()->GetBinLowEdge(j+1);
      double Fh = fFluxHist->GetXaxis()->GetBinLowEdge(j+2);
      double Fe = fFluxHist->GetBinContent(j+1);
      double Fw = fFluxHist->GetXaxis()->GetBinWidth(j+1);
      
      if (Fl >= Ml and Fh <= Mh){ fluxint += Fe; }
      else if (Fl < Ml and Fl < Mh and Fh > Ml and Fh < Mh){  fluxint += Fe * (Fh - Ml)/Fw;  }
      else if (Fh > Mh and Fl < Mh and Fh > Ml and Fl > Ml){  fluxint += Fe * (Mh - Fl)/Fw; }
      else if (Ml >= Fl and Mh <= Fh){	fluxint += Fe * (Mh - Ml)/Fw; }
      else { continue; }

    }

    pdfflux->SetBinContent(i+1, fluxint);
  }

  // Scale MC hist by pdfflux
  for (int i = 0; i < mcHist->GetNbinsX(); i++){
    if (pdfflux->GetBinContent(i+1) == 0.0) continue;

    mcHist->SetBinContent(i+1, mcHist->GetBinContent(i+1) / pdfflux->GetBinContent(i+1));
    mcHist->SetBinError(i+1, mcHist->GetBinError(i+1) / pdfflux->GetBinContent(i+1));
  }

  delete eventhist;
  delete fFluxHist;

  return;
};


// MOVE TO GENERAL UTILS
//********************************************************************                                                                                                
void PlotUtils::Set2DHistFromText(std::string dataFile, TH2* hist, double norm, bool skipbins){
//********************************************************************

  std::string line;
  std::ifstream data(dataFile.c_str(),ifstream::in);

  int yBin = 0;
  while(std::getline(data >> std::ws, line, '\n')){
    
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");

    // Loop over entries and insert them into the histogram
    for (uint xBin = 0; xBin < entries.size(); xBin++){
    
      if (!skipbins or entries[xBin] != -1.0)
	hist->SetBinContent(xBin+1, yBin+1, entries[xBin]*norm);
    }
    yBin++;
  }

  return;

}



// MOVE TO GENERAL UTILS
TH1D* PlotUtils::GetTH1DFromFile(std::string dataFile, std::string title, std::string fPlotTitles, std::string alt_name){
  
  TH1D* tempPlot;

  // If format is a root file
  if (dataFile.find(".root") != std::string::npos){
    
    TFile* temp_infile = new TFile(dataFile.c_str(), "READ");
    tempPlot = (TH1D*)temp_infile->Get(title.c_str());
    tempPlot->SetDirectory(0);

    temp_infile->Close();
    delete temp_infile;

  // Else its a space seperated txt file
  } else {
    
    // Make a TGraph Errors 
    TGraphErrors *gr = new TGraphErrors(dataFile.c_str(),"%lg %lg %lg");
    if (gr->IsZombie()) {
      exit(-1);
    }
    double* bins = gr->GetX();
    double* values = gr->GetY();
    double* errors = gr->GetEY();
    int npoints = gr->GetN();

    // Fill the histogram from it
    tempPlot = new TH1D(title.c_str(),title.c_str(),npoints-1,bins);

    for (int i = 0; i < npoints; ++i){
      tempPlot->SetBinContent(i+1, values[i]);
      tempPlot->SetBinError(i+1, errors[i]);
    }

    delete gr;
  }

  // Allow alternate naming for root files
  if (!alt_name.empty()){
    tempPlot->SetNameTitle(alt_name.c_str(), alt_name.c_str());
  }

  // Allow alternate axis titles
  if (!fPlotTitles.empty()){
    tempPlot->SetNameTitle( tempPlot->GetName(), (std::string(tempPlot->GetTitle()) + fPlotTitles).c_str() );
  }
  
  return tempPlot;
};

TH1D* PlotUtils::GetRatioPlot(TH1D* hist1,   TH1D* hist2){

  // make copy of first hist
  TH1D* new_hist = (TH1D*) hist1->Clone();
  
  // Do bins and errors ourselves as scales can go awkward
  for (int i = 0; i < new_hist->GetNbinsX(); i++){

    if (hist2->GetBinContent(i+1) == 0.0){
      new_hist->SetBinContent(i+1, 0.0);
    }

    new_hist->SetBinContent(i+1, hist1->GetBinContent(i+1) / hist2->GetBinContent(i+1) );
    new_hist->SetBinError(i+1,   hist1->GetBinError(i+1)   / hist2->GetBinContent(i+1) );
  }

  return new_hist;

};

TH1D* PlotUtils::GetRenormalisedPlot(TH1D* hist1,   TH1D* hist2){

  // make copy of first hist
  TH1D* new_hist = (TH1D*) hist1->Clone();

  if (hist1->Integral("width") == 0 or hist2->Integral("width") == 0){
    new_hist->Reset();
    return new_hist;
  }

  Double_t scaleF = hist2->Integral("width") / hist1->Integral("width");
  new_hist->Scale(scaleF);
  
  return new_hist;
};


TH1D* PlotUtils::GetShapePlot(TH1D* hist1){

  // make copy of first hist
  TH1D* new_hist = (TH1D*) hist1->Clone();

  if (hist1->Integral("width") == 0){
    new_hist->Reset();
    return new_hist;
  }

  Double_t scaleF1 = 1.0 / hist1->Integral("width");
  
  new_hist->Scale(scaleF1);
    
  return new_hist;
};

TH1D* PlotUtils::GetShapeRatio(TH1D* hist1,   TH1D* hist2){

  TH1D* new_hist1 = GetShapePlot(hist1);
  TH1D* new_hist2 = GetShapePlot(hist2);

  // Do bins and errors ourselves as scales can go awkward
  for (int i = 0; i < new_hist1->GetNbinsX(); i++){

    if (hist2->GetBinContent(i+1) == 0){
      new_hist1->SetBinContent(i+1, 0.0);
    }

    new_hist1->SetBinContent(i+1, new_hist1->GetBinContent(i+1) / new_hist2->GetBinContent(i+1) );
    new_hist1->SetBinError(i+1,   new_hist1->GetBinError(i+1)   / new_hist2->GetBinContent(i+1) );
  }

  delete new_hist2;

  return new_hist1;
};

TH2D* PlotUtils::GetCovarPlot(TMatrixDSym* cov, std::string name, std::string title){

  TH2D* CovarPlot;

  if (cov) CovarPlot = new TH2D((*cov));
  else CovarPlot = new TH2D(name.c_str(), title.c_str(), 1,0,1,1,0,1);

  CovarPlot->SetName(name.c_str());
  CovarPlot->SetTitle(title.c_str());
  
  return CovarPlot;

}


TH2D* PlotUtils::GetFullCovarPlot(TMatrixDSym* cov, std::string name){
  return PlotUtils::GetCovarPlot(cov, name + "_COV", name + "_COV;Bins;Bins;Covariance (#times10^{-76})");
}

TH2D* PlotUtils::GetInvCovarPlot(TMatrixDSym* cov, std::string name){
  return PlotUtils::GetCovarPlot(cov, name + "_INVCOV", name + "_INVCOV;Bins;Bins;Inv. Covariance (#times10^{-76})");
}

TH2D* PlotUtils::GetDecompCovarPlot(TMatrixDSym* cov, std::string name){
  return PlotUtils::GetCovarPlot(cov, name + "_DECCOV", name + "_DECCOV;Bins;Bins;Decomp Covariance (#times10^{-76})");
}

TH1D* PlotUtils::GetTH1DFromRootFile(std::string file, std::string name){

  if (name.empty()){
    std::vector<std::string> tempfile = GeneralUtils::ParseToStr(file,";");
    file = tempfile[0];
    name = tempfile[1];
  }

  TFile* rootHistFile = new TFile(file.c_str(),"READ");
  TH1D* tempHist =  (TH1D*) rootHistFile->Get(name.c_str())->Clone();
  tempHist->SetDirectory(0);

  rootHistFile->Close();

  return tempHist;
}


TH2D* PlotUtils::GetTH2DFromRootFile(std::string file, std::string name){

  if (name.empty()){
    std::vector<std::string> tempfile = GeneralUtils::ParseToStr(file,";");
    file = tempfile[0];
    name = tempfile[1];
  }

  TFile* rootHistFile = new TFile(file.c_str(),"READ");
  TH2D* tempHist =  (TH2D*) rootHistFile->Get(name.c_str())->Clone();
  tempHist->SetDirectory(0);

  rootHistFile->Close();

  return tempHist;
}


TH2D* PlotUtils::GetTH2DFromTextFile(std::string file){

  /// Contents should be 
  /// Low Edfe


  return NULL;
}


void PlotUtils::AddNeutModeArray(TH1D* hist1[], TH1D* hist2[], double scaling){

  for (int i = 0; i < 60; i++){
    if (!hist2[i]) continue;
    if (!hist1[i]) continue;
    hist1[i]->Add(hist2[i], scaling);
  }
  return;
}


void PlotUtils::ScaleToData(TH1D* data, TH1D* mc, TH1I* mask){

  double scaleF = GetDataMCRatio(data, mc, mask);
  mc->Scale(scaleF);
  
  return;
}


void PlotUtils::MaskBins(TH1D* hist, TH1I* mask){

  for (int i = 0; i < hist->GetNbinsX(); i++){

    if (mask->GetBinContent(i+1) <= 0.5) continue;

    hist->SetBinContent(i+1, 0.0);
    hist->SetBinError(i+1, 0.0);
    
    LOG(REC)<<"MaskBins: Set "<<hist->GetName()<<" Bin "<<i+1<<" to 0.0 +- 0.0"<<std::endl;

  }
 
  return;
}

void PlotUtils::MaskBins(TH2D* hist, TH2I* mask){

  for (int i = 0; i < hist->GetNbinsX(); i++){
    for (int j = 0; j < hist->GetNbinsY(); j++){

      if (mask->GetBinContent(i+1,j+1) <= 0.5) continue;

      hist->SetBinContent(i+1,j+1, 0.0);
      hist->SetBinError(i+1, j+1,0.0);

      LOG(REC)<<"MaskBins: Set "<<hist->GetName()<<" Bin "<<i+1<<" "<<j+1<<" to 0.0 +- 0.0"<<std::endl;
    }
  }
  return;

}

double PlotUtils::GetDataMCRatio(TH1D* data, TH1D* mc, TH1I* mask){

  double rat = 1.0;
  
  TH1D* newmc = (TH1D*)mc->Clone();
  TH1D* newdt = (TH1D*)data->Clone();

  if (mask){
    MaskBins(newmc, mask);
    MaskBins(newdt, mask);
  }

  rat = newdt->Integral()/newmc->Integral();

  return rat;
}

TH2D* PlotUtils::GetCorrelationPlot(TH2D* cov, std::string name){

  TH2D* cor = (TH2D*)cov->Clone();
  cor->Reset();

  for (int i = 0; i < cov->GetNbinsX(); i++){
    for (int j = 0; j < cov->GetNbinsY(); j++){
   
      if (cov->GetBinContent(i+1,i+1) != 0.0 and cov->GetBinContent(j+1,j+1) != 0.0)
	cor->SetBinContent(i+1,j+1, cov->GetBinContent(i+1,j+1)/(sqrt(cov->GetBinContent(i+1,i+1) * cov->GetBinContent(j+1,j+1))));

    }
  }

  if (!name.empty()) {
    cor->SetNameTitle(name.c_str(), (name + ";;correlation").c_str());
  }

  cor->SetMinimum(-1);
  cor->SetMaximum(1);

  return cor;
}

TH2D* PlotUtils::GetDecompPlot(TH2D* cov, std::string name){

  TMatrixDSym* covarmat = new TMatrixDSym(cov->GetNbinsX());

  for (int i = 0; i < cov->GetNbinsX(); i++)
    for (int j = 0; j < cov->GetNbinsY(); j++)
      (*covarmat)(i,j) = cov->GetBinContent(i+1,j+1);

  TMatrixDSym* decompmat = StatUtils::GetDecomp(covarmat);

  TH2D* dec = (TH2D*) cov->Clone();
  for (int i = 0; i < cov->GetNbinsX(); i++)
    for (int j = 0; j < cov->GetNbinsY(); j++)
      dec->SetBinContent(i+1,j+1,(*decompmat)(i,j));

  delete covarmat;
  delete decompmat;

  dec->SetNameTitle(name.c_str(),(name + ";;;decomposition").c_str());

  return dec;
}


TH2D* PlotUtils::MergeIntoTH2D(TH1D* xhist, TH1D* yhist, std::string zname){


  std::vector<double> xedges, yedges;
  for (int i = 0; i < xhist->GetNbinsX()+2; i++) {
    xedges.push_back(xhist->GetXaxis()->GetBinLowEdge(i+1));
  }
  for (int i = 0; i < yhist->GetNbinsX()+2; i++) {
    yedges.push_back(yhist->GetXaxis()->GetBinLowEdge(i+1));
  }

  int nbinsx = xhist->GetNbinsX();
  int nbinsy = yhist->GetNbinsX();

  std::string name = std::string(xhist->GetName())+"_vs_"+std::string(yhist->GetName());
  std::string titles = ";" + std::string(xhist->GetXaxis()->GetTitle()) + ";" + std::string(yhist->GetXaxis()->GetTitle()) + ";" + zname;

  TH2D* newplot = new TH2D(name.c_str(),(name+titles).c_str(), nbinsx, &xedges[0], nbinsy, &yedges[0]);

  return newplot;
}

//***************************************************
void PlotUtils::MatchEmptyBins(TH1D* data, TH1D* mc){
//**************************************************

  for (int i = 0; i < data->GetNbinsX(); i++){
    if (data->GetBinContent(i+1) == 0.0 or data->GetBinError(i+1) == 0.0)
      mc->SetBinContent(i+1,0.0);
  }
  
  return;
}


//***************************************************
void PlotUtils::MatchEmptyBins(TH2D* data, TH2D* mc){
//**************************************************

  for (int i = 0; i < data->GetNbinsX(); i++){
    for (int j = 0; j < data->GetNbinsY(); j++){
      if (data->GetBinContent(i+1,j+1) == 0.0 or data->GetBinError(i+1,j+1) == 0.0)
	mc->SetBinContent(i+1,j+1,0.0);
    }
  }

  return;
}


//***************************************************    
TH1D* PlotUtils::GetProjectionX(TH2D* hist, TH2I* mask){
//***************************************************    

  TH2D* maskedhist = StatUtils::ApplyHistogramMasking(hist, mask);

  TH1D* hist_X = maskedhist->ProjectionX();

  delete maskedhist;
  return hist_X;
}


//***************************************************
TH1D* PlotUtils::GetProjectionY(TH2D* hist, TH2I* mask){
//***************************************************

  TH2D* maskedhist = StatUtils::ApplyHistogramMasking(hist, mask);

  TH1D* hist_Y = maskedhist->ProjectionY();

  delete maskedhist;
  return hist_Y;
}
