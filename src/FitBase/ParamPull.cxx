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
#include "ParamPull.h"

//*******************************************************************************
ParamPull::ParamPull(std::string name, std::string inputfile, std::string type){
//*******************************************************************************

  fName  = name;
  fInput = inputfile;
  fType  = type;
  
  // Set the pull type
  SetType(fType);
  
  // Setup Histograms from input file
  SetupHistograms(fInput);    
};

//*******************************************************************************
void ParamPull::SetType(std::string type){
//*******************************************************************************

  fType = type;
  // Assume Default if empty
  if (type.empty()){
    ERR(WRN) << "No type specified for ParmPull class " << fName << endl;
    ERR(WRN) << "Assuming GAUSTHROW/GAUSPULL" << endl;

    type = "GAUSTHROW/GAUSPULL";
  }
  
  // Set Dial options
  if (type.find("FRAC") != std::string::npos){

    fDialOptions = "FRAC";
    fPlotTitles  = ";; Fractional RW Value";

  } else if (type.find("ABS") != std::string::npos){

    fDialOptions = "ABS";
    fPlotTitles= ";; ABS RW Value";

  } else {

    fDialOptions = "";
    fPlotTitles= ";; RW Value";

  }

  // Parse throw types
  if (type.find("GAUSPULL") != std::string::npos) fCalcType = kGausPull;
  else fCalcType = kNoPull;

  if (type.find("GAUSTHROW") != std::string::npos) fThrowType = kGausThrow;
  else fCalcType = kNoThrow;

  // Extra check to see if throws or pulls are turned off
  if (type.find("NOPULL") != std::string::npos)  fCalcType = kNoPull;
  if (type.find("NOTHROW") != std::string::npos) fThrowType = kNoThrow;
  
}

//*******************************************************************************
void ParamPull::SetupHistograms(std::string input){
//*******************************************************************************

  // Extract Types from Input
  fFileType = "";
  const int nfiletypes = 4;
  const std::string filetypes[nfiletypes] = {"FIT","ROOT","TXT","DIAL"};

  for (int i = 0; i < nfiletypes; i++) {
    std::string tempTypes = filetypes[i] + ":";
    if (input.find(tempTypes) != std::string::npos) {
      fFileType = filetypes[i];
      input.replace(input.find(tempTypes), tempTypes.size(), "");
      break;
    }
  }

  // Read Files
  if      (!fFileType.compare("FIT")) ReadFitFile(input);
  else if (!fFileType.compare("ROOT")) ReadRootFile(input);
  else if (!fFileType.compare("TXT")) ReadTextFile(input);
  else if (!fFileType.compare("DIAL")) ReadDialInput(input);
  else {
    ERR(FTL) << "Unknown ParamPull Type: " << input << endl;
  }

  // Check Dials are all good
  CheckDialsValid();
  
  // Setup MC Histogram
  fMCHist = (TH1D*) fDataHist->Clone();
  fMCHist->Reset();
  fMCHist->SetNameTitle( (fName + "_MC").c_str(),
			 (fName + " MC" + fPlotTitles).c_str() );

  // Sort Covariances
  fInvCovar = StatUtils::GetInvert(fCovar);
  fDecomp   = StatUtils::GetDecomp(fCovar);
}

//******************************************************************************* 
void ParamPull::ReadFitFile(std::string input){
//*******************************************************************************

  TFile* tempfile = new TFile(input.c_str(),"READ");

  // Read Data
  fDataHist = (TH1D*) tempfile->Get("fit_dials");
  if (!fDataHist){
    ERR(FTL) << "Can't find TH1D hist fit_dials in " << fName << endl;
    ERR(FTL) << "File Entries:" << endl;
    tempfile->ls();

    throw;
  }

  fDataHist->SetDirectory(0);
  fDataHist->SetNameTitle( (fName + "_data").c_str(),
			   (fName + " data" + fPlotTitles).c_str() );

  // Read Covar
  TH2D* tempcov = (TH2D*) tempfile->Get("covariance_free");
  if (!tempcov){
    ERR(FTL) << "Can't find TH2D covariance_free in " << fName << endl;
    ERR(FTL) << "File Entries:" << endl;
    tempfile->ls();

    throw;
  }

  // Setup Covar
  int nbins = fDataHist->GetNbinsX();
  fCovar = new TMatrixDSym( nbins );

  for (int i = 0; i < nbins; i++){
    for(int j = 0; j < nbins; j++){
      (*fCovar)(i,j) = tempcov->GetBinContent(i+1,j+1);
    }
  }
  
  return;
}

//*******************************************************************************
void ParamPull::ReadRootFile(std::string input){
//******************************************************************************* 

  std::vector<std::string> inputlist = PlotUtils::FillVectorSFromString(input,";");

  // Check all given
  if (inputlist.size() < 3){
    ERR(FTL) << "Covar supplied in 'ROOT' format should have 3 semi-colon seperated entries!" << endl
	     << "ROOT:filename;histname;covarname" << endl;
    ERR(FTL) << "histname = TH1D, covarname = TH2D" << endl;
    throw;
  }

  // Get Entries
  std::string filename  = inputlist[0];
  std::string histname  = inputlist[1];
  std::string covarname = inputlist[2];

  // Read File
  TFile* tempfile = new TFile(filename.c_str(),"READ");
  if (tempfile->IsZombie()){
    ERR(FTL) << "Can't find covar file in " << fName << endl;
    throw;
  }

  // Read Hist
  fDataHist = (TH1D*) tempfile->Get(histname.c_str());
  if (!fDataHist){
    ERR(FTL) << "Can't find TH1D hist " << histname << " in " << fName << endl;
    ERR(FTL) << "File Entries:" << endl;
    tempfile->ls();

    throw;
  }
  fDataHist->SetDirectory(0);
  fDataHist->SetNameTitle( (fName + "_data").c_str(),
			   (fName + " data" + fPlotTitles).c_str() );
  
  // Read Covar
  TH2D* tempcov = (TH2D*) tempfile->Get(covarname.c_str());
  if (!tempcov){
    ERR(FTL) << "Can't find TH2D covar " << covarname << " in " << fName << endl;
    ERR(FTL) << "File Entries:" << endl;
    tempfile->ls();

    throw;
  }

  // Setup Covar
  int nbins = fDataHist->GetNbinsX();
  fCovar = new TMatrixDSym( nbins );

  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){
      (*fCovar)(i,j) = tempcov->GetBinContent(i+1,j+1);
    }
  }
  
}

//*******************************************************************************  
void ParamPull::ReadTextFile(std::string input){
//*******************************************************************************

  ERR(FTL) << " TEXT Files not yet supported in ParamPull! " << endl;
  throw;
  
}

//*******************************************************************************  
void ParamPull::ReadDialInput(std::string input){
//*******************************************************************************  

  std::vector<std::string> inputlist = PlotUtils::FillVectorSFromString(input,";");
  if (inputlist.size() < 3){
    ERR(FTL) << "" << endl;
  }

  ERR(FTL) << " DIAL Files not yet supported in ParamPull! " << endl;
  
}


//******************************************************************************* 
bool ParamPull::CheckDialsValid(){
//*******************************************************************************

  std::string helpstring = "";
  
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){
    std::string name = std::string(fDataHist->GetXaxis()->GetBinLabel(i+1));

    // If dial exists its all good
    if (FitBase::GetRW()->DialIncluded(name)) continue;

    // If it doesn't but its a sample norm also continue
    if (name.find("_norm") != std::string::npos){
      ERR(WRN) << "Norm dial included in covar but not set in FitWeight." << endl;
      ERR(WRN) << "Assuming its a sample norm and skipping..." << endl;
    }

    // Dial unknown so print a help statement
    std::ostringstream tempstr;
    tempstr << "unknown_parameter " << name << " "
	    << fDataHist->GetBinContent(i+1) << " "
	    << fDataHist->GetBinContent(i+1) - fDataHist->GetBinError(i+1) << " "
	    << fDataHist->GetBinContent(i+1) + fDataHist->GetBinError(i+1) << " "
	    << fDataHist->GetBinError(i+1) << " ";

    if (!fType.empty()) tempstr << fType << endl;
    else tempstr << "FREE" << endl;
    helpstring += tempstr.str(); 
  }

  // Show statement before failing
  if (!helpstring.empty()){
    
    ERR(WRN) <<"Dial(s) included in covar but not set in FitWeight." << endl
             << "ParamPulls needs to know how you want it to be treated." << endl
             <<"Include the following lines into your card:" << endl;

    std::cout << helpstring << endl;
    sleep(10);
    throw;
    return false;
  } else {
    return true;
  }
}

    

//*******************************************************************************
void ParamPull::Reconfigure(){
//*******************************************************************************
  
  FitWeight* rw = FitBase::GetRW();
  
  // Get Dial Names that are valid
  std::vector<std::string> namevec  = rw->GetDialNames();
  std::vector<double>      valuevec = rw->GetDialValues();

  // Set Bin Values from RW
  for (UInt_t i = 0; i < namevec.size(); i++){

    // Loop over bins and check name matches
    std::string syst = namevec.at(i);
    for (int j = 0; j < fMCHist->GetNbinsX(); j++){

      // If Match set value
      if (!syst.compare( fMCHist->GetXaxis()->GetBinLabel(j+1))){
	
	double curval = rw->GetDialValue(syst, fDialOptions);
	fMCHist->SetBinContent(j+1, curval);
      }
    }
  }

  return;

};

//******************************************************************************* 
double ParamPull::GetLikelihood(){
//*******************************************************************************
  
  double like = 0.0;

  switch(fCalcType){

  // Gaussian Calculation with correlations
  case kGausPull:
    like = StatUtils::GetLikelihoodFromCov(fDataHist, fMCHist, fInvCovar, NULL);
    break;

  // Default says this has no pull
  case kNoThrow:
  default:
    like = 0.0;
    break;
  }
  
  return like;
  
};

//*******************************************************************************
int ParamPull::GetNDOF(){
//*******************************************************************************

  int ndof = 0;

  if (fCalcType != kNoThrow){
    ndof += fDataHist->GetNbinsX();
  }

  return ndof;
};


//******************************************************************************* 
void ParamPull::ThrowCovariance(){
//*******************************************************************************

  // Reset toy for throw
  ResetToy();
  
  // Generate random Gaussian throws
  std::vector<double> randthrows;
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){

    double randtemp = 0.0;
    
    switch(fThrowType){

    // Gaussian Throws
    case kGausThrow:
      randtemp = gRandom->Gaus(0.0,1.0);
      break;

    // No Throws (DEFAULT)
    default:
      break;
    }

    randthrows.push_back(randtemp);
  }

  // Create Bin Modifications
  double totalres = 0.0;
  for (int i = 0; i < fDataHist->GetNbinsX(); i++){

    // Calc Bin Mod
    double binmod  = 0.0;
    for (int j = 0; j < fDataHist->GetNbinsX(); j++){
      binmod += (*fDecomp)(j,i) * randthrows.at(j);
    }

    // Add up fraction dif
    totalres += binmod / fDataHist->GetBinContent(i+1);
    
    // Add to current data
    fDataHist->SetBinContent(i+1,fDataHist->GetBinContent(i+1) + binmod);
  }

  // Print Status
  LOG(REC) << "Created new toy histogram. Total Fractional Dif = "
	   << totalres << endl;
  return;
};


//*******************************************************************************
TH2D ParamPull::GetCovar(){
//*******************************************************************************

  TH2D tempCov = TH2D(*fInvCovar);

  for (int i = 0; i < tempCov.GetNbinsX(); i++){
    tempCov.GetXaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
    tempCov.GetYaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
  }

  tempCov.SetNameTitle((fName + "_INVCOV").c_str(),
			(fName + " InvertedCovariance;Dials;Dials").c_str());

  return tempCov;
}

//*******************************************************************************
TH2D ParamPull::GetFullCovar(){
//*******************************************************************************

  TH2D tempCov = TH2D(*fCovar);

  for (int i = 0; i < tempCov.GetNbinsX(); i++){
    tempCov.GetXaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
    tempCov.GetYaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
  }

  tempCov.SetNameTitle((fName + "_COV").c_str(),
			(fName + " Covariance;Dials;Dials").c_str());

  return tempCov;
}

//*******************************************************************************
TH2D ParamPull::GetDecompCovar(){
//*******************************************************************************

  TH2D tempCov = TH2D(*fCovar);

  for (int i = 0; i < tempCov.GetNbinsX(); i++){
    tempCov.GetXaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
    tempCov.GetYaxis()->SetBinLabel(i+1,fDataHist->GetXaxis()->GetBinLabel(i+1));
  }

  tempCov.SetNameTitle((fName + "_DEC").c_str(),
			(fName + " Decomposition;Dials;Dials").c_str());

  return tempCov;
}


//******************************************************************************* 
void ParamPull::Write(std::string writeoptt){
//******************************************************************************* 

  fDataHist->Write();
  fMCHist->Write();
  
  GetCovar().Write();
  GetFullCovar().Write();
  GetDecompCovar().Write();
  
  return;
};

