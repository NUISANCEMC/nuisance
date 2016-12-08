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

#include "Measurement2D.h"
#include "TDecompChol.h"

//********************************************************************
Measurement2D::Measurement2D() {
//********************************************************************

  covar = NULL;
  fDecomp = NULL;
  fFullCovar = NULL;

  fMCHist = NULL;
  fMCFine = NULL;
  fDataHist = NULL;

  fMCHist_X = NULL;
  fMCHist_Y = NULL;
  fDataHist_X = NULL;
  fDataHist_Y = NULL;

  fMaskHist = NULL;
  fMapHist = NULL;
  fDataOrig = NULL;
  fDataTrue = NULL;
  fMCWeighted = NULL;
  
  fDefaultTypes = "FIX/FULL/CHI2";
  fAllowedTypes = "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/FITPROJX/FITPROJY";

  fIsFix   = false;
  fIsShape = false;
  fIsFree  = false;

  fIsDiag  = false;
  fIsFull  = false;

  fAddNormPen = false;
  fIsMask = false;
  fIsChi2SVD = false;

  fIsRawEvents = false;
  fIsDifXSec = false;
  fIsEnu = false;

};

//********************************************************************
Measurement2D:: ~Measurement2D() {
//********************************************************************
};

/*
  Setup Functions
*/
//********************************************************************
void Measurement2D::SetupMeasurement(std::string inputfile, std::string type, FitWeight *rw, std::string fkdt){
//********************************************************************

  // Check if name contains Evt, indicating that it is a raw number of events
  // measurements and should thus be treated as once
  fIsRawEvents = false;
  if ((fName.find("Evt") != std::string::npos) && fIsRawEvents == false) {
    fIsRawEvents = true;
    LOG(SAM) << "Found event rate measurement but fIsRawEvents == false!"
             << std::endl;
    LOG(SAM) << "Overriding this and setting fIsRawEvents == true!"
             << std::endl;
  }

  fIsEnu = false;
  if ((fName.find("XSec") != std::string::npos) && (fName.find("Enu") != std::string::npos)) {

    fIsEnu = true;
    LOG(SAM) << "::" << fName << "::" << std::endl;
    LOG(SAM) << "Found XSec Enu measurement, applying flux integrated scaling, "
                "not flux averaged!"
             << std::endl;
    if (FitPar::Config().GetParB("EventManager")){
      ERR(FTL) << "Enu Measurements do not yet work with the Event Manager!" <<std::endl;
      ERR(FTL) << "If you want decent flux unfolded results please run in series mode (-q EventManager=0)" << std::endl;
      sleep(2);
    }
  }

  if (fIsEnu && fIsRawEvents) {
    LOG(SAM) << "Found 1D Enu XSec distribution AND fIsRawEvents, is this "
                "really correct?!"
             << std::endl;
    LOG(SAM) << "Check experiment constructor for " << fName
             << " and correct this!" << std::endl;
    LOG(SAM) << "I live in " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  // Reset everything to NULL
  fRW = rw;

  // Setting up 2D Inputs
  this->SetupInputs(inputfile);
  
  // Set Default Options
  SetFitOptions( fDefaultTypes );

  // Set Passed Options
  SetFitOptions(type);

}

//********************************************************************
void Measurement2D::SetupDefaultHist(){
//********************************************************************

  // Setup fMCHist
  fMCHist = (TH2D*) fDataHist->Clone();
  fMCHist->SetNameTitle( (fName + "_MC").c_str(), (fName + "_MC" + fPlotTitles).c_str() );

  // Setup fMCFine
  Int_t nBinsX = fMCHist->GetNbinsX();
  Int_t nBinsY = fMCHist->GetNbinsY();
  fMCFine = new TH2D( (fName + "_MC_FINE").c_str(), (fName + "_MC_FINE" + fPlotTitles).c_str(),
           nBinsX*3, fMCHist->GetXaxis()->GetBinLowEdge(1), fMCHist->GetXaxis()->GetBinLowEdge(nBinsX+1),
			   nBinsY*3, fMCHist->GetYaxis()->GetBinLowEdge(1), fMCHist->GetYaxis()->GetBinLowEdge(nBinsY+1));

  // Setup the NEUT Mode Array
  PlotUtils::CreateNeutModeArray(fMCHist,(TH1**)fMCHist_PDG);

  // Setup bin masks using sample name
  if (fIsMask){
    std::string maskloc = FitPar::Config().GetParDIR( fName + ".mask");
    if (maskloc.empty()){
      maskloc = FitPar::GetDataBase() + "/masks/" + fName + ".mask";
    }

    SetBinMask(maskloc);
  }
  
  return;
}


//********************************************************************
void Measurement2D::SetFitOptions(std::string opt){
//********************************************************************

  // Do nothing if set DEFAULT
  if (opt == "DEFAULT") return;
  
  // CHECK Conflicting Fit Options
  std::vector<std::string> fit_option_allow = GeneralUtils::ParseToStr(fAllowedTypes, "/");
  for (UInt_t i = 0; i < fit_option_allow.size(); i++){
    std::vector<std::string> fit_option_section = GeneralUtils::ParseToStr(fit_option_allow.at(i), ",");
    bool found_option = false;

    for (UInt_t j = 0; j < fit_option_section.size(); j++){
      std::string av_opt = fit_option_section.at(j);

      if (!found_option and opt.find(av_opt) != std::string::npos) {
	found_option = true;

      } else if (found_option and  opt.find(av_opt) != std::string::npos){

	ERR(FTL) << "ERROR: Conflicting fit options provided: "<<opt<<std::endl;
	ERR(FTL) << "Conflicting group = "<<fit_option_section.at(i)<<std::endl;
	ERR(FTL) << "You should only supply one of these options in card file."<<std::endl;
	exit(-1);

      }
    }
  }

  // Check all options are allowed
  std::vector<std::string> fit_options_input = GeneralUtils::ParseToStr(opt,"/");
  for (UInt_t i = 0; i < fit_options_input.size(); i++){
    if (fAllowedTypes.find(fit_options_input.at(i)) == std::string::npos){

      ERR(FTL) <<"ERROR: Fit Option '"<<fit_options_input.at(i)<<"' Provided is not allowed for this measurement."<<std::endl;
      ERR(FTL) <<"Fit Options should be provided as a '/' seperated list (e.g. FREE/DIAG/NORM)" << std::endl;
      ERR(FTL) <<"Available options for "<<fName<<" are '"<< fAllowedTypes <<"'"<<std::endl;

      exit(-1);
    }
  }


  // Set TYPE
  fFitType = opt;

  // FIX,SHAPE,FREE
  if (opt.find("FIX") != std::string::npos){
    fIsFree = fIsShape = false;
    fIsFix  = true;
  } else if (opt.find("SHAPE") != std::string::npos){
    fIsFree = fIsFix = false;
    fIsShape = true;
  } else if (opt.find("FREE") != std::string::npos){
    fIsFix = fIsShape = false;
    fIsFree = true;
  }

  // DIAG,FULL (or default to full)
  if (opt.find("DIAG") != std::string::npos){
    fIsDiag = true;
    fIsFull = false;
  } else if (opt.find("FULL") != std::string::npos){
    fIsDiag = false;
    fIsFull = true;
  }

  // CHI2/LL (OTHERS?)
  if (opt.find("LOG") != std::string::npos) fIsChi2 = false;
  else fIsChi2 = true;

  // EXTRAS
  if (opt.find("RAW")   != std::string::npos) fIsRawEvents = true;
  if (opt.find("DIF")   != std::string::npos) fIsDifXSec   = true;
  if (opt.find("ENU1D") != std::string::npos) fIsEnu       = true;
  if (opt.find("NORM")  != std::string::npos) fAddNormPen  = true;
  if (opt.find("MASK")  != std::string::npos) fIsMask      = true;

  fIsProjFitX      = (opt.find("FITPROJX") != std::string::npos);
  fIsProjFitY      = (opt.find("FITPROJY") != std::string::npos);

  return;
};


//********************************************************************
void Measurement2D::SetDataValues(std::string dataFile, std::string TH2Dname) {
//********************************************************************

  if (dataFile.find(".root") == std::string::npos) {

    ERR(FTL) << "Error! " << dataFile << " is not a .root file" << std::endl;
    ERR(FTL) << "Currently only .root file reading is supported (MiniBooNE CC1pi+ 2D), but implementing .txt should be dirt easy" << std::endl;
    ERR(FTL) << "See me at " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);

  } else {

    TFile *inFile = new TFile(dataFile.c_str(), "READ");
    fDataHist = (TH2D*)(inFile->Get(TH2Dname.c_str())->Clone());
    fDataHist->SetDirectory(0);

    fDataHist->SetNameTitle((fName+"_data").c_str(), (fName+"_MC"+fPlotTitles).c_str());

    delete inFile;

  }

  return;
}


//********************************************************************
void Measurement2D::SetDataValues(std::string dataFile, double dataNorm, std::string errorFile, double errorNorm) {
//********************************************************************

  // Make a counter to track the line number
  int yBin = 0;

  std::string line;
  std::ifstream data(dataFile.c_str(),ifstream::in);

  fDataHist = new TH2D((fName+"_data").c_str(), (fName+fPlotTitles).c_str(), fNDataPointsX-1, fXBins, fNDataPointsY-1, fYBins);

  if(data.is_open()) LOG(SAM) << "Reading data from: " << dataFile.c_str() << std::endl;

  while(std::getline(data >> std::ws, line, '\n')){
    int xBin = 0;

    // Loop over entries and insert them into the histogram
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++){

      fDataHist->SetBinContent(xBin+1, yBin+1, (*iter)*dataNorm);
      xBin++;
    }
    yBin++;
  }

  yBin = 0;
  std::ifstream error(errorFile.c_str(),ifstream::in);

  if(error.is_open()) LOG(SAM) << "Reading errors from: " << errorFile.c_str() << std::endl;

  while(std::getline(error >> std::ws, line, '\n')){
    int xBin = 0;

    // Loop over entries and insert them into the histogram
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++){
      fDataHist->SetBinError(xBin+1, yBin+1, (*iter)*errorNorm);
      xBin++;
    }
    yBin++;
  }

  return;

};


//********************************************************************
void Measurement2D::SetDataValuesFromText(std::string dataFile, double dataNorm) {
//********************************************************************

  fDataHist = new TH2D((fName+"_data").c_str(), (fName+fPlotTitles).c_str(),
			    fNDataPointsX-1, fXBins, fNDataPointsY-1, fYBins);

  LOG(SAM) <<"Reading data from: "<<dataFile<<std::endl;
  PlotUtils::Set2DHistFromText(dataFile, fDataHist, dataNorm, true);

  return;
};


//********************************************************************
void Measurement2D::SetCovarMatrix(std::string covarFile){
//********************************************************************

  // Used to read a covariance matrix from a root file
  TFile* tempFile = new TFile(covarFile.c_str(),"READ");

  // Make plots that we want
  TH2D* covarPlot = new TH2D();
  //  TH2D* decmpPlot = new TH2D();
  TH2D* covarInvPlot = new TH2D();
  TH2D* fFullCovarPlot = new TH2D();

  // Get covariance options for fake data studies
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("throw_covariance");

  // Which matrix to get?
  if (fIsShape || fIsFree) covName = "shp_";
  if (fIsDiag) covName += "diag";
  else  covName += "full";

  covarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  covarInvPlot = (TH2D*) tempFile->Get((covName + "covinv").c_str());

  // Throw either the sub matrix or the full matrix
  if (!covOption.compare("SUB")) fFullCovarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  else if (!covOption.compare("FULL"))   fFullCovarPlot = (TH2D*) tempFile->Get("fullcov");
  else ERR(WRN)<<" Incorrect thrown_covariance option in parameters."<<std::endl;

  // Bin masking?
  int dim = int(fDataHist->GetNbinsX());//-this->masked->Integral());
  int covdim = int(fDataHist->GetNbinsX());

  // Make new covars
  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  fDecomp = new TMatrixDSym(dim);

  // Full covariance values
  int row,column = 0;
  row = 0;
  column = 0;
  for (Int_t i = 0; i < covdim; i++){

    // masking can be dodgy
    // if (this->masked->GetBinContent(i+1) > 0) continue;
    for (Int_t j = 0; j < covdim; j++){

      //   if (this->masked->GetBinContent(j+1) > 0) continue;
      (*this->covar)(row, column)  = covarPlot->GetBinContent(i+1,j+1);
      (*fFullCovar)(row, column) = fFullCovarPlot->GetBinContent(i+1,j+1);

      column++;
    }
    column = 0;
    row++;
  }

  // Set bin errors on data
  if (!fIsDiag){
    for (Int_t i = 0; i < fDataHist->GetNbinsX(); i++){
      fDataHist->SetBinError(i+1, sqrt((covarPlot->GetBinContent(i+1,i+1)))*1E-38);
    }
  }

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  tempFile->Close();
  delete tempFile;

  return;
};


//********************************************************************
void Measurement2D::SetCovarMatrixFromText(std::string covarFile, int dim){
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(),ifstream::in);

  this->covar = new TMatrixDSym(dim);
  fFullCovar = new TMatrixDSym(dim);
  if(covar.is_open()) LOG(SAM) << "Reading covariance matrix from file: " << covarFile << std::endl;

  while(std::getline(covar >> std::ws, line, '\n')){
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++){

      double val = (*iter)*fDataHist->GetBinError(row+1)*1E38*fDataHist->GetBinError(column+1)*1E38;
      (*this->covar)(row, column) = val;
      (*fFullCovar)(row, column) = val;

      column++;
    }

    row++;
  }

  // Robust matrix inversion method
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  return;
};


//********************************************************************
void Measurement2D::SetCovarMatrixFromChol(std::string covarFile, int dim){
  //********************************************************************

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covarread(covarFile.c_str(),ifstream::in);

  TMatrixD* newcov = new TMatrixD(dim,dim);

  if(covarread.is_open()) LOG(SAM) << "Reading covariance matrix from file: " << covarFile << std::endl;
  while(std::getline(covarread >> std::ws, line, '\n')){
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation matrix
    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++){

      (*newcov)(row, column) = *iter;
      column++;
    }

    row++;
  }
  covarread.close();

  // Form full covariance
  TMatrixD* trans = (TMatrixD*) (newcov)->Clone();
  trans->T();
  (*trans) *= (*newcov);

  fFullCovar = new TMatrixDSym(dim, trans->GetMatrixArray(), "");

  delete newcov;
  delete trans;

  // Robust matrix inversion method
  TDecompChol LU = TDecompChol(*this->fFullCovar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");
  
  return;
};

// virtual void SetMaskValuesFromText(std::string dataFile);

//********************************************************************
void Measurement2D::SetMapValuesFromText(std::string dataFile){
//********************************************************************

  fMapHist = new TH2I((fName+"_map").c_str(), (fName+fPlotTitles).c_str(),
			   fNDataPointsX-1, fXBins, fNDataPointsY-1, fYBins);

  LOG(SAM) <<"Reading map from: "<<dataFile<<std::endl;
  PlotUtils::Set2DHistFromText(dataFile, fMapHist, 1.0);

  return;
};


//********************************************************************
void Measurement2D::SetBinMask(std::string maskFile){
//********************************************************************

// Create a mask histogram.
  int nbinsX = fDataHist->GetNbinsX();
  int nbinsY = fDataHist->GetNbinsY();

  fMaskHist = new TH2I((fName+"_fMaskHist").c_str(),(fName+"_fMaskHist; Bin; Mask?")	\
			    .c_str(),nbinsX,0,nbinsX,nbinsY,0,nbinsY);
  //  int row ,column= 0;
  std::string line;
  std::ifstream mask(maskFile.c_str(),ifstream::in);

  if (mask.is_open()) LOG(SAM) <<"Reading bin mask from file: "<<maskFile <<std::endl;
  else ERR(WRN) <<" Cannot find mask file."<<std::endl;

  while(std::getline(mask >> std::ws, line, '\n')){
    
    std::vector<int> entries = GeneralUtils::ParseToInt(line, " ");

    // Skip lines with poorly formatted lines
    if (entries.size() < 3) {
      LOG(WRN) << "Measurement2D::SetBinMask(), couldn't parse line: " << line << std::endl;
      continue;
    }

    // The indices should be x, y, value
    fMaskHist->SetBinContent(entries[0], entries[1], entries[2]);
  }

  // Set masked data bins to zero
  PlotUtils::MaskBins(fDataHist, fMaskHist);

  return;
}






/*
    XSec Functions
*/
//********************************************************************
void Measurement2D::SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm){
//********************************************************************

  // Note this expects the flux bins to be given in terms of MeV
  // Used to read in the flux from a text file
  LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

  TGraph *f = new TGraph(fluxFile.c_str(),"%lg %lg");

  fFluxHist = new TH1D((fName+"_flux").c_str(), (fName+";E_{#nu} (GeV)").c_str(), f->GetN()-1, minE, maxE);

  // Get graph points
  Double_t *yVal = f->GetY();

  // Fill flux histogram from graph
  for (int i = 0; i<fFluxHist->GetNbinsX(); ++i)
    fFluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);

  delete f;

};

//********************************************************************
double Measurement2D::TotalIntegratedFlux(std::string intOpt, double low, double high){
//********************************************************************

  if(GetInput()->GetType() == kGiBUU){
    return 1.0;
  }

  // Return the integrated flux between two energy values
  // If non passed return it between the experimental flux
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  // Get bin integers
  int minBin = fFluxHist->GetXaxis()->FindBin(low);
  int maxBin = fFluxHist->GetXaxis()->FindBin(high);

  // Find integral
  double integral = fFluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral;

};



/*
  Reconfigure LOOP
*/

//********************************************************************
void Measurement2D::ResetAll(){
//********************************************************************

  // Simple function to reset the mc Histograms incase that is all that is needed.

  // Clear histograms
  fMCHist->Reset();
  fMCFine->Reset();

  return;
};


//********************************************************************
void Measurement2D::FillHistograms(){
//********************************************************************

  if (Signal){
    fMCHist->Fill(fXVar,fYVar,Weight);
    fMCFine->Fill(fXVar,fYVar,Weight);

    PlotUtils::FillNeutModeArray((TH2D**)fMCHist_PDG, Mode, fXVar, fYVar, Weight);
  }

  return;
}

//********************************************************************
void Measurement2D::ScaleEvents(){
//********************************************************************

  if (fMCWeighted) delete fMCWeighted;

  fMCWeighted = (TH2D*) fMCHist->Clone();
  fMCWeighted->SetNameTitle( (fName + "_MC_WGHTS").c_str(),
			     (fName + "_MC_WGHTS" + fPlotTitles).c_str() );
  fMCWeighted->GetYaxis()->SetTitle("Weighted Events");
  
  if (fIsEnu) { // If we have Enu we need to do flux integration bin by bin

    // This assumes we have the Enu on the x-axis
    // fairly trivial to make the change but only MiniBooNE 2D CC1pi+ has Enu in a 2D
    PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
    PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

    fMCHist->Scale(fScaleFactor);
    fMCFine->Scale(fScaleFactor);

  } else { // Else we just do normal scaling

    // Scale bin errors correctly
    fMCHist->GetSumw2();
    fMCFine->GetSumw2();

    // Final Scaling factors
    fMCHist->Scale(fScaleFactor, "width");
    fMCFine->Scale(fScaleFactor, "width");

    PlotUtils::ScaleNeutModeArray((TH1**)this->fMCHist_PDG, fScaleFactor, "width");
  }

  return;
};

//********************************************************************
void Measurement2D::ApplyNormScale(double norm){
  //********************************************************************

  fCurrentNorm = norm;

  double scale = 0.0;
  if (norm > 0.0) scale = 1.0/norm;

  fMCHist->Scale(scale);
  fMCFine->Scale(scale);

  PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, scale);

  return;

};


/*
  Statistic Functions - Outsources to StatUtils
*/
//********************************************************************
int Measurement2D::GetNDOF(){
//********************************************************************

  // Just incase it has gone...
  if (!fDataHist) return 0;

  int nDOF = 0;

  // If datahist has no errors make sure we don't include those bins as they are not data points
  for (int xBin = 0; xBin < fDataHist->GetNbinsX()+1; ++xBin){
    for (int yBin = 0; yBin < fDataHist->GetNbinsY()+1; ++yBin){
      if (fDataHist->GetBinContent(xBin, yBin) != 0 && fDataHist->GetBinError(xBin, yBin) != 0)
	++nDOF;
    }
  }

  // Account for possible bin masking
  int nMasked = 0;
  if (fMaskHist and fIsMask)
    if (fMaskHist->Integral()>0)
      for (int xBin = 0; xBin < fMaskHist->GetNbinsX()+1; ++xBin)
	for (int yBin = 0; yBin < fMaskHist->GetNbinsY()+1; ++yBin)
	  if (fMaskHist->GetBinContent(xBin, yBin) > 0.5) ++nMasked;

  // Take away those masked DOF
  nDOF -= nMasked;

  return nDOF;
}


//********************************************************************
double Measurement2D::GetLikelihood(){
//********************************************************************

  // Fix weird masking bug
  if (!fIsMask){
    if (fMaskHist){
      fMaskHist = NULL;
    }
  } else {
    if (fMaskHist){
      PlotUtils::MaskBins(fMCHist, fMaskHist);
    }
  }
  
  //  if (fIsProjFitX or fIsProjFitY) return GetProjectedChi2();

  // Scale up the results to match each other (Not using width might be inconsistent with Meas1D)
  double scaleF = fDataHist->Integral()/fMCHist->Integral();
  if (fIsShape){
    fMCHist->Scale(scaleF);
    fMCFine->Scale(scaleF);
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, scaleF);
  }

  if (!fMapHist){
    fMapHist = StatUtils::GenerateMap(fDataHist);
  }
  

  // Get the chi2 from either covar or diagonals
  double chi2;

  if (fIsChi2){
    if (fIsDiag) {
      chi2 = StatUtils::GetChi2FromDiag(fDataHist, fMCHist, fMapHist, fMaskHist);
    } else {
      chi2 = StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMapHist, fMaskHist);
    }
  } else {
    if (fIsDiag){
      chi2 = StatUtils::GetLikelihoodFromDiag(fDataHist, fMCHist, fMapHist, fMaskHist);
    } else {
      chi2 = StatUtils::GetLikelihoodFromCov(fDataHist, fMCHist, covar, fMapHist, fMaskHist);
    }
  }

  // Add a normal penalty term
  if (fAddNormPen){
    chi2 += (1- (fCurrentNorm))*(1-(fCurrentNorm))/(fNormError*fNormError);
    LOG(REC)<<"Norm penalty = "<<(1- (fCurrentNorm ))*(1-(fCurrentNorm))/(fNormError*fNormError)<<std::endl;
  }


  // Adjust the shape back to where it was. 
  if (fIsShape and !FitPar::Config().GetParB("saveshapescaling")){
    fMCHist->Scale(1./scaleF);
    fMCFine->Scale(1./scaleF);
    PlotUtils::ScaleNeutModeArray((TH1**)fMCHist_PDG, 1.0/scaleF);
  }

  LOG(REC)<<fName+" Chi2 = "<<chi2<<" \n";

  return chi2;
};

// //********************************************************************
// double Measurement2D::GetProjectedChi2(){
// //********************************************************************

//   PlotUtils::MatchEmptfYBins(fDataHist,fMCHist);

//   fMCHist_X = PlotUtils::GetProjectionX(fMCHist, fMaskHist);
//   fMCHist_Y = PlotUtils::GetProjectionY(fMCHist, fMaskHist);

//   fDataHist_X = PlotUtils::GetProjectionX(fDataHist, fMaskHist);
//   fDataHist_Y = PlotUtils::GetProjectionY(fDataHist, fMaskHist);

//   // Depending on the option either the rate of only X or only Y is used.

//   // If using Y rate, scale X to match data and vice versa
//   // Note: Projection will have already accounted for masking.
//   if (fIsProjFitY) fMCHist_X->Scale(PlotUtils::GetDataMCRatio(fDataHist_X, fMCHist_X));
//   if (fIsProjFitX) fMCHist_Y->Scale(PlotUtils::GetDataMCRatio(fDataHist_Y, fMCHist_Y));

//   // Now get individual chi2 from each
//   double chi2X = StatUtils::GetChi2FromDiag(fDataHist_X, fMCHist_X);
//   double chi2Y = StatUtils::GetChi2FromDiag(fDataHist_Y, fMCHist_Y);

//   double chi2 = chi2X  + chi2Y;

//   fMCHist_X->SetTitle(Form("%d", chi2X));
//   fMCHist_Y->SetTitle(Form("%d", chi2Y));

//   return chi2;
// }

/*
  Fake Data
*/
//********************************************************************
void Measurement2D::SetFakeDataValues(std::string fakeOption) {
//********************************************************************

  // This is the original data
  if (!(fDataOrig)) fDataOrig = (TH2D*)fDataHist->Clone((fName+"_data_original").c_str());
  TH2D *tempData = (TH2D*)fDataHist->Clone();

  TFile *fake = new TFile();

  if (fakeOption.compare("MC")==0){
    LOG(SAM) << fName <<"Setting fake data from MC "<<std::endl;
    fDataHist = (TH2D*)fMCHist->Clone((fName+"_MC").c_str());
    if (fMCHist->Integral() == 0.0) LOG(SAM) << fName <<"Invalid histogram"<<std::endl;
  }
  else {
    fake = new TFile(fakeOption.c_str());
    fDataHist = (TH2D*)fake->Get((fName+"_MC").c_str());
  }

  fDataHist ->SetNameTitle((fName+"_FAKE").c_str(), (fName+fPlotTitles).c_str());

  int nbins_x = fDataHist->GetNbinsX();
  int nbins_y = fDataHist->GetNbinsY();
  double alpha_i = 0.0;

  for (int i = 0; i < nbins_x; i++){
    for (int j = 0; j < nbins_y; j++){

      if (tempData->GetBinContent(i+1,j+1) == 0.0) continue;

      alpha_i =  fDataHist->GetBinContent(i+1,j+1)/fDataOrig->GetBinContent(i+1,j+1);
      fDataHist->SetBinError(i+1,j+1,alpha_i*fDataOrig->GetBinError(i+1,j+1));

    }
  }
  fDataTrue = (TH2D*) fDataHist->Clone();

  fake->Close();
  delete fake;

  return;
};

// virtual void ResetFakeData();
// virtual void ResetData();

//********************************************************************
void Measurement2D::ThrowCovariance(){
//********************************************************************

  return;
};


/*
    Access Functions
*/
//********************************************************************
std::vector<TH1*> Measurement2D::GetMCList(){
//********************************************************************

  // If this isn't a NULL pointer, make the plot pretty!
  if (!fMCHist) return std::vector<TH1*> (1, fMCHist);

  std::ostringstream chi2;
  chi2 << std::setprecision(5) << this->GetLikelihood();

  int plotcolor = kRed;
  if (FitPar::Config().GetParI("linecolour") > 0){
    plotcolor = FitPar::Config().GetParI("linecolour");
  }

  int plotstyle = 1;
  if (FitPar::Config().GetParI("linestyle") > 0){
    plotstyle = FitPar::Config().GetParI("linestyle");
  }

  int plotfillstyle=0;
  if (FitPar::Config().GetParI("fillstyle") > 0){
    plotfillstyle = FitPar::Config().GetParI("fillstyle");
  }

  LOG(SAM) << fName << " chi2 = " << GetLikelihood() << std::endl;

  fMCHist->SetTitle(chi2.str().c_str());
  fMCHist->SetLineWidth(2);
  fMCHist->SetLineColor(plotcolor);
  fMCHist->SetFillColor(plotcolor);
  fMCHist->SetLineStyle(plotstyle);
  fMCHist->SetFillStyle(plotfillstyle);

  return std::vector<TH1*> (1, fMCHist);
};

//********************************************************************
std::vector<TH1*> Measurement2D::GetDataList(){
//********************************************************************

  // If this isn't a NULL pointer, make the plot pretty!

  if (!fDataHist) return std::vector<TH1*> (1, fDataHist);

  fDataHist->SetLineWidth(2);
  fDataHist->SetLineColor(kBlue);

  return std::vector<TH1*> (1, fDataHist);
};


//********************************************************************
void Measurement2D:: GetBinContents(std::vector<double>& cont, std::vector<double>& err){
  //********************************************************************

  int count = 0;
  for (int i = 0; i < (fMCHist->GetNbinsX()+2) * (fMCHist->GetNbinsY()+2); i++){
    cont.push_back(fMCHist->GetBinContent(i));
    err.push_back(fMCHist->GetBinError(i));
    count++;
  }
  return;
};


//********************************************************************
std::vector<double> Measurement2D::GetXSec(std::string option){
//********************************************************************

  std::vector<double> vals;
  vals.push_back(0.0);
  vals.push_back(0.0);

  bool getMC = !option.compare("MC");
  bool getDT = !option.compare("DATA");

  for (int i = 0; i < fMCHist->GetNbinsX(); i++){
    if (fDataHist->GetBinContent(i+1) == 0.0 and fDataHist->GetBinError(i+1) == 0.0) continue;

    if (getMC){

      vals[0] += fMCHist->GetBinContent(i+1) * fMCHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += fMCHist->GetBinError(i+1) * fMCHist->GetBinError(i+1) * fMCHist->GetXaxis()->GetBinWidth(i+1) * fMCHist->GetXaxis()->GetBinWidth(i+1);

    } else if (getDT){

      vals[0] += fDataHist->GetBinContent(i+1) * fDataHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += fDataHist->GetBinError(i+1) * fDataHist->GetBinError(i+1) * fDataHist->GetXaxis()->GetBinWidth(i+1) * fDataHist->GetXaxis()->GetBinWidth(i+1);

    }
  }

  // If not diag Get the total error from the covariance
  if (!fIsDiag and getDT){
    vals[1] = 0.0;
    for (int i = 0; i < fDataHist->GetNbinsX(); i++){
      for(int j = 0; j < fDataHist->GetNbinsX(); j++){

	vals[1] += (*fFullCovar)(i,j);

      }
    }
    vals[1] = sqrt(vals[1]) * 1E-38;
  }

  return vals;
}




/*
  Write Functions
*/

//********************************************************************
void Measurement2D::Write(std::string drawOpt){
//********************************************************************

  // If null pointer return
  if (!fMCHist and !fDataHist){
    LOG(SAM) << fName <<"Incomplete histogram set!"<<std::endl;
    return;
  }

  //  FitPar::Config().out->cd();

  // Get Draw Options
  drawOpt = FitPar::Config().GetParS("drawopts");
  bool drawData   = (drawOpt.find("DATA") != std::string::npos);
  bool drawNormal = (drawOpt.find("MC") != std::string::npos);
  bool drawEvents = (drawOpt.find("EVT") != std::string::npos);
  bool drawXSec   = (drawOpt.find("XSEC") != std::string::npos);
  bool drawFine   = (drawOpt.find("FINE") != std::string::npos);
  bool drawRatio  = (drawOpt.find("RATIO") != std::string::npos);
  bool drawModes  = (drawOpt.find("MODES") != std::string::npos);
  bool drawShape  = (drawOpt.find("SHAPE") != std::string::npos);
  bool residual   = (drawOpt.find("RESIDUAL") != std::string::npos);
  bool drawMatrix = (drawOpt.find("MATRIX") != std::string::npos);
  bool drawFlux   = (drawOpt.find("FLUX") != std::string::npos);
  bool drawMask   = (drawOpt.find("MASK") != std::string::npos);
  bool drawMap    = (drawOpt.find("MAP")  != std::string::npos);
  bool drawProj   = (drawOpt.find("PROJ") != std::string::npos);
  //bool drawCanvPDG = (drawOpt.find("CANVPDG") != std::string::npos);
  bool drawCov    = (drawOpt.find("COV") != std::string::npos);
  bool drawSliceCanvYMC = (drawOpt.find("CANVYMC") != std::string::npos);
  bool drawWeighted = (drawOpt.find("WGHT") != std::string::npos);
  
  if (FitPar::Config().GetParB("EventManager")){
    drawFlux = false;
    drawXSec = false;
    drawEvents = false;
  }

  // Save standard plots
  if (drawData)    this->GetDataList().at(0)->Write();
  if (drawNormal)  this->GetMCList()  .at(0)->Write();

  if (drawCov){
    TH2D(*fFullCovar).Write( (fName + "_COV").c_str() );
  }
  
  if (drawOpt.find("INVCOV") != std::string::npos){
    TH2D(*covar).Write( (fName + "_INVCOV").c_str() );
  }
  
  // Generate a simple map
  if (!fMapHist)
    fMapHist = StatUtils::GenerateMap(fDataHist);

  // Convert to 1D Lists
  TH1D* data_1D = StatUtils::MapToTH1D(fDataHist, fMapHist);
  TH1D* mc_1D   = StatUtils::MapToTH1D(fMCHist,   fMapHist);
  TH1I* mask_1D = StatUtils::MapToMask(fMaskHist, fMapHist);

  data_1D->Write();
  mc_1D->Write();

  if (mask_1D){
    mask_1D->Write();

    TMatrixDSym* calc_cov  = StatUtils::ApplyInvertedMatrixMasking(covar, mask_1D);
    TH1D* calc_data = StatUtils::ApplyHistogramMasking(data_1D, mask_1D);
    TH1D* calc_mc   = StatUtils::ApplyHistogramMasking(mc_1D, mask_1D);

    TH2D* bin_cov = new TH2D(*calc_cov);

    bin_cov->Write();
    calc_data->Write();
    calc_mc->Write();

    delete mask_1D;
    delete calc_cov;
    delete calc_data;
    delete calc_mc;
    delete bin_cov;

  }

  delete data_1D;
  delete mc_1D;


   // Save only mc and data if splines
  if(fEventType == 4 or fEventType==3){ return; }

  // Draw Extra plots
  if (drawFine)    this->GetFineList().at(0)->Write();
  if (drawFlux)    fFluxHist->Write();
  if (drawEvents)  fEventHist->Write();
  if (fIsMask and drawMask) fMaskHist->Write( (fName + "_MSK").c_str() ); //< save mask
  if (drawMap) fMapHist->Write( (fName + "_MAP").c_str() ); //< save map

  // Save neut stack
  if (drawModes){
    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack((fName + "_MC_PDG").c_str(), (TH1**)fMCHist_PDG, 0);
    combo_fMCHist_PDG.Write();
  }

  // Save Matrix plots
  if (drawMatrix and fFullCovar and covar and fDecomp){

    TH2D cov = TH2D((*fFullCovar));
    cov.SetNameTitle((fName+"_cov").c_str(),(fName+"_cov;Bins; Bins;").c_str());
    cov.Write();

    TH2D covinv = TH2D((*this->covar));
    covinv.SetNameTitle((fName+"_covinv").c_str(),(fName+"_cov;Bins; Bins;").c_str());
    covinv.Write();

    TH2D covdec = TH2D((*fDecomp));
    covdec.SetNameTitle((fName+"_covdec").c_str(),(fName+"_cov;Bins; Bins;").c_str());
    covdec.Write();

  }


  // Save ratio plots if required
  if (drawRatio){

    // Needed for error bars
    for(int i = 0; i < fMCHist->GetNbinsX()*fMCHist->GetNbinsY(); i++)
      fMCHist->SetBinError(i+1,0.0);

    fDataHist->GetSumw2();
    fMCHist->GetSumw2();

    // Create Ratio Histograms
    TH2D* dataRatio = (TH2D*) fDataHist->Clone((fName + "_data_RATIO").c_str());
    TH2D* mcRatio   = (TH2D*) fMCHist->Clone((fName + "_MC_RATIO").c_str());

    mcRatio->Divide(fMCHist);
    dataRatio->Divide(fMCHist);

    // Cancel bin errors on MC
    for(int i = 0; i < mcRatio->GetNbinsX()*mcRatio->GetNbinsY(); i++) {
      mcRatio->SetBinError(i+1,fMCHist->GetBinError(i+1) / fMCHist->GetBinContent(i+1));
    }

    mcRatio->SetMinimum(0);
    mcRatio->SetMaximum(2);
    dataRatio->SetMinimum(0);
    dataRatio->SetMaximum(2);

    mcRatio->Write();
    dataRatio->Write();

    delete mcRatio;
    delete dataRatio;
  }

  // Save Shape Plots if required
  if (drawShape){

    // Create Shape Histogram
    TH2D* mcShape = (TH2D*) fMCHist->Clone((fName + "_MC_SHAPE").c_str());

    double shapeScale = 1.0;
    if (fIsRawEvents){
      shapeScale = fDataHist->Integral() / fMCHist->Integral();
    } else {
      shapeScale =  fDataHist->Integral("width") / fMCHist->Integral("width");
    }
    
    mcShape->Scale( shapeScale );

    mcShape->SetLineWidth(3);
    mcShape->SetLineStyle(7); //dashes

    mcShape->Write();

    // Save shape ratios
    if (drawRatio){

      // Needed for error bars
      mcShape->GetSumw2();

      // Create shape ratio histograms
      TH2D* mcShapeRatio   = (TH2D*)mcShape->Clone((fName + "_MC_SHAPE_RATIO").c_str());
      TH2D* dataShapeRatio = (TH2D*)fDataHist->Clone((fName + "_data_SHAPE_RATIO").c_str());

      // Divide the histograms
      mcShapeRatio   ->Divide(mcShape);
      dataShapeRatio ->Divide(mcShape);

      // Colour the shape ratio plots
      mcShapeRatio ->SetLineWidth(3);
      mcShapeRatio ->SetLineStyle(7); // dashes

      mcShapeRatio  ->Write();
      dataShapeRatio->Write();

      delete mcShapeRatio;
      delete dataShapeRatio;
    }

    delete mcShape;
  }

  // Save residual calculations of what contributed to the chi2 values.
  if (residual){
  }

  if (fIsProjFitX or fIsProjFitY or drawProj){

    // If not already made, make the projections
    if (!fMCHist_X){
      PlotUtils::MatchEmptyBins(fDataHist,fMCHist);

      fMCHist_X = PlotUtils::GetProjectionX(fMCHist, fMaskHist);
      fMCHist_Y = PlotUtils::GetProjectionY(fMCHist, fMaskHist);

      fDataHist_X = PlotUtils::GetProjectionX(fDataHist, fMaskHist);
      fDataHist_Y = PlotUtils::GetProjectionY(fDataHist, fMaskHist);

      double chi2X = StatUtils::GetChi2FromDiag(fDataHist_X, fMCHist_X);
      double chi2Y = StatUtils::GetChi2FromDiag(fDataHist_Y, fMCHist_Y);

      fMCHist_X->SetTitle(Form("%f", chi2X));
      fMCHist_Y->SetTitle(Form("%f", chi2Y));
    }

    // Save the histograms
    fDataHist_X->Write();
    fMCHist_X->Write();

    fDataHist_Y->Write();
    fMCHist_Y->Write();
  }

  if (drawSliceCanvYMC or true){
    TCanvas* c1 = new TCanvas((fName + "_MC_CANV_Y").c_str(),
			      (fName + "_MC_CANV_Y").c_str(),
			      800,600);

    c1->Divide( int(sqrt(fDataHist->GetNbinsY()+1)), int(sqrt(fDataHist->GetNbinsY()+1)) );
    TH2D* mcShape = (TH2D*) fMCHist->Clone((fName + "_MC_SHAPE").c_str());
    double shapeScale = fDataHist->Integral("width")/fMCHist->Integral("width");
    mcShape->Scale(shapeScale);
    mcShape->SetLineStyle(7);

    c1->cd(1);
    TLegend* leg = new TLegend(0.6,0.6,0.9,0.9);
    leg->AddEntry(fDataHist, (fName + " Data").c_str(), "ep");
    leg->AddEntry(fMCHist,   (fName + " MC").c_str(), "l");
    leg->AddEntry(mcShape,  (fName + " Shape").c_str(), "l");
    leg->Draw("SAME");

    /*
    // Make Y slices
    for (int i = 0; i < fDataHist->GetNbinY(); i++){
      
      c1->cd(i+2);
      TH1D* fDataHist_SliceY = PlotUtils::GetSliceY(fDataHist, i);
      fDataHist_SliceY->Draw("E1");

      TH1D* fMCHist_SliceY = PlotUtils::GetSliceY(fMCHist, i);
      fMCHist_SliceY->Draw("SAME HIST C");

      TH1D* mcShape_SliceY = PlotUtils::GetSliceY(mcShape, i);
      mcShape_SliceY->Draw("SAME HIST C");
    }
    */
    c1->Write();
  }
  
  if (drawWeighted){
    fMCWeighted->Write();
  }
  
  // Returning
  LOG(SAM) << "Written Histograms: "<<fName<<std::endl;
  return;
};


THStack Measurement2D::GetModeStack(){
  THStack combo_hist = PlotUtils::GetNeutModeStack((fName + "_MC_PDG").c_str(), (TH1**)fMCHist_PDG, 0);
  return combo_hist;
}
