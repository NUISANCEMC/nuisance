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

#include "Measurement1D.h"

/* 
   Constructor/destructor Functions
*/
//********************************************************************  
Measurement1D::Measurement1D() {
//********************************************************************  
  this->currentNorm = 1.0;
  this->mcHist = NULL;
  this->dataHist = NULL;
  this->mcFine = NULL;

  this->maskHist = NULL;
  this->covar = NULL;
  this->fullcovar = NULL;
  this->decomp = NULL;

  this->fakeDataFile = "";
  this->fluxHist  = NULL;
  this->eventHist = NULL;
  this->xsecHist  = NULL;

  default_types = "FIX/FULL/CHI2";
  allowed_types = "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/MASK";

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

}

//********************************************************************  
Measurement1D::~Measurement1D(){
//********************************************************************  
}

//********************************************************************  
void Measurement1D::Init(){
//********************************************************************  
}


/*
  Setup Functions
  -- All these are used only at the start of the Measurement 
*/
//********************************************************************   
void Measurement1D::SetupMeasurement(std::string inputfile, std::string type, FitWeight *rw, std::string fkdt){
//********************************************************************   

  // Reset everything to NULL                                                                                                                          
  Init();

  // Check if name contains Evt, indicating that it is a raw number of events measurements                                       
  // and should thus be treated as once                                                                                                                
  isRawEvents = false;
  if ((measurementName.find("Evt") != std::string::npos) && isRawEvents == false) {
    isRawEvents = true;
    LOG(SAM) << "Found event rate measurement but isRawEvents == false!" << std::endl;
    LOG(SAM) << "Overriding this and setting isRawEvents == true!" << std::endl;
  }

  isEnu1D = false;
  if (measurementName.find("XSec_1DEnu") != std::string::npos) {
    isEnu1D = true;
    LOG(SAM) << "::" << measurementName << "::" << std::endl;
    LOG(SAM) << "Found XSec Enu measurement, applying flux integrated scaling, not flux averaged!" << std::endl;
  }

  if (isEnu1D && isRawEvents) {
    LOG(SAM) << "Found 1D Enu XSec distribution AND isRawEvents, is this really correct?!" << std::endl;
    LOG(SAM) << "Check experiment constructor for " << measurementName << " and correct this!" << std::endl;
    LOG(SAM) << "I live in " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

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

  //  // Set Enu Flux Scaling
  //  if (isFlatFluxFolding) this->Input()->ApplyFluxFolding( this->defaultFluxHist );
  
}

//******************************************************************** 
void Measurement1D::SetupDefaultHist(){
//******************************************************************** 

  // Setup mcHist
  this->mcHist = (TH1D*) this->dataHist->Clone();
  this->mcHist->SetNameTitle( (this->measurementName + "_MC").c_str(), (this->measurementName + "_MC" + this->plotTitles).c_str() );

  // Setup mcFine
  Int_t nBins = this->mcHist->GetNbinsX();
  this->mcFine = new TH1D( (this->measurementName + "_MC_FINE").c_str(), (this->measurementName + "_MC_FINE" + this->plotTitles).c_str(),
			   nBins*6, this->mcHist->GetBinLowEdge(1), this->mcHist->GetBinLowEdge(nBins+1) );


  mcStat = (TH1D*) mcHist->Clone();
  mcStat->Reset();
  
  this->mcHist->Reset();
  this->mcFine->Reset();

  // Setup the NEUT Mode Array
  PlotUtils::CreateNeutModeArray((TH1D*)this->mcHist,(TH1**)this->mcHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1**)this->mcHist_PDG);

  return;
}

//********************************************************************
void Measurement1D::SetFitOptions(std::string opt){
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

  return;
};

//********************************************************************
void Measurement1D::SetDataValues(std::string dataFile) {
//********************************************************************

  // Override this function if the input file isn't in a suitable format
  LOG(SAM) << "Reading data from: " << dataFile.c_str() << std::endl;
  this->dataHist = PlotUtils::GetTH1DFromFile(dataFile, (this->measurementName+"_data"), this->plotTitles);
  this->dataTrue = (TH1D*)this->dataHist->Clone();

  return;
};

//******************************************************************** 
void Measurement1D::SetDataFromDatabase(std::string inhistfile, std::string histname){
//********************************************************************

  LOG(SAM) << "Filling histogram from "<< inhistfile << "->"<< histname <<std::endl;

  this->dataHist = PlotUtils::GetTH1DFromRootFile((std::string(std::getenv("EXT_FIT")) + "/data/" + inhistfile), histname);
  this->dataHist->SetNameTitle((this->measurementName+"_data").c_str(), (this->measurementName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetDataFromFile(std::string inhistfile, std::string histname){
//********************************************************************

  LOG(SAM) << "Filling histogram from "<< inhistfile << "->"<< histname <<std::endl;
  this->dataHist = PlotUtils::GetTH1DFromRootFile((inhistfile), histname);

  return;
};




//********************************************************************
void Measurement1D::SetCovarMatrix(std::string covarFile){
//********************************************************************
  
  TFile* tempFile = new TFile(covarFile.c_str(),"READ");

  TH2D* covarPlot = new TH2D();
  TH2D* decmpPlot = new TH2D();
  TH2D* covarInvPlot = new TH2D();
  TH2D* fullcovarPlot = new TH2D();
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("thrown_covariance");

  if (this->isShape || this->isFree) covName = "shp_";
  if (this->isDiag) covName += "diag";
  else  covName += "full";

  covarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  covarInvPlot = (TH2D*) tempFile->Get((covName + "covinv").c_str());

  
  if (!covOption.compare("SUB")) fullcovarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());      
  else if (!covOption.compare("FULL"))   fullcovarPlot = (TH2D*) tempFile->Get("fullcov");
  else ERR(WRN) <<"Incorrect thrown_covariance option in parameters."<<std::endl;

  int dim = int(this->dataHist->GetNbinsX());//-this->masked->Integral()); 
  int covdim = int(this->dataHist->GetNbinsX());

  this->covar = new TMatrixDSym(dim);
  this->fullcovar = new TMatrixDSym(dim);
  this->decomp = new TMatrixDSym(dim);

  int row,column = 0;
  row = 0;
  column = 0;
  for (Int_t i = 0; i < covdim; i++){

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
    StatUtils::SetDataErrorFromCov(dataHist, fullcovar);
  }            

  // Get Deteriminant and inverse matrix
  covDet = this->covar->Determinant();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrixFromText(std::string covarFile, int dim){
//********************************************************************

  // WARNIGN this reads in the data CORRELATIONS

  // Make a counter to track the line number
  int row = 0;

  std::string line;
  std::ifstream covar(covarFile.c_str(),ifstream::in);
  
  this->covar = new TMatrixDSym(dim);
  this->fullcovar = new TMatrixDSym(dim);
  if(covar.is_open()) LOG(SAM) << "Reading covariance matrix from file: " << covarFile << std::endl;
  else ERR(FTL) <<"Covariance matrix provided is incorrect: "<<covarFile<<std::endl;

  // MINERvA CC1pip needs slightly different method
  // Only half the covariance matrix is given and I'm too lazy to write the full one so let the code do it instead
  if (measurementName.find("MINERvA_CC1pip") == std::string::npos && measurementName.find("MINERvA_CCNpip") == std::string::npos) {
    while(std::getline(covar, line, '\n')){
      std::istringstream stream(line);
      double entry;
      int column = 0;

      // Loop over entries and insert them into matrix
      // Multiply by the errors to get the covariance, rather than the correlation matrix
      while(stream >> entry){

        double val = entry * this->dataHist->GetBinError(row+1)*1E38*this->dataHist->GetBinError(column+1)*1E38;

        (*this->covar)(row, column) = val;
        (*this->fullcovar)(row, column) = val;

        column++;
      }
    
      row++;
    }

    // Robust matrix inversion method
    TDecompSVD LU = TDecompSVD(*this->covar);
    this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  } else { // Here's the MINERvA CC1pip method; very similar
    while(std::getline(covar, line, '\n')){
      std::istringstream stream(line);
      double entry;
      int column = 0;
      while (column < dim) {
      if (column < row) {
        (*this->covar)(row,column) = (*this->covar)(column,row);
        column++;
      } else {
        while(stream >> entry){
          double val = entry*(this->dataHist->GetBinError(row+1)*1E38*this->dataHist->GetBinError(column+1)*1E38); // need in these units to do Cholesky
          (*this->covar)(row, column) = val;
          (*this->fullcovar)(row, column) = val;

          column++;
          }
        }
      }
      row++;
    }

    // Robust matrix inversion method
    TDecompChol a = TDecompChol(*this->covar);
    this->covar = new TMatrixDSym(dim, a.Invert().GetMatrixArray(), "");

  } // end special treatment for MINERvA CC1pi+

  return;
};

//********************************************************************
void Measurement1D::SetSmearingMatrix(std::string smearFile, int true_dim, int reco_dim){
//********************************************************************
  
  // The smearing matrix describes the migration from true bins (rows) to reco bins (columns)
  // Counter over the true bins!
  int row = 0;

  std::string line;
  std::ifstream smear(smearFile.c_str(),ifstream::in);

  // Note that the smearing matrix may be rectangular. 
  this->smear = new TMatrixD(true_dim, reco_dim);

  if(smear.is_open()) LOG(SAM) << "Reading smearing matrix from file: " << smearFile << std::endl;
  else ERR(FTL) <<"Smearing matrix provided is incorrect: "<< smearFile <<std::endl;

  while(std::getline(smear, line, '\n')){
    std::istringstream stream(line);
    double entry;
    int column = 0;

    while(stream >> entry){

      double val = entry;
      (*this->smear)(row, column) = val/100.;   // Convert to fraction from percentage (this may not be general enough)
      column++;
    }
    row++;
  }
  return;
}

//********************************************************************
void Measurement1D::SetCovarFromDataFile(std::string covarFile, std::string covName){
//********************************************************************

  LOG(SAM) << "Getting covariance from "<<covarFile<<"->"<<covName<<std::endl;

  TFile* tempFile = new TFile(covarFile.c_str(),"READ");
  TH2D* covPlot = (TH2D*) tempFile->Get(covName.c_str());
  covPlot->SetDirectory(0);
  
  int dim = covPlot->GetNbinsX();
  this->fullcovar = new TMatrixDSym(dim);
  

  for (int i = 0; i < dim; i++){
    for (int j = 0; j < dim; j++){
      (*this->fullcovar)(i,j) = covPlot->GetBinContent(i+1,j+1);
    }
  }

  this->covar = (TMatrixDSym*) this->fullcovar->Clone();
  this->decomp =(TMatrixDSym*) this->fullcovar->Clone();
  
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");
  
  TDecompChol LUChol = TDecompChol(*this->decomp);
  LUChol.Decompose();
  this->decomp = new TMatrixDSym(dim, LU .GetU().GetMatrixArray(), "");

  return;  
};


//********************************************************************
void Measurement1D::SetBinMask(std::string maskFile){
//********************************************************************

  // Create a mask histogram.    
  int nbins = this->dataHist->GetNbinsX();
  this->maskHist = new TH1I((this->measurementName+"_maskHist").c_str(),(this->measurementName+"_maskHist; Bin; Mask?").c_str(),nbins,0,nbins);
  int row ,column= 0;
  std::string line;
  std::ifstream mask(maskFile.c_str(),ifstream::in);

  if (mask.is_open()) LOG(SAM) <<"Reading bin mask from file: "<<maskFile <<std::endl;
  else std::cerr <<" Cannot find mask file."<<std::endl;

  while(std::getline(mask, line, '\n')){
    std::istringstream stream(line);
    int column = 0;
    double entry;
    int bin;
    while (stream >> entry){
      if (column == 0) bin = int(entry);
      if (column > 1) break;
      column++;
    }
    this->maskHist->SetBinContent(bin,entry);
  }

  // Set masked data bins to zero
  PlotUtils::MaskBins(this->dataHist, this->maskHist);
  
  return;
}


/*
  XSec Functions
*/

//********************************************************************
void Measurement1D::SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm){
//********************************************************************

  // Note this expects the flux bins to be given in terms of MeV
  LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

  TGraph f(fluxFile.c_str(),"%lg %lg");

  this->fluxHist = new TH1D((this->measurementName+"_flux").c_str(), (this->measurementName+"; E_{#nu} (GeV)").c_str(), f.GetN()-1, minE, maxE);

  Double_t *yVal = f.GetY();

  for (int i = 0; i<fluxHist->GetNbinsX(); ++i)
    this->fluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);
  
};

//********************************************************************
double Measurement1D::TotalIntegratedFlux(std::string intOpt, double low, double high){
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;
  
  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->fluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral;
};



/*
  Reconfigure LOOP
*/

//********************************************************************
void Measurement1D::ResetAll(){
//********************************************************************

  // Simple function to reset the mc Histograms incase that is all that is needed.

  // Clear histograms
  this->mcHist->Reset();
  this->mcFine->Reset();
  this->mcStat->Reset();
  
  PlotUtils::ResetNeutModeArray((TH1**)this->mcHist_PDG);
  
  return;
};

//******************************************************************** 
void Measurement1D::FillHistograms(){
//********************************************************************

  if (Signal){
    this->mcHist->Fill(X_VAR, Weight);
    this->mcFine->Fill(X_VAR, Weight);
    this->mcStat->Fill(X_VAR, 1.0);

    PlotUtils::FillNeutModeArray(mcHist_PDG, Mode, X_VAR, Weight);    
  }

  return;
};

//********************************************************************
void Measurement1D::ScaleEvents(){
//********************************************************************
  
  // Simple function to scale to xsec result if this is all that is needed.
  // Scale bin errors correctly  
  TH1D* tempFine = (TH1D*) mcFine->Clone();

  // Should apply different scaling for:
  // 1D Enu distributions -- need bin by bin flux unfolding (bin by bin flux integration)
  // 1D count distributions -- need shape scaling to data
  // anything else -- flux averages

  // Scaling for raw event rates
  if (isRawEvents) {
    this->mcHist->Scale(dataHist->Integral()/mcHist->Integral());
    this->mcFine->Scale(dataHist->Integral()/mcFine->Integral());
  // Scaling for XSec as function of Enu
  } else if (isEnu1D) {
    PlotUtils::FluxUnfoldedScaling(mcHist, fluxHist);
    PlotUtils::FluxUnfoldedScaling(mcFine, fluxHist);

    mcHist->Scale(scaleFactor);
    mcFine->Scale(scaleFactor);
  // Any other differential scaling
  } else {
    this->mcHist->Scale(this->scaleFactor, "width");
    this->mcFine->Scale(this->scaleFactor, "width");
  }


  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  // Scale the MC histogram
  for(int i=0; i<mcStat->GetNbinsX();i++) {

    if (mcStat->GetBinContent(i+1) != 0) {
      this->mcHist->SetBinError(i+1, this->mcHist->GetBinContent(i+1) * mcStat->GetBinError(i+1) / mcStat->GetBinContent(i+1) );
    } else {
      this->mcHist->SetBinError(i+1, this->mcHist->Integral());
    }
  }

  // Scale the fine MC histogram
  for(int i=0; i<tempFine->GetNbinsX();i++) {
    if (tempFine->GetBinContent(i+1) != 0) {
      this->mcFine->SetBinError(i+1, this->mcFine->GetBinContent(i+1) * tempFine->GetBinError(i+1) / tempFine->GetBinContent(i+1) );
    } else { 
      this->mcFine->SetBinError(i+1, this->mcFine->Integral());
    }
  }

  // Scale NEUT Mode Array
  PlotUtils::ScaleNeutModeArray((TH1**)this->mcHist_PDG, this->scaleFactor, "width");

  return;
};

//********************************************************************
void Measurement1D::ApplyNormScale(double norm){
//********************************************************************

  this->currentNorm = norm;
  
  this->mcHist->Scale(1.0/norm);
  this->mcFine->Scale(1.0/norm);

  PlotUtils::ScaleNeutModeArray((TH1**)this->mcHist_PDG, 1.0/norm);
  
  return;
};

//********************************************************************
void Measurement1D::ApplySmearingMatrix(){
//********************************************************************

  if (!this->smear){
    ERR(WRN) << this->measurementName <<": attempted to apply smearing matrix, but none was set"<<std::endl;
    return;
  }

  TH1D* unsmeared = (TH1D*)mcHist->Clone();
  TH1D* smeared   = (TH1D*)mcHist->Clone();
  smeared->Reset();

  // Loop over reconstructed bins
  // true = row; reco = column
  for (int rbin=0; rbin < this->smear->GetNcols(); ++rbin){
    
    // Sum up the constributions from all true bins
    double rBinVal = 0;
    
    // Loop over true bins
    for (int tbin=0; tbin < this->smear->GetNrows(); ++tbin){
      rBinVal += (*this->smear)(tbin,rbin)*unsmeared->GetBinContent(tbin+1);
    }
    smeared->SetBinContent(rbin+1, rBinVal);
  }
  this->mcHist = (TH1D*)smeared->Clone();
  
  return;
}

/*
  Statistic Functions - Outsources to StatUtils
*/

//********************************************************************
int Measurement1D::GetNDOF(){
//********************************************************************
  return this->dataHist->GetNbinsX(); // - this->maskHist->Integral();
}

//********************************************************************
double Measurement1D::GetLikelihood(){
//********************************************************************

  double stat = 0.0;

  // Fix weird masking bug
  if (!isMask){
    if (maskHist){
      maskHist = NULL;
    }
  }
  
  // Sort Initial Scaling
  double scaleF = this->dataHist->Integral(1,this->dataHist->GetNbinsX(),"width")/this->mcHist->Integral(1, this->mcHist->GetNbinsX(), "width");

  if (isShape){
    this->mcHist->Scale(scaleF);
    this->mcFine->Scale(scaleF);
  }

  // Get Chi2
  if (isChi2){
    if (!isDiag){
      
      if (!isChi2SVD) {
	stat = StatUtils::GetChi2FromCov(dataHist, mcHist, covar, maskHist);
      } else {
	stat = StatUtils::GetChi2FromSVD(dataHist,mcHist, fullcovar, maskHist);
      }
      
    } else {
      
      if (isRawEvents) {
	stat = StatUtils::GetChi2FromEventRate(dataHist, mcHist, maskHist);
      } else {
	stat = StatUtils::GetChi2FromDiag(dataHist, mcHist, maskHist);
      } 
    }
  } else {
    if (!this->isDiag){
      if (!isChi2SVD) stat = StatUtils::GetLikelihoodFromCov(dataHist, mcHist, covar, maskHist);
      else stat = StatUtils::GetLikelihoodFromSVD(dataHist,mcHist, fullcovar, maskHist);
    } else {
      if (this->isRawEvents) stat = StatUtils::GetLikelihoodFromEventRate(dataHist, mcHist, maskHist);
      else stat = StatUtils::GetLikelihoodFromDiag(dataHist, mcHist, maskHist);
    }
  }

  // Sort Penalty Terms
  if (this->addNormPenalty){
    double penalty = (1. - this->currentNorm)*(1. - this->currentNorm)/(this->normError*this->normError);
    stat += penalty;
  }

  LOG(REC) << this->measurementName<<": Sample Chi^2 = " << stat <<std::endl;

  // Return to normal scaling
  if (this->isShape){
    this->mcHist->Scale(1./scaleF);
    this->mcFine->Scale(1./scaleF);
  }

  return stat;
}


//********************************************************************
void Measurement1D::SetFakeDataValues(std::string fakeOption) {
//********************************************************************

  // Reset things
  if (usingfakedata){
    this->ResetFakeData();
  } else {
    usingfakedata = true;
  }
  
  // Make a copy of the original data histogram.
  if (!(this->dataOrig)) this->dataOrig = (TH1D*)this->dataHist->Clone((this->measurementName+"_data_original").c_str());
  
  TH1D *tempData = (TH1D*)this->dataHist->Clone();
  TFile *fake = new TFile();
  
  if (fakeOption.compare("MC")==0){
    LOG(SAM) << "Setting fake data from MC "<<std::endl;
    this->dataHist = (TH1D*)this->mcHist->Clone((this->measurementName+"_MC").c_str());
    if (this->mcHist->Integral() == 0.0) ERR(WRN) << this->measurementName <<": Invalid histogram"<<std::endl;
  }
  else {
    fake = new TFile(fakeOption.c_str());
    this->dataHist = (TH1D*)fake->Get((this->measurementName+"_MC").c_str());    
  }

  this->dataHist ->SetNameTitle((this->measurementName+"_FAKE").c_str(), (this->measurementName+this->plotTitles).c_str());

  this->dataTrue = (TH1D*)this->dataHist->Clone();
  this->dataTrue ->SetNameTitle((this->measurementName+"_FAKE_TRUE").c_str(), (this->measurementName+this->plotTitles).c_str());
  
  int nbins = this->dataHist->GetNbinsX();
  double alpha_i = 0.0;
  double alpha_j = 0.0;
  
  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){

      alpha_i = this->dataHist->GetBinContent(i+1)/tempData->GetBinContent(i+1);
      alpha_j = this->dataHist->GetBinContent(j+1)/tempData->GetBinContent(j+1);

      (*this->covar)(i,j)     = (1.0/(alpha_i*alpha_j))*(*this->covar)(i,j);
      (*this->fullcovar)(i,j) = alpha_i*alpha_j*(*this->fullcovar)(i,j);
      
    }
  }

  (this->covar) = (TMatrixDSym*) this->fullcovar->Clone();
  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(nbins, LU .Invert().GetMatrixArray(), "");

  return;
};


//********************************************************************
void Measurement1D::ResetFakeData(){
//********************************************************************
  
  if (usingfakedata)
    if (this->dataHist) delete dataHist;
    this->dataHist = (TH1D*) this->dataTrue->Clone((this->measurementName+"_FKDAT").c_str());

  return;
}

//********************************************************************    
void Measurement1D::ResetData(){
//********************************************************************    
  
  if (usingfakedata)
    if (this->dataHist) delete dataHist;
    this->dataHist = (TH1D*) this->dataTrue->Clone((this->measurementName+"_Data").c_str());

  usingfakedata = false;

}



//********************************************************************
void Measurement1D::ThrowCovariance(){
//********************************************************************

  // Take a decomposition and use it to throw the current dataset.
  // Requires dataTrue also be set incase used repeatedly.
  
  delete dataHist;
  this->dataHist = StatUtils::ThrowHistogram(this->dataTrue, this->fullcovar);

  return;
};



/*
  Access Functions
*/

//********************************************************************
std::vector<TH1*> Measurement1D::GetMCList(){
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
  this->mcHist->SetLineWidth(3);
  this->mcHist->SetLineColor(plotcolor);
  this->mcHist->SetFillColor(plotcolor);
  this->mcHist->SetLineStyle(plotstyle);
  this->mcHist->SetFillStyle(plotfillstyle);

  return std::vector<TH1*> (1, this->mcHist);
};

//********************************************************************
std::vector<TH1*> Measurement1D::GetDataList(){
//********************************************************************
  // If this isn't a NULL pointer, make the plot pretty!

  if (!this->dataHist) return std::vector<TH1*> (1, this->dataHist);
  
  this->dataHist->SetLineWidth(2);
  this->dataHist->SetMarkerStyle(8);

  this->dataHist->SetLineColor(kBlack);

  return std::vector<TH1*> (1, this->dataHist);
};


//********************************************************************
void Measurement1D::GetBinContents(std::vector<double>& cont, std::vector<double>& err){
//********************************************************************
  
  // Return a vector of the main bin contents
  for (int i = 0; i < this->mcHist->GetNbinsX(); i++){
    cont.push_back(this->mcHist->GetBinContent(i+1));
    err.push_back(this->mcHist->GetBinError(i+1));
    
  }
  
  return;
};


//********************************************************************     
std::vector<double> Measurement1D::GetXSec(std::string option){
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
  if (!this->isDiag and !this->isRawEvents and getDT and fullcovar){
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

// Save all the histograms at once
//********************************************************************
void Measurement1D::Write(std::string drawOpt){
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
  bool drawXSec   = (drawOpt.find("XSEC") != std::string::npos);
  bool drawFlux   = (drawOpt.find("FLUX") != std::string::npos);
  bool drawMask   = (drawOpt.find("MASK") != std::string::npos);
  bool drawCov    = (drawOpt.find("COV")  != std::string::npos);
  bool drawInvCov = (drawOpt.find("INVCOV") != std::string::npos);
  bool drawDecomp = (drawOpt.find("DECOMP") != std::string::npos);
  
  LOG(SAM)<<"Writing Normal Plots" <<std::endl;
  // Save standard plots
  if (drawData)    this->GetDataList().at(0)->Write();
  if (drawNormal)  this->GetMCList()  .at(0)->Write();

   // Save only mc and data if splines
  if(this->eventType == 4 or this->eventType==3){ return; }

  // Draw Extra plots
  LOG(SAM)<<"Writing Fine List"<<std::endl;
  if (drawFine)    this->GetFineList().at(0)->Write();
  LOG(SAM)<<"Writing events"<<std::endl;
  if (drawFlux)    this->fluxHist->Write();
  LOG(SAM)<<"Writing true events"<<std::endl;
  //  if (drawXSec)    this->xsecHist->Write();
  if (drawEvents)  this->eventHist->Write();
  if (isMask and drawMask) this->maskHist->Write( (this->measurementName + "_MSK").c_str() ); //< save mask

  // Save neut stack
  if (drawModes){
    LOG(SAM) << "Writing MC Hist PDG"<<std::endl;
    THStack combo_mcHist_PDG = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_PDG").c_str(), (TH1**)this->mcHist_PDG, 0);
    combo_mcHist_PDG.Write();
  }

  // Save Matrix plots
  if (!isRawEvents and !isDiag){
    if (drawCov and fullcovar){
      TH2D cov = TH2D((*this->fullcovar));
      cov.SetNameTitle((this->measurementName+"_cov").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
      cov.Write();
    }

    if (drawInvCov and covar){
      TH2D covinv = TH2D((*this->covar));
      covinv.SetNameTitle((this->measurementName+"_covinv").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
      covinv.Write();
    }

    if (drawDecomp and decomp){
      TH2D covdec = TH2D((*this->decomp));
      covdec.SetNameTitle((this->measurementName+"_covdec").c_str(),(this->measurementName+"_cov;Bins; Bins;").c_str());
      covdec.Write();
    }
  }
  

  // Save ratio plots if required 
  if (drawRatio){

    // Needed for error bars
    for(int i = 0; i < this->mcHist->GetNbinsX()*this->mcHist->GetNbinsY(); i++)
      this->mcHist->SetBinError(i+1,0.0);

    this->dataHist->GetSumw2();
    this->mcHist->GetSumw2();

    // Create Ratio Histograms
    TH1D* dataRatio = (TH1D*) this->dataHist->Clone((this->measurementName + "_data_RATIO").c_str());
    TH1D* mcRatio   = (TH1D*) this->mcHist->Clone((this->measurementName + "_MC_RATIO").c_str());
    
    mcRatio->Divide(this->mcHist);
    dataRatio->Divide(this->mcHist);
    
    // Cancel bin errors on MC
    for(int i = 0; i < mcRatio->GetNbinsX(); i++)
      mcRatio->SetBinError(i+1,this->mcHist->GetBinError(i+1) / this->mcHist->GetBinContent(i+1));

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
    TH1D* mcShape = (TH1D*) this->mcHist->Clone((this->measurementName + "_MC_SHAPE").c_str());
    
    double shapeScale = dataHist->Integral("width")/mcHist->Integral("width");
    mcShape->Scale(shapeScale);

    std::stringstream ss;
    ss << shapeScale;
    mcShape->SetTitle(ss.str().c_str());
    
    mcShape->SetLineWidth(3);
    mcShape->SetLineStyle(7); //dashes
    
    mcShape->Write();

    // Save shape ratios
    if (drawRatio) {
    
      // Needed for error bars
      mcShape->GetSumw2();
      
      // Create shape ratio histograms
      TH1D* mcShapeRatio   = (TH1D*)mcShape->Clone((this->measurementName + "_MC_SHAPE_RATIO").c_str());
      TH1D* dataShapeRatio = (TH1D*)dataHist->Clone((this->measurementName + "_data_SHAPE_RATIO").c_str());
      
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

  // Returning
  LOG(SAM) << this->measurementName  << "Written Histograms: "<<this->measurementName<<std::endl;
  return;
};




THStack Measurement1D::GetModeStack(){
  THStack combo_hist = PlotUtils::GetNeutModeStack((this->measurementName + "_MC_PDG").c_str(), (TH1**)this->mcHist_PDG, 0);
  return combo_hist;
}
