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

#include "Measurement1D.h"

/*
   Constructor/destructor Functions
*/
//********************************************************************
Measurement1D::Measurement1D() {
//********************************************************************
  this->fCurrentNorm = 1.0;
  this->fMCHist = NULL;
  this->fDataHist = NULL;
  this->fMCFine = NULL;

  this->fMaskHist = NULL;
  this->covar = NULL;
  this->fullcovar = NULL;
  this->fDecomp = NULL;

  this->fakeDataFile = "";
  this->fFluxHist  = NULL;
  this->fEventHist = NULL;
  this->fXSecHist  = NULL;

  fDefaultTypes = "FIX/FULL/CHI2";
  fAllowedTypes = "FIX,FREE,SHAPE/FULL,DIAG/CHI2/NORM/ENUCORR/Q2CORR/ENU1D/MASK";

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
  fIsEnu1D = false;

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
  fIsRawEvents = false;
  if ((fName.find("Evt") != std::string::npos) && fIsRawEvents == false) {
    fIsRawEvents = true;
    LOG(SAM) << "Found event rate measurement but fIsRawEvents == false!" << std::endl;
    LOG(SAM) << "Overriding this and setting fIsRawEvents == true!" << std::endl;
  }

  fIsEnu1D = false;
  if (fName.find("XSec_1DEnu") != std::string::npos) {
    fIsEnu1D = true;
    LOG(SAM) << "::" << fName << "::" << std::endl;
    LOG(SAM) << "Found XSec Enu measurement, applying flux integrated scaling, not flux averaged!" << std::endl;
  }

  if (fIsEnu1D && fIsRawEvents) {
    LOG(SAM) << "Found 1D Enu XSec distribution AND fIsRawEvents, is this really correct?!" << std::endl;
    LOG(SAM) << "Check experiment constructor for " << fName << " and correct this!" << std::endl;
    LOG(SAM) << "I live in " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  rw_engine = rw;

  this->SetupInputs(inputfile);

  // Set Default Options
  SetFitOptions( this->fDefaultTypes );

  // Set Passed Options
  SetFitOptions(type);

  // Still adding support for flat flux inputs
  //  // Set Enu Flux Scaling
  //  if (isFlatFluxFolding) this->Input()->ApplyFluxFolding( this->defaultFluxHist );

}

//********************************************************************
void Measurement1D::SetupDefaultHist(){
//********************************************************************

  // Setup fMCHist
  this->fMCHist = (TH1D*) this->fDataHist->Clone();
  this->fMCHist->SetNameTitle( (this->fName + "_MC").c_str(), (this->fName + "_MC" + this->fPlotTitles).c_str() );

  // Setup fMCFine
  Int_t nBins = this->fMCHist->GetNbinsX();
  this->fMCFine = new TH1D( (this->fName + "_MC_FINE").c_str(), (this->fName + "_MC_FINE" + this->fPlotTitles).c_str(),
			   nBins*6, this->fMCHist->GetBinLowEdge(1), this->fMCHist->GetBinLowEdge(nBins+1) );


  fMCStat = (TH1D*) fMCHist->Clone();
  fMCStat->Reset();

  this->fMCHist->Reset();
  this->fMCFine->Reset();

  // Setup the NEUT Mode Array
  PlotUtils::CreateNeutModeArray((TH1D*)this->fMCHist,(TH1**)this->fMCHist_PDG);
  PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_PDG);

  // Setup bin masks using sample name
  if (fIsMask){
    std::string maskloc = FitPar::Config().GetParDIR( this->fName + ".mask");
    if (maskloc.empty()){
      maskloc = FitPar::GetDataBase() + "/masks/" + fName + ".mask";
    }

    SetBinMask(maskloc);
  }
  
  return;
}

//********************************************************************
void Measurement1D::SetFitOptions(std::string opt){
//********************************************************************

  // Do nothing if default given
  if (opt == "DEFAULT") return;
  
  // CHECK Conflicting Fit Options
  std::vector<std::string> fit_option_allow = PlotUtils::FillVectorSFromString(fAllowedTypes, "/");
  for (UInt_t i = 0; i < fit_option_allow.size(); i++){
    std::vector<std::string> fit_option_section = PlotUtils::FillVectorSFromString(fit_option_allow.at(i), ",");
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
  std::vector<std::string> fit_options_input = PlotUtils::FillVectorSFromString(opt,"/");
  for (UInt_t i = 0; i < fit_options_input.size(); i++){
    if (fAllowedTypes.find(fit_options_input.at(i)) == std::string::npos){

      ERR(FTL) <<"ERROR: Fit Option '"<<fit_options_input.at(i)<<"' Provided is not allowed for this measurement."<<std::endl;
      ERR(FTL) <<"Fit Options should be provided as a '/' seperated list (e.g. FREE/DIAG/NORM)" << std::endl;
      ERR(FTL) <<"Available options for "<<fName<<" are '"<< fAllowedTypes <<"'"<<std::endl;

      exit(-1);
    }
  }

  // Set TYPE
  this->fFitType = opt;

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
  if (opt.find("ENU1D") != std::string::npos) fIsEnu1D     = true;
  if (opt.find("NORM")  != std::string::npos) fAddNormPen = true;
  if (opt.find("MASK")  != std::string::npos) fIsMask      = true;

  return;
};

//********************************************************************
void Measurement1D::SetDataValues(std::string dataFile) {
//********************************************************************

  // Override this function if the input file isn't in a suitable format
  LOG(SAM) << "Reading data from: " << dataFile.c_str() << std::endl;
  this->fDataHist = PlotUtils::GetTH1DFromFile(dataFile, (this->fName+"_data"), this->fPlotTitles);
  this->fDataTrue = (TH1D*)this->fDataHist->Clone();

  return;
};

//********************************************************************
void Measurement1D::SetDataFromDatabase(std::string inhistfile, std::string histname){
//********************************************************************

  LOG(SAM) << "Filling histogram from "<< inhistfile << "->"<< histname <<std::endl;
  this->fDataHist = PlotUtils::GetTH1DFromRootFile((std::string(std::getenv("EXT_FIT")) + "/data/" + inhistfile), histname);
  this->fDataHist->SetNameTitle((this->fName+"_data").c_str(), (this->fName + "_data").c_str());

  return;
};

//********************************************************************
void Measurement1D::SetDataFromFile(std::string inhistfile, std::string histname){
//********************************************************************

  LOG(SAM) << "Filling histogram from "<< inhistfile << "->"<< histname <<std::endl;
  this->fDataHist = PlotUtils::GetTH1DFromRootFile((inhistfile), histname);

  return;
};




//********************************************************************
void Measurement1D::SetCovarMatrix(std::string covarFile){
//********************************************************************

  // Covariance function, only really used when reading in the MB Covariances.

  TFile* tempFile = new TFile(covarFile.c_str(),"READ");

  TH2D* covarPlot = new TH2D();
  //  TH2D* decmpPlot = new TH2D();
  TH2D* covarInvPlot = new TH2D();
  TH2D* fullcovarPlot = new TH2D();
  std::string covName = "";
  std::string covOption = FitPar::Config().GetParS("thrown_covariance");

  if (this->fIsShape || this->fIsFree) covName = "shp_";
  if (this->fIsDiag) covName += "diag";
  else  covName += "full";

  covarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  covarInvPlot = (TH2D*) tempFile->Get((covName + "covinv").c_str());


  if (!covOption.compare("SUB")) fullcovarPlot = (TH2D*) tempFile->Get((covName + "cov").c_str());
  else if (!covOption.compare("FULL"))   fullcovarPlot = (TH2D*) tempFile->Get("fullcov");
  else ERR(WRN) <<"Incorrect thrown_covariance option in parameters."<<std::endl;

  int dim = int(this->fDataHist->GetNbinsX());//-this->masked->Integral());
  int covdim = int(this->fDataHist->GetNbinsX());

  this->covar = new TMatrixDSym(dim);
  this->fullcovar = new TMatrixDSym(dim);
  this->fDecomp = new TMatrixDSym(dim);

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
  if (!this->fIsDiag){
    StatUtils::SetDataErrorFromCov(fDataHist, fullcovar);
  }

  // Get Deteriminant and inverse matrix
  fCovDet = this->covar->Determinant();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  return;
};

//********************************************************************
void Measurement1D::SetCovarMatrixFromText(std::string covarFile, int dim){
//********************************************************************

  // WARNING this reads in the data CORRELATIONS

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
  if (fName.find("MINERvA_CC1pip") == std::string::npos && fName.find("MINERvA_CCNpip") == std::string::npos) {
    while(std::getline(covar, line, '\n')){
      std::istringstream stream(line);
      double entry;
      int column = 0;

      // Loop over entries and insert them into matrix
      // Multiply by the errors to get the covariance, rather than the correlation matrix
      while(stream >> entry){

        double val = entry * this->fDataHist->GetBinError(row+1)*1E38*this->fDataHist->GetBinError(column+1)*1E38;

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
          double val = entry*(this->fDataHist->GetBinError(row+1)*1E38*this->fDataHist->GetBinError(column+1)*1E38); // need in these units to do Cholesky
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
void Measurement1D::SetSmearingMatrix(std::string smearfile, int truedim, int recodim){
//********************************************************************

  // The smearing matrix describes the migration from true bins (rows) to reco bins (columns)
  // Counter over the true bins!
  int row = 0;

  std::string line;
  std::ifstream smear(smearfile.c_str(),ifstream::in);

  // Note that the smearing matrix may be rectangular.
  this->fSmearMatrix = new TMatrixD(truedim, recodim);

  if(smear.is_open()) LOG(SAM) << "Reading smearing matrix from file: " << smearfile << std::endl;
  else ERR(FTL) <<"Smearing matrix provided is incorrect: "<< smearfile <<std::endl;

  while(std::getline(smear, line, '\n')){
    std::istringstream stream(line);
    double entry;
    int column = 0;

    while(stream >> entry){

      double val = entry;
      (*this->fSmearMatrix)(row, column) = val/100.;   // Convert to fraction from percentage (this may not be general enough)
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
  this->fDecomp =(TMatrixDSym*) this->fullcovar->Clone();

  TDecompSVD LU = TDecompSVD(*this->covar);
  this->covar = new TMatrixDSym(dim, LU .Invert().GetMatrixArray(), "");

  TDecompChol LUChol = TDecompChol(*this->fDecomp);
  LUChol.Decompose();
  this->fDecomp = new TMatrixDSym(dim, LU .GetU().GetMatrixArray(), "");

  return;
};


//********************************************************************
void Measurement1D::SetBinMask(std::string maskFile){
//********************************************************************

  // Create a mask histogram.
  int nbins = this->fDataHist->GetNbinsX();
  this->fMaskHist = new TH1I((this->fName+"_fMaskHist").c_str(),(this->fName+"_fMaskHist; Bin; Mask?").c_str(),nbins,0,nbins);
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
    this->fMaskHist->SetBinContent(bin,entry);
  }

  // Set masked data bins to zero
  PlotUtils::MaskBins(this->fDataHist, this->fMaskHist);

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

  this->fFluxHist = new TH1D((this->fName+"_flux").c_str(), (this->fName+"; E_{#nu} (GeV)").c_str(), f.GetN()-1, minE, maxE);

  Double_t *yVal = f.GetY();

  for (int i = 0; i<fFluxHist->GetNbinsX(); ++i)
    this->fFluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);

};

//********************************************************************
double Measurement1D::TotalIntegratedFlux(std::string intOpt, double low, double high){
//********************************************************************

  if(GetInput()->GetType() == kGiBUU){
    return 1.0;
  }

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = this->fFluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fFluxHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->fFluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

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
  this->fMCHist->Reset();
  this->fMCFine->Reset();
  this->fMCStat->Reset();

  PlotUtils::ResetNeutModeArray((TH1**)this->fMCHist_PDG);

  return;
};

//********************************************************************
void Measurement1D::FillHistograms(){
//********************************************************************

  if (Signal){
    this->fMCHist->Fill(fXVar, Weight);
    this->fMCFine->Fill(fXVar, Weight);
    this->fMCStat->Fill(fXVar, 1.0);

    PlotUtils::FillNeutModeArray(fMCHist_PDG, Mode, fXVar, Weight);
  }

  return;
};

//********************************************************************
void Measurement1D::ScaleEvents(){
//********************************************************************

  // Simple function to scale to xsec result if this is all that is needed.
  // Scale bin errors correctly
  TH1D* tempFine = (TH1D*) fMCFine->Clone();

  // Should apply different scaling for:
  // 1D Enu distributions -- need bin by bin flux unfolding (bin by bin flux integration)
  // 1D count distributions -- need shape scaling to data
  // anything else -- flux averages

  // Scaling for raw event rates
  if (fIsRawEvents) {

    PlotUtils::ScaleNeutModeArray((TH1**)this->fMCHist_PDG, (fDataHist->Integral()/fMCHist->Integral()), "width");
    this->fMCHist->Scale(fDataHist->Integral()/fMCHist->Integral());
    this->fMCFine->Scale(fDataHist->Integral()/fMCFine->Integral());

  // Scaling for XSec as function of Enu
  } else if (fIsEnu1D) {
    PlotUtils::FluxUnfoldedScaling(fMCHist, fFluxHist);
    PlotUtils::FluxUnfoldedScaling(fMCFine, fFluxHist);

    fMCHist->Scale(fScaleFactor);
    fMCFine->Scale(fScaleFactor);
  // Any other differential scaling
  } else {
    this->fMCHist->Scale(this->fScaleFactor, "width");
    this->fMCFine->Scale(this->fScaleFactor, "width");
    PlotUtils::ScaleNeutModeArray((TH1**)this->fMCHist_PDG, this->fScaleFactor, "width");
  }


  // Proper error scaling - ROOT Freaks out with xsec weights sometimes
  // Scale the MC histogram
  for(int i=0; i<fMCStat->GetNbinsX();i++) {

    if (fMCStat->GetBinContent(i+1) != 0) {
      this->fMCHist->SetBinError(i+1, this->fMCHist->GetBinContent(i+1) * fMCStat->GetBinError(i+1) / fMCStat->GetBinContent(i+1) );
    } else {
      this->fMCHist->SetBinError(i+1, this->fMCHist->Integral());
    }
  }

  // Scale the fine MC histogram
  for(int i=0; i<tempFine->GetNbinsX();i++) {
    if (tempFine->GetBinContent(i+1) != 0) {
      this->fMCFine->SetBinError(i+1, this->fMCFine->GetBinContent(i+1) * tempFine->GetBinError(i+1) / tempFine->GetBinContent(i+1) );
    } else {
      this->fMCFine->SetBinError(i+1, this->fMCFine->Integral());
    }
  }

  return;
};

//********************************************************************
void Measurement1D::ApplyNormScale(double norm){
//********************************************************************

  this->fCurrentNorm = norm;

  this->fMCHist->Scale(1.0/norm);
  this->fMCFine->Scale(1.0/norm);

  PlotUtils::ScaleNeutModeArray((TH1**)this->fMCHist_PDG, 1.0/norm);

  return;
};

//********************************************************************
void Measurement1D::ApplySmearingMatrix(){
//********************************************************************

  if (!this->fSmearMatrix){
    ERR(WRN) << this->fName <<": attempted to apply smearing matrix, but none was set"<<std::endl;
    return;
  }

  TH1D* unsmeared = (TH1D*)fMCHist->Clone();
  TH1D* smeared   = (TH1D*)fMCHist->Clone();
  smeared->Reset();

  // Loop over reconstructed bins
  // true = row; reco = column
  for (int rbin=0; rbin < this->fSmearMatrix->GetNcols(); ++rbin){

    // Sum up the constributions from all true bins
    double rBinVal = 0;

    // Loop over true bins
    for (int tbin=0; tbin < this->fSmearMatrix->GetNrows(); ++tbin){
      rBinVal += (*this->fSmearMatrix)(tbin,rbin)*unsmeared->GetBinContent(tbin+1);
    }
    smeared->SetBinContent(rbin+1, rBinVal);
  }
  this->fMCHist = (TH1D*)smeared->Clone();

  return;
}

/*
  Statistic Functions - Outsources to StatUtils
*/

//********************************************************************
int Measurement1D::GetNDOF(){
//********************************************************************
  return this->fDataHist->GetNbinsX(); // - this->fMaskHist->Integral();
}

//********************************************************************
double Measurement1D::GetLikelihood(){
//********************************************************************

  double stat = 0.0;

  // Fix weird masking bug
  if (!fIsMask){
    if (fMaskHist){
      fMaskHist = NULL;
    }
  } else {
    if (fMaskHist){
      PlotUtils::MaskBins(this->fMCHist, this->fMaskHist);
    }
  }

  // Sort Initial Scaling
  double scaleF = this->fDataHist->Integral(1,this->fDataHist->GetNbinsX(),"width")/this->fMCHist->Integral(1, this->fMCHist->GetNbinsX(), "width");

  if (fIsShape){
    this->fMCHist->Scale(scaleF);
    this->fMCFine->Scale(scaleF);
  }

  // Get Chi2
  if (fIsChi2){
    if (!fIsDiag){

      if (!fIsChi2SVD) {
	stat = StatUtils::GetChi2FromCov(fDataHist, fMCHist, covar, fMaskHist);
      } else {
	stat = StatUtils::GetChi2FromSVD(fDataHist,fMCHist, fullcovar, fMaskHist);
      }

    } else {

      if (fIsRawEvents) {
	stat = StatUtils::GetChi2FromEventRate(fDataHist, fMCHist, fMaskHist);
      } else {
	stat = StatUtils::GetChi2FromDiag(fDataHist, fMCHist, fMaskHist);
      }
    }
  } else {
    if (!this->fIsDiag){
      if (!fIsChi2SVD) stat = StatUtils::GetLikelihoodFromCov(fDataHist, fMCHist, covar, fMaskHist);
      else stat = StatUtils::GetLikelihoodFromSVD(fDataHist,fMCHist, fullcovar, fMaskHist);
    } else {
      if (this->fIsRawEvents) stat = StatUtils::GetLikelihoodFromEventRate(fDataHist, fMCHist, fMaskHist);
      else stat = StatUtils::GetLikelihoodFromDiag(fDataHist, fMCHist, fMaskHist);
    }
  }

  // Sort Penalty Terms
  if (this->fAddNormPen){
    double penalty = (1. - this->fCurrentNorm)*(1. - this->fCurrentNorm)/(this->fNormError*this->fNormError);
    stat += penalty;
  }

  // Return to normal scaling
  if (this->fIsShape){
    this->fMCHist->Scale(1./scaleF);
    this->fMCFine->Scale(1./scaleF);
  }

  return stat;
}


//********************************************************************
void Measurement1D::SetFakeDataValues(std::string fakeOption) {
//********************************************************************

  // Reset things
  if (fIsFakeData){
    this->ResetFakeData();
  } else {
    fIsFakeData = true;
  }

  // Make a copy of the original data histogram.
  if (!(this->fDataOrig)) this->fDataOrig = (TH1D*)this->fDataHist->Clone((this->fName+"_data_original").c_str());

  TH1D *tempData = (TH1D*)this->fDataHist->Clone();
  TFile *fake = new TFile();

  if (fakeOption.compare("MC")==0){
    LOG(SAM) << "Setting fake data from MC "<<std::endl;
    this->fDataHist = (TH1D*)this->fMCHist->Clone((this->fName+"_MC").c_str());
    if (this->fMCHist->Integral() == 0.0) ERR(WRN) << this->fName <<": Invalid histogram"<<std::endl;
  }
  else {
    fake = new TFile(fakeOption.c_str());
    this->fDataHist = (TH1D*)fake->Get((this->fName+"_MC").c_str());
  }

  this->fDataHist ->SetNameTitle((this->fName+"_FAKE").c_str(), (this->fName+this->fPlotTitles).c_str());

  this->fDataTrue = (TH1D*)this->fDataHist->Clone();
  this->fDataTrue ->SetNameTitle((this->fName+"_FAKE_TRUE").c_str(), (this->fName+this->fPlotTitles).c_str());

  int nbins = this->fDataHist->GetNbinsX();
  double alpha_i = 0.0;
  double alpha_j = 0.0;

  for (int i = 0; i < nbins; i++){
    for (int j = 0; j < nbins; j++){

      alpha_i = this->fDataHist->GetBinContent(i+1)/tempData->GetBinContent(i+1);
      alpha_j = this->fDataHist->GetBinContent(j+1)/tempData->GetBinContent(j+1);

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

  if (fIsFakeData)
    if (this->fDataHist) delete fDataHist;
    this->fDataHist = (TH1D*) this->fDataTrue->Clone((this->fName+"_FKDAT").c_str());

  return;
}

//********************************************************************
void Measurement1D::ResetData(){
//********************************************************************

  if (fIsFakeData)
    if (this->fDataHist) delete fDataHist;
    this->fDataHist = (TH1D*) this->fDataTrue->Clone((this->fName+"_Data").c_str());

  fIsFakeData = false;

}



//********************************************************************
void Measurement1D::ThrowCovariance(){
//********************************************************************

  // Take a fDecomposition and use it to throw the current dataset.
  // Requires fDataTrue also be set incase used repeatedly.

  delete fDataHist;
  this->fDataHist = StatUtils::ThrowHistogram(this->fDataTrue, this->fullcovar);

  return;
};



/*
  Access Functions
*/

//********************************************************************
std::vector<TH1*> Measurement1D::GetMCList(){
  //********************************************************************

  // If this isn't a NULL pointer, make the plot pretty!
  if (!this->fMCHist) return std::vector<TH1*> (1, this->fMCHist);

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

  std::cout << fName << " chi2 = " << GetLikelihood() << std::endl;

  this->fMCHist->SetTitle(chi2.str().c_str());
  this->fMCHist->SetLineWidth(3);
  this->fMCHist->SetLineColor(plotcolor);
  this->fMCHist->SetFillColor(plotcolor);
  this->fMCHist->SetLineStyle(plotstyle);
  this->fMCHist->SetFillStyle(plotfillstyle);

  return std::vector<TH1*> (1, this->fMCHist);
};

//********************************************************************
std::vector<TH1*> Measurement1D::GetDataList(){
//********************************************************************
  // If this isn't a NULL pointer, make the plot pretty!

  if (!this->fDataHist) return std::vector<TH1*> (1, this->fDataHist);

  this->fDataHist->SetLineWidth(2);
  this->fDataHist->SetMarkerStyle(8);

  this->fDataHist->SetLineColor(kBlack);

  return std::vector<TH1*> (1, this->fDataHist);
};


//********************************************************************
void Measurement1D::GetBinContents(std::vector<double>& cont, std::vector<double>& err){
//********************************************************************

  // Return a vector of the main bin contents
  for (int i = 0; i < this->fMCHist->GetNbinsX(); i++){
    cont.push_back(this->fMCHist->GetBinContent(i+1));
    err.push_back(this->fMCHist->GetBinError(i+1));

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

  for (int i = 0; i < this->fMCHist->GetNbinsX(); i++){
    if (this->fDataHist->GetBinContent(i+1) == 0.0 and this->fDataHist->GetBinError(i+1) == 0.0) continue;

    if (getMC){

      vals[0] += this->fMCHist->GetBinContent(i+1) * this->fMCHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += this->fMCHist->GetBinError(i+1) * this->fMCHist->GetBinError(i+1) * this->fMCHist->GetXaxis()->GetBinWidth(i+1) * this->fMCHist->GetXaxis()->GetBinWidth(i+1);

    } else if (getDT){

      vals[0] += this->fDataHist->GetBinContent(i+1) * this->fDataHist->GetXaxis()->GetBinWidth(i+1);
      vals[1] += this->fDataHist->GetBinError(i+1) * this->fDataHist->GetBinError(i+1) * this->fDataHist->GetXaxis()->GetBinWidth(i+1) * this->fDataHist->GetXaxis()->GetBinWidth(i+1);

    }
  }

  // If not diag Get the total error from the covariance
  if (!this->fIsDiag and !this->fIsRawEvents and getDT and fullcovar){
    vals[1] = 0.0;
    for (int i = 0; i < this->fDataHist->GetNbinsX(); i++){
      for(int j = 0; j < this->fDataHist->GetNbinsX(); j++){

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
  if (!this->fMCHist and !this->fDataHist){
    LOG(SAM) << this->fName <<"Incomplete histogram set!"<<std::endl;
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
  //  bool drawMatrix = (drawOpt.find("MATRIX") != std::string::npos);
  //  bool drawXSec   = (drawOpt.find("XSEC") != std::string::npos);
  bool drawFlux   = (drawOpt.find("FLUX") != std::string::npos);
  bool drawMask   = (drawOpt.find("MASK") != std::string::npos);
  bool drawCov    = (drawOpt.find("COV")  != std::string::npos);
  bool drawInvCov = (drawOpt.find("INVCOV") != std::string::npos);
  bool drawDecomp = (drawOpt.find("DECOMP") != std::string::npos);
  bool drawCanvPDG = (drawOpt.find("CANVPDG") != std::string::npos);
  bool drawCanvMC = (drawOpt.find("CANVMC") != std::string::npos);
  
  LOG(SAM)<<"Writing Normal Plots" <<std::endl;
  // Save standard plots
  if (drawData)    this->GetDataList().at(0)->Write();
  if (drawNormal)  this->GetMCList()  .at(0)->Write();

   // Save only mc and data if splines
  if(this->fEventType == 4 or this->fEventType==3){ return; }

  // Draw Extra plots
  LOG(SAM)<<"Writing Fine List"<<std::endl;
  if (drawFine)    this->GetFineList().at(0)->Write();
  LOG(SAM)<<"Writing events"<<std::endl;
  if (drawFlux)    this->fFluxHist->Write();
  LOG(SAM)<<"Writing true events"<<std::endl;
  //  if (drawXSec)    this->fXSecHist->Write();
  if (drawEvents)  this->fEventHist->Write();
  if (fIsMask and drawMask) this->fMaskHist->Write( (this->fName + "_MSK").c_str() ); //< save mask

  // Save neut stack
  if (drawModes){
    LOG(SAM) << "Writing MC Hist PDG"<<std::endl;
    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack((this->fName + "_MC_PDG").c_str(), (TH1**)this->fMCHist_PDG, 0);
    combo_fMCHist_PDG.Write();
  }

  // Save Matrix plots
  if (!fIsRawEvents and !fIsDiag){
    if (drawCov and fullcovar){
      TH2D cov = TH2D((*this->fullcovar));
      cov.SetNameTitle((this->fName+"_cov").c_str(),(this->fName+"_cov;Bins; Bins;").c_str());
      cov.Write();
    }

    if (drawInvCov and covar){
      TH2D covinv = TH2D((*this->covar));
      covinv.SetNameTitle((this->fName+"_covinv").c_str(),(this->fName+"_cov;Bins; Bins;").c_str());
      covinv.Write();
    }

    if (drawDecomp and fDecomp){
      TH2D covdec = TH2D((*this->fDecomp));
      covdec.SetNameTitle((this->fName+"_covdec").c_str(),(this->fName+"_cov;Bins; Bins;").c_str());
      covdec.Write();
    }
  }


  // Save ratio plots if required
  if (drawRatio){

    // Needed for error bars
    for(int i = 0; i < this->fMCHist->GetNbinsX()*this->fMCHist->GetNbinsY(); i++)
      this->fMCHist->SetBinError(i+1,0.0);

    this->fDataHist->GetSumw2();
    this->fMCHist->GetSumw2();

    // Create Ratio Histograms
    TH1D* dataRatio = (TH1D*) this->fDataHist->Clone((this->fName + "_data_RATIO").c_str());
    TH1D* mcRatio   = (TH1D*) this->fMCHist->Clone((this->fName + "_MC_RATIO").c_str());

    mcRatio->Divide(this->fMCHist);
    dataRatio->Divide(this->fMCHist);

    // Cancel bin errors on MC
    for(int i = 0; i < mcRatio->GetNbinsX(); i++)
      mcRatio->SetBinError(i+1,this->fMCHist->GetBinError(i+1) / this->fMCHist->GetBinContent(i+1));

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
    TH1D* mcShape = (TH1D*) this->fMCHist->Clone((this->fName + "_MC_SHAPE").c_str());

    double shapeScale = fDataHist->Integral("width")/fMCHist->Integral("width");
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
      TH1D* mcShapeRatio   = (TH1D*)mcShape->Clone((this->fName + "_MC_SHAPE_RATIO").c_str());
      TH1D* dataShapeRatio = (TH1D*)fDataHist->Clone((this->fName + "_data_SHAPE_RATIO").c_str());

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

  // Make a pretty PDG Canvas
  if (drawCanvPDG or true){
    TCanvas* c1 = new TCanvas((this->fName + "_PDG_CANV").c_str(),
			      (this->fName + "_PDG_CANV").c_str(),
			      800,600);
    fDataHist->Draw("E1");
    fMCHist->Draw("HIST SAME");
    
    THStack combo_fMCHist_PDG = PlotUtils::GetNeutModeStack((this->fName + "_MC_PDG").c_str(),
    							   (TH1**)this->fMCHist_PDG, 0);
    combo_fMCHist_PDG.Draw("HIST SAME");
    TLegend leg = PlotUtils::GenerateStackLegend(combo_fMCHist_PDG, 0.6,0.6,0.9,0.9);
    fDataHist->Draw("E1 SAME");
    
    //leg.Draw("SAME");
    c1->Write();
  }


  if (drawCanvMC or true){
    TCanvas* c1 = new TCanvas((this->fName + "_MC_CANV").c_str(),
			      (this->fName + "_MC_CANV").c_str(),
			      800,600);
    c1->cd();
    fDataHist->Draw("E1");
    fMCHist->Draw("SAME HIST C");
    
    TH1D* mcShape = (TH1D*) this->fMCHist->Clone((this->fName + "_MC_SHAPE").c_str());
    double shapeScale = fDataHist->Integral("width")/fMCHist->Integral("width");
    mcShape->Scale(shapeScale);
    mcShape->SetLineStyle(7);

    mcShape->Draw("SAME HIST C");

    TLegend* leg = new TLegend(0.6,0.6,0.9,0.9);
    leg->AddEntry(fDataHist, (this->fName + " Data").c_str(), "ep");
    leg->AddEntry(fMCHist,   (this->fName + " MC").c_str(), "l");
    leg->AddEntry(mcShape,  (this->fName + " Shape").c_str(), "l");
  }
  
  // Returning
  LOG(SAM) << this->fName  << "Written Histograms: "<<this->fName<<std::endl;
  return;
};




THStack Measurement1D::GetModeStack(){
  THStack combo_hist = PlotUtils::GetNeutModeStack((this->fName + "_MC_PDG").c_str(), (TH1**)this->fMCHist_PDG, 0);
  return combo_hist;
}
