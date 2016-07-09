// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "Measurement2D.h"
#include "TDecompChol.h"

//********************************************************************
Measurement2D::Measurement2D() {
//********************************************************************

    covar = NULL;
    decomp = NULL;
    fullcovar = NULL;

    mcHist = NULL;
    mcFine = NULL;
    dataHist = NULL;

    mcHist_X = NULL;
    mcHist_Y = NULL;
    dataHist_X = NULL;
    dataHist_Y = NULL;
    
    maskHist = NULL;
    mapHist = NULL;
    dataOrig = NULL;
    dataTrue = NULL;

    default_types = "FIX/FULL/CHI2";
    allowed_types = "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/FITPROJX/FITPROJY";

    isFix   = false;
    isShape = false;
    isFree  = false;

    isDiag  = false;
    isFull  = false;

    addNormPenalty = false;
    isMask = false;
    isChi2SVD = false;

    isRawEvents = false;
    isDifXSec = false;
    isEnu1D = false;
    
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

  // Reset everything to NULL                                                                                                                                                   
  rw_engine = rw;

  this->input     = new InputHandler(this->measurementName, inputfile);
  this->fluxHist  = input->GetFluxHistogram();
  this->eventHist = input->GetEventHistogram();
  this->xsecHist  = input->GetXSecHistogram();
  this->nevents   = input->GetNEvents();

  // Set Default Options                                                                                                                                                                     
  SetFitOptions( this->default_types );

  // Set Passed Options                                                                                                                                                                      
  SetFitOptions(type);

}

//********************************************************************  
void Measurement2D::SetupDefaultHist(){
//********************************************************************  

  // Setup mcHist          
  this->mcHist = (TH2D*) this->dataHist->Clone();
  this->mcHist->SetNameTitle( (this->measurementName + "_MC").c_str(), (this->measurementName + "_MC" + this->plotTitles).c_str() );

  // Setup mcFine          
  Int_t nBinsX = this->mcHist->GetNbinsX();
  Int_t nBinsY = this->mcHist->GetNbinsY();
  this->mcFine = new TH2D( (this->measurementName + "_MC_FINE").c_str(), (this->measurementName + "_MC_FINE" + this->plotTitles).c_str(),
           nBinsX*3, this->mcHist->GetXaxis()->GetBinLowEdge(1), this->mcHist->GetXaxis()->GetBinLowEdge(nBinsX+1), 
			   nBinsY*3, this->mcHist->GetYaxis()->GetBinLowEdge(1), this->mcHist->GetYaxis()->GetBinLowEdge(nBinsY+1));

  // Setup the NEUT Mode Array             
  PlotUtils::CreateNeutModeArray(this->mcHist,(TH1**)this->mcHist_PDG);

  return;
}


//********************************************************************
void Measurement2D::SetFitOptions(std::string opt){
//********************************************************************

  // CHECK Conflicting Fit Options
  std::vector<std::string> fit_option_allow = PlotUtils::FillVectorSFromString(allowed_types, "/");
  for (int i = 0; i < fit_option_allow.size(); i++){
    std::vector<std::string> fit_option_section = PlotUtils::FillVectorSFromString(fit_option_allow.at(i), ",");
    bool found_option = false;

    for (int j = 0; j < fit_option_section.size(); j++){
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
  std::vector<std::string> fit_options_input = PlotUtils::FillVectorSFromString(opt,"/");
  for (int i = 0; i < fit_options_input.size(); i++){
    if (allowed_types.find(fit_options_input.at(i)) == std::string::npos){

      ERR(FTL) <<"ERROR: Fit Option '"<<fit_options_input.at(i)<<"' Provided is not allowed for this measurement."<<std::endl;
      ERR(FTL) <<"Fit Options should be provided as a '/' seperated list (e.g. FREE/DIAG/NORM)" << std::endl;
      ERR(FTL) <<"Available options for "<<measurementName<<" are '"<< allowed_types <<"'"<<std::endl;

      exit(-1);
    }
  }
  

  // Set TYPE
  this->fitType = opt;

  // FIX,SHAPE,FREE
  if (opt.find("FIX") != std::string::npos){
    isFree = isShape = false;
    isFix  = true;
  } else if (opt.find("SHAPE") != std::string::npos){
    isFree = isFix = false;
    isShape = true;
  } else if (opt.find("FREE") != std::string::npos){
    isFix = isShape = false;
    isFree = true;
  }

  // DIAG,FULL (or default to full)
  if (opt.find("DIAG") != std::string::npos){
    isDiag = true;
    isFull = false;
  } else if (opt.find("FULL") != std::string::npos){
    isDiag = false;
    isFull = true;
  }

  // CHI2/LL (OTHERS?)
  if (opt.find("LOG") != std::string::npos) isChi2 = false;
  else isChi2 = true;

  // EXTRAS
  if (opt.find("RAW")   != std::string::npos) isRawEvents = true;
  if (opt.find("DIF")   != std::string::npos) isDifXSec   = true;
  if (opt.find("ENU1D") != std::string::npos) isEnu1D     = true;
  if (opt.find("NORM")  != std::string::npos) addNormPenalty = true;
  if (opt.find("MASK")  != std::string::npos) isMask      = true;

  this->isProjFitX      = (opt.find("FITPROJX") != std::string::npos);
  this->isProjFitY      = (opt.find("FITPROJY") != std::string::npos);
  
  return;
};


//******************************************************************** 
void Measurement2D::SetDataValues(std::string dataFile, std::string TH2Dname) {
//********************************************************************
  
  if (dataFile.find(".root") == std::string::npos) {

    std::cerr << "Error! " << dataFile << " is not a .root file" << std::endl;
    std::cerr << "Currently only .root file reading is supported (MiniBooNE CC1pi+ 2D), but implementing .txt should be dirt easy" << std::endl;
    std::cerr << "See me at " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);

  } else {
    
    TFile *inFile = new TFile(dataFile.c_str(), "READ");
    dataHist = (TH2D*)(inFile->Get(TH2Dname.c_str())->Clone());
    dataHist->SetDirectory(0);

    dataHist->SetNameTitle((measurementName+"_data").c_str(), (measurementName+"_MC"+plotTitles).c_str());

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

  this->dataHist = new TH2D((this->measurementName+"_data").c_str(), (this->measurementName+this->plotTitles).c_str(), this->data_points_x-1, this->xBins, this->data_points_y-1, this->yBins);

  if(data.is_open()) LOG(SAM) << "Reading data from: " << dataFile.c_str() << std::endl;

  while(std::getline(data, line, '\n')){
    std::istringstream stream(line);
    double entry;
    int xBin = 0;

    // Loop over entries and insert them into the histogram   
    while(stream >> entry){
      this->dataHist->SetBinContent(xBin+1, yBin+1, entry*dataNorm);
      xBin++;
    }
    yBin++;
  }

  yBin = 0;
  std::ifstream error(errorFile.c_str(),ifstream::in);

  if(error.is_open()) LOG(SAM) << "Reading errors from: " << errorFile.c_str() << std::endl;

  while(std::getline(error, line, '\n')){
    std::istringstream stream(line);
    double entry;
    int xBin = 0;

    // Loop over entries and insert them into the histogram   
    while(stream >> entry){
      this->dataHist->SetBinError(xBin+1, yBin+1, entry*errorNorm);
      xBin++;
    }
    yBin++;
  }

  return;

};


//******************************************************************** 
void Measurement2D::SetDataValuesFromText(std::string dataFile, double dataNorm) {
//********************************************************************

  this->dataHist = new TH2D((this->measurementName+"_data").c_str(), (this->measurementName+this->plotTitles).c_str(),
			    this->data_points_x-1, this->xBins, this->data_points_y-1, this->yBins);

  LOG(SAM) <<"Reading data from: "<<dataFile<<std::endl;
  PlotUtils::Set2DHistFromText(dataFile, this->dataHist, dataNorm, true);
  
  return;
};


//********************************************************************
void Measurement2D::SetCovarMatrix(std::string covarFile){
//********************************************************************

  // Used to read a covariance matrix from a root file
  TFile* tempFile = new TFile(covarFile.c_str(),"READ");

  // Make plots that we want
  TH2D* covarPlot = new TH2D();
  TH2D* decmpPlot = new TH2D();
  TH2D* covarInvPlot = new TH2D();
  TH2D* fullcovarPlot = new TH2D();
  
  // Get covariance options for fake data studies
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("throw_covariance");

  // Which matrix to get?
  if (this->isShape || this->isFree) covName = "shp_";
  if (this->isDiag) covName += "diag";
  else  covName += "full";

  covarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  covarInvPlot = (TH2D*) tempFile->Get((covName + "covinv").c_str());
  
  // Throw either the sub matrix or the full matrix
  if (!covOption.compare("SUB")) fullcovarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());      
  else if (!covOption.compare("FULL"))   fullcovarPlot = (TH2D*) tempFile->Get("fullcov");
  else ERR(WRN)<<" Incorrect thrown_covariance option in parameters."<<std::endl;

  // Bin masking?
  int dim = int(this->dataHist->GetNbinsX());//-this->masked->Integral()); 
  int covdim = int(this->dataHist->GetNbinsX());

  // Make new covars
  this->covar = new TMatrixDSym(dim);
  this->fullcovar = new TMatrixDSym(dim);
  this->decomp = new TMatrixDSym(dim);

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
      (*this->fullcovar)(row, column) = fullcovarPlot->GetBinContent(i+1,j+1);

      column++;
    }
    column = 0;
    row++;
  }

  // Set bin errors on data 
  if (!this->isDiag){
    for (Int_t i = 0; i < this->dataHist->GetNbinsX(); i++){
      this->dataHist->SetBinError(i+1, sqrt((covarPlot->GetBinContent(i+1,i+1)))*1E-38);              
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
  this->fullcovar = new TMatrixDSym(dim);
  if(covar.is_open()) LOG(SAM) << "Reading covariance matrix from file: " << covarFile << std::endl;

  while(std::getline(covar, line, '\n')){
    std::istringstream stream(line);
    double entry;
    int column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation matrix
    while(stream >> entry){

      double val = entry*this->dataHist->GetBinError(row+1)*1E38*this->dataHist->GetBinError(column+1)*1E38;
      (*this->covar)(row, column) = val;
      (*this->fullcovar)(row, column) = val;

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
  int column  = 0;
  while(std::getline(covarread, line, '\n')){
    std::istringstream stream(line);
    double entry;
    column = 0;

    // Loop over entries and insert them into matrix
    // Multiply by the errors to get the covariance, rather than the correlation matrix
    while(stream >> entry){
      
      double val = entry;
      (*newcov)(row, column) = val;
      
      column++;
    }

    row++;
  }
  covarread.close();

  // Form full covariance
  TMatrixD* trans = (TMatrixD*) (newcov)->Clone();
  trans->T();

  (*trans) *= (*newcov);
  newcov = (TMatrixD*) trans->Clone();
  
  this->covar = new TMatrixDSym(dim, newcov->GetMatrixArray(), "");
  this->fullcovar = new TMatrixDSym(dim, newcov->GetMatrixArray(), "");

  delete newcov;
  delete trans;
  
  // Robust matrix inversion method
  TDecompChol LU = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  return;
};

// virtual void SetMaskValuesFromText(std::string dataFile);

//******************************************************************** 
void Measurement2D::SetMapValuesFromText(std::string dataFile){
//********************************************************************     

  this->mapHist = new TH2I((this->measurementName+"_map").c_str(), (this->measurementName+this->plotTitles).c_str(),
			   this->data_points_x-1, this->xBins, this->data_points_y-1, this->yBins);

  LOG(SAM) <<"Reading map from: "<<dataFile<<std::endl;
  PlotUtils::Set2DHistFromText(dataFile, this->mapHist, 1.0);

  return;
};


//********************************************************************                                                 
void Measurement2D::SetBinMask(std::string maskFile){
//********************************************************************

// Create a mask histogram.                                                                                           
  int nbinsX = this->dataHist->GetNbinsX();
  int nbinsY = this->dataHist->GetNbinsY();
  
  this->maskHist = new TH2I((this->measurementName+"_maskHist").c_str(),(this->measurementName+"_maskHist; Bin; Mask?")	\
			    .c_str(),nbinsX,0,nbinsX,nbinsY,0,nbinsY);
  int row ,column= 0;
  std::string line;
  std::ifstream mask(maskFile.c_str(),ifstream::in);
  
  if (mask.is_open()) LOG(SAM) <<"Reading bin mask from file: "<<maskFile <<std::endl;
  else std::cerr <<" Cannot find mask file."<<std::endl;
  
  while(std::getline(mask, line, '\n')){
    std::istringstream stream(line);
    int column = 0;
    double entry;
    int binx,biny,value;
    while (stream >> entry){
      if (column == 0) binx = int(entry);
      if (column == 1) biny = int(entry);
      if (column == 2) value = int(entry);
      if (column > 2) break;
      column++;
    }
  this->maskHist->SetBinContent(binx,biny,value);
  }
  
  // Set masked data bins to zero                                                                                      
  PlotUtils::MaskBins(this->dataHist, this->maskHist);
  
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

  this->fluxHist = new TH1D((this->measurementName+"_flux").c_str(), (this->measurementName+";E_{#nu} (GeV)").c_str(), f->GetN()-1, minE, maxE);

  // Get graph points
  Double_t *yVal = f->GetY();

  // Fill flux histogram from graph
  for (int i = 0; i<fluxHist->GetNbinsX(); ++i)
    this->fluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);

  delete f;
  
};

//********************************************************************  
double Measurement2D::TotalIntegratedFlux(std::string intOpt, double low, double high){
//********************************************************************  

  // Return the integrated flux between two energy values
  // If non passed return it between the experimental flux
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  // Get bin integers
  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  // Find integral
  double integral = this->fluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

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
  this->mcHist->Reset();
  this->mcFine->Reset();
  
  return;
};


//********************************************************************
void Measurement2D::FillHistograms(){
//********************************************************************

  if (Signal){
    this->mcHist->Fill(X_VAR,Y_VAR,Weight);
    this->mcFine->Fill(X_VAR,Y_VAR,Weight);
    
    PlotUtils::FillNeutModeArray((TH2D**)this->mcHist_PDG, Mode, X_VAR, Y_VAR, Weight);
  }

  return;
}

//********************************************************************
void Measurement2D::ScaleEvents(){
//********************************************************************

  if (isEnu) { // If we have Enu we need to do flux integration bin by bin

    int axis = 0;
    // If name is 2DEnu, means Enu is the x-axis
    // Convention is 2DXVARYVAR
    if (measurementName.find("2DEnu") != std::string::npos) {
      axis = 0;
    } else {
      axis = 1;
    }

    PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist, axis);
    PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist, axis);

    mcHist->Scale(scaleFactor);
    mcFine->Scale(scaleFactor);

  } else { // Else we just do normal scaling

    // Scale bin errors correctly
    this->mcHist->GetSumw2();
    this->mcFine->GetSumw2();

    // Final Scaling factors       
    this->mcHist->Scale(this->scaleFactor, "width");
    this->mcFine->Scale(this->scaleFactor, "width");
    
    PlotUtils::ScaleNeutModeArray((TH1**)this->mcHist_PDG, this->scaleFactor, "width");
  }
 
  return;
};

//********************************************************************
void Measurement2D::ApplyNormScale(double norm){
  //********************************************************************

  this->currentNorm = norm;

  double scale = 0.0;
  if (norm > 0.0) scale = 1.0/norm;

  this->mcHist->Scale(scale);
  this->mcFine->Scale(scale);

  return;

};


/*
  Statistic Functions - Outsources to StatUtils
*/
//********************************************************************   
int Measurement2D::GetNDOF(){
//********************************************************************   

  // Just incase it has gone...
  if (!this->dataHist) return 0;
  
  int nDOF = 0;

  // If datahist has no errors make sure we don't include those bins as they are not data points
  for (int xBin = 0; xBin < this->dataHist->GetNbinsX()+1; ++xBin){
    for (int yBin = 0; yBin < this->dataHist->GetNbinsY()+1; ++yBin){
      if (this->dataHist->GetBinContent(xBin, yBin) != 0 && this->dataHist->GetBinError(xBin, yBin) != 0)
	++nDOF;
    }
  }

  // Account for possible bin masking
  int nMasked = 0;
  if (this->maskHist and this->isMask)
    if (this->maskHist->Integral()>0)
      for (int xBin = 0; xBin < this->maskHist->GetNbinsX()+1; ++xBin)
	for (int yBin = 0; yBin < this->maskHist->GetNbinsY()+1; ++yBin)
	  if (this->maskHist->GetBinContent(xBin, yBin) > 0.5) ++nMasked;
  
  // Take away those masked DOF
  nDOF -= nMasked;

  return nDOF;
}


//********************************************************************    
double Measurement2D::GetLikelihood(){
//********************************************************************    

//  if (isProjFitX or isProjFitY) return GetProjectedChi2();

  // Scale up the results to match each other (Not using width might be inconsistent with Meas1D)
  double scaleF = this->dataHist->Integral()/this->mcHist->Integral();
  if (this->isShape){
    this->mcHist->Scale(scaleF);
    this->mcFine->Scale(scaleF);
  }

  if (!mapHist){
    mapHist = StatUtils::GenerateMap(dataHist);
  }

  // Get the chi2 from either covar or diagonals
  double chi2;

  if (isChi2){
    if (this->isDiag) {
      chi2 = StatUtils::GetChi2FromDiag(dataHist, mcHist, mapHist, maskHist);
    } else {
      chi2 = StatUtils::GetChi2FromCov(dataHist, mcHist, covar, mapHist, maskHist);
    }
  } else {
    if (this->isDiag){
      chi2 = StatUtils::GetLikelihoodFromDiag(dataHist, mcHist, mapHist, maskHist);
    } else {
      chi2 = StatUtils::GetLikelihoodFromCov(dataHist, mcHist, covar, mapHist, maskHist);
    }
  }

  // Add a normal penalty term   
  if (this->addNormPenalty){
    chi2 += (1- (this->currentNorm))*(1-(this->currentNorm))/(this->normError*this->normError);
    LOG(REC)<<"Norm penalty = "<<(1- (this->currentNorm ))*(1-(this->currentNorm))/(this->normError*this->normError)<<std::endl;
  }


  // Adjust the shape back to where it was.
  if (this->isShape){
    this->mcHist->Scale(1./scaleF);
    this->mcFine->Scale(1./scaleF);
  }

  LOG(REC)<<this->measurementName+" Chi2 = "<<chi2<<" \n";

  return chi2;
};

// //********************************************************************   
// double Measurement2D::GetProjectedChi2(){
// //********************************************************************

//   PlotUtils::MatchEmptyBins(dataHist,mcHist);

//   mcHist_X = PlotUtils::GetProjectionX(mcHist, maskHist);
//   mcHist_Y = PlotUtils::GetProjectionY(mcHist, maskHist);

//   dataHist_X = PlotUtils::GetProjectionX(dataHist, maskHist);
//   dataHist_Y = PlotUtils::GetProjectionY(dataHist, maskHist);

//   // Depending on the option either the rate of only X or only Y is used.

//   // If using Y rate, scale X to match data and vice versa
//   // Note: Projection will have already accounted for masking.
//   if (isProjFitY) mcHist_X->Scale(PlotUtils::GetDataMCRatio(dataHist_X, mcHist_X));
//   if (isProjFitX) mcHist_Y->Scale(PlotUtils::GetDataMCRatio(dataHist_Y, mcHist_Y));

//   // Now get individual chi2 from each
//   double chi2X = StatUtils::GetChi2FromDiag(dataHist_X, mcHist_X);
//   double chi2Y = StatUtils::GetChi2FromDiag(dataHist_Y, mcHist_Y);

//   double chi2 = chi2X  + chi2Y;

//   mcHist_X->SetTitle(Form("%d", chi2X));
//   mcHist_Y->SetTitle(Form("%d", chi2Y));
  
//   return chi2;
// }

/*
  Fake Data
*/
//******************************************************************** 
void Measurement2D::SetFakeDataValues(std::string fakeOption) {
//******************************************************************** 

  // This is the original data           
  if (!(this->dataOrig)) this->dataOrig = (TH2D*)this->dataHist->Clone((this->measurementName+"_data_original").c_str());
  TH2D *tempData = (TH2D*)this->dataHist->Clone();

  TFile *fake = new TFile();

  if (fakeOption.compare("MC")==0){
    LOG(SAM) << this->measurementName <<"Setting fake data from MC "<<std::endl;
    this->dataHist = (TH2D*)this->mcHist->Clone((this->measurementName+"_MC").c_str());
    if (this->mcHist->Integral() == 0.0) LOG(SAM) << this->measurementName <<"Invalid histogram"<<std::endl;
  }
  else {
    fake = new TFile(fakeOption.c_str());
    this->dataHist = (TH2D*)fake->Get((this->measurementName+"_MC").c_str());
  }

  this->dataHist ->SetNameTitle((this->measurementName+"_FAKE").c_str(), (this->measurementName+this->plotTitles).c_str());

  int nbins_x = this->dataHist->GetNbinsX();
  int nbins_y = this->dataHist->GetNbinsY();
  double alpha_i = 0.0;

  for (int i = 0; i < nbins_x; i++){
    for (int j = 0; j < nbins_y; j++){

      if (tempData->GetBinContent(i+1,j+1) == 0.0) continue;

      alpha_i =  this->dataHist->GetBinContent(i+1,j+1)/this->dataOrig->GetBinContent(i+1,j+1);
      this->dataHist->SetBinError(i+1,j+1,alpha_i*this->dataOrig->GetBinError(i+1,j+1));

    }
  }
  this->dataTrue = (TH2D*) this->dataHist->Clone();

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
  if (!this->mcHist) return std::vector<TH1*> (1, this->mcHist);

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

  std::cout << measurementName << " chi2 = " << GetLikelihood() << std::endl;

  this->mcHist->SetTitle(chi2.str().c_str());
  this->mcHist->SetLineWidth(2);
  this->mcHist->SetLineColor(plotcolor);
  this->mcHist->SetFillColor(plotcolor);
  this->mcHist->SetLineStyle(plotstyle);
  this->mcHist->SetFillStyle(plotfillstyle);

  return std::vector<TH1*> (1, this->mcHist);
};

//******************************************************************** 
std::vector<TH1*> Measurement2D::GetDataList(){
//******************************************************************** 

  // If this isn't a NULL pointer, make the plot pretty!

  if (!this->dataHist) return std::vector<TH1*> (1, this->dataHist);
  
  this->dataHist->SetLineWidth(2);
  this->dataHist->SetLineColor(kBlue);

  return std::vector<TH1*> (1, this->dataHist);
};


//********************************************************************
void Measurement2D:: GetBinContents(std::vector<double>& cont, std::vector<double>& err){
  //********************************************************************

  int count = 0;
  for (int i = 0; i < (this->mcHist->GetNbinsX()+2) * (this->mcHist->GetNbinsY()+2); i++){
    cont.push_back(this->mcHist->GetBinContent(i));
    err.push_back(this->mcHist->GetBinError(i));
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

  for (int i = 0; i < this->mcHist->GetNbinsX(); i++){
    if (this->dataHist->GetBinContent(i+1) == 0.0 and this->dataHist->GetBinError(i+1) == 0.0) continue;

    if (getMC){
      
      vals[0] += this->mcHist->GetBinContent(i+1) * this->mcHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += this->mcHist->GetBinError(i+1) * this->mcHist->GetBinError(i+1) * this->mcHist->GetXaxis()->GetBinWidth(i+1) * this->mcHist->GetXaxis()->GetBinWidth(i+1);
    
    } else if (getDT){

      vals[0] += this->dataHist->GetBinContent(i+1) * this->dataHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += this->dataHist->GetBinError(i+1) * this->dataHist->GetBinError(i+1) * this->dataHist->GetXaxis()->GetBinWidth(i+1) * this->dataHist->GetXaxis()->GetBinWidth(i+1);

    }
  }

  // If not diag Get the total error from the covariance
  if (!this->isDiag and getDT){
    vals[1] = 0.0;
    for (int i = 0; i < this->dataHist->GetNbinsX(); i++){
      for(int j = 0; j < this->dataHist->GetNbinsX(); j++){

	vals[1] += (*fullcovar)(i,j);

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
  if (!this->mcHist and !this->dataHist){
    LOG(SAM) << this->measurementName <<"Incomplete histogram set!"<<std::endl;
    return;
  }

  // Get Draw Options
  drawOpt = FitPar::Config().GetParS("drawopts");
  bool drawData   = (drawOpt.find("DATA") != std::string::npos);
  bool drawNormal = (drawOpt.find("MC") != std::string::npos);
  bool drawEvents = (drawOpt.find("EVT") != std::string::npos);
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
  
  // Save standard plots
  if (drawData)    this->GetDataList().at(0)->Write();
  if (drawNormal)  this->GetMCList()  .at(0)->Write();

  // Generate a simple map                                                                                 
  if (!mapHist)
    mapHist = StatUtils::GenerateMap(dataHist);

  // Convert to 1D Lists                                                                              
  TH1D* data_1D = StatUtils::MapToTH1D(dataHist, mapHist);
  TH1D* mc_1D   = StatUtils::MapToTH1D(mcHist,   mapHist);
  TH1I* mask_1D = StatUtils::MapToMask(maskHist, mapHist);
  
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
  if(this->eventType == 4 or this->eventType==3){ return; }

  // Draw Extra plots
  if (drawFine)    this->GetFineList().at(0)->Write();
  if (drawFlux)    this->fluxHist->Write();
  if (drawEvents)  this->eventHist->Write();
  if (isMask and drawMask) this->maskHist->Write( (this->measurementName + "_MSK").c_str() ); //< save mask
  if (drawMap) this->mapHist->Write( (this->measurementName + "_MAP").c_str() ); //< save map

  // Save neut stack
  if (drawModes){
    THStack combo_mcHist_PDG = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_PDG").c_str(), (TH1**)this->mcHist_PDG, 0);
    combo_mcHist_PDG.Write();
  }

  // Save Matrix plots
  if (drawMatrix and fullcovar and covar and decomp){ 

    TH2D cov = TH2D((*this->fullcovar));
    cov.SetNameTitle((this->measurementName+"_cov").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
    cov.Write();

    TH2D covinv = TH2D((*this->covar));
    covinv.SetNameTitle((this->measurementName+"_covinv").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
    covinv.Write();

    TH2D covdec = TH2D((*this->decomp));
    covdec.SetNameTitle((this->measurementName+"_covdec").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
    covdec.Write();

  }
  

  // Save ratio plots if required 
  if (drawRatio){

    // Needed for error bars
    for(int i = 0; i < this->mcHist->GetNbinsX()*this->mcHist->GetNbinsY(); i++)
      this->mcHist->SetBinError(i+1,0.0);

    this->dataHist->GetSumw2();
    this->mcHist->GetSumw2();

    // Create Ratio Histograms
    TH2D* dataRatio = (TH2D*) this->dataHist->Clone((this->measurementName + "_data_RATIO").c_str());
    TH2D* mcRatio   = (TH2D*) this->mcHist->Clone((this->measurementName + "_MC_RATIO").c_str());
    
    mcRatio->Divide(this->mcHist);
    dataRatio->Divide(this->mcHist);
    
    // Cancel bin errors on MC
    for(int i = 0; i < mcRatio->GetNbinsX()*mcRatio->GetNbinsY(); i++) {
      mcRatio->SetBinError(i+1,this->mcHist->GetBinError(i+1) / this->mcHist->GetBinContent(i+1));
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
    TH2D* mcShape = (TH2D*) this->mcHist->Clone((this->measurementName + "_MC_SHAPE").c_str());
    
    mcShape->Scale( this->dataHist->Integral("width") 
			  / this->mcHist->Integral("width"));
    
    mcShape->SetLineWidth(3);
    mcShape->SetLineStyle(7); //dashes
    
    mcShape->Write();

    // Save shape ratios
    if (drawRatio){
    
      // Needed for error bars
      mcShape->GetSumw2();
      
      // Create shape ratio histograms
      TH2D* mcShapeRatio   = (TH2D*)mcShape->Clone((this->measurementName + "_MC_SHAPE_RATIO").c_str());
      TH2D* dataShapeRatio = (TH2D*)dataHist->Clone((this->measurementName + "_data_SHAPE_RATIO").c_str());
      
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
  
  if (isProjFitX or isProjFitY or drawProj){

    // If not already made, make the projections
    if (!mcHist_X){
      PlotUtils::MatchEmptyBins(dataHist,mcHist);
      
      mcHist_X = PlotUtils::GetProjectionX(mcHist, maskHist);
      mcHist_Y = PlotUtils::GetProjectionY(mcHist, maskHist);
      
      dataHist_X = PlotUtils::GetProjectionX(dataHist, maskHist);
      dataHist_Y = PlotUtils::GetProjectionY(dataHist, maskHist);

      double chi2X = StatUtils::GetChi2FromDiag(dataHist_X, mcHist_X);
      double chi2Y = StatUtils::GetChi2FromDiag(dataHist_Y, mcHist_Y);

      mcHist_X->SetTitle(Form("%d", chi2X));
      mcHist_Y->SetTitle(Form("%d", chi2Y));      
    }

    // Save the histograms
    dataHist_X->Write();
    mcHist_X->Write();

    dataHist_Y->Write();
    mcHist_Y->Write();
  }

  
  // Returning
  LOG(SAM) << this->measurementName  << "Written Histograms: "<<this->measurementName<<std::endl;
  return;
};


THStack Measurement2D::GetModeStack(){
  THStack combo_hist = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_PDG").c_str(), (TH1**)this->mcHist_PDG, 0);
  return combo_hist;
}
