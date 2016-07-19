#include "minimizerFCN.h"
#include <stdio.h> 
#include "FitUtils.h"

minimizerFCN::minimizerFCN(std::string cardfile,  TFile *outfile){
  
  out       = outfile;
  card      = cardfile;
  
  LoadSamples(card);

  useFullCovar = false; // default                                                                                  
  this->current_iteration = 0;
  filledMC = false;
  randGen = new TRandom3();
  randGen->SetSeed(time(NULL));
};

minimizerFCN::~minimizerFCN() {
  for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    delete exp;
  }
};


double minimizerFCN::DoEval(const double *x) const {

  dialChanged = false;

  std::cout<<"Evaluating"<<std::endl;
  // WEIGHT ENGINE
  FitBase::GetRW()->UpdateWeightEngine(x);
  dialChanged = FitBase::GetRW()->HasDialChanged();
  FitBase::EvtManager().ResetWeightFlags();
  
  std::cout<<"Reconfiguring samples"<<std::endl;
  // SORT SAMPLES
  ReconfigureSamples();


  std::cout<<"Getting LIKE"<<std::endl;
  // GET TEST STAT
  double likelihood = GetLikelihood();

  // PRINT PROGRESS
  LOG(FIT) << "Current Stat (iter. "<< this->current_iteration << ") = "<<likelihood<<std::endl;
  return likelihood;
}


int minimizerFCN::GetNDOF() {

  int nDOF = 0;

  for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    nDOF += exp->GetNDOF();
  }
  
  return nDOF;
}

double minimizerFCN::GetLikelihood() const {

  double chi2 = 0.0;

  if (!FitPar::Config().GetParB("JOINT_COVARIANCE")) {
    for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){

      MeasurementBase* exp = *iter;
      chi2 += exp->GetLikelihood();
    }

  } else {

  std::vector<double> mcBins;
  std::vector<double> dataBins;
  std::vector<double> difBins;
  
  mcBins.clear();
  dataBins.clear();
  difBins.clear();
  
  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;

    std::vector<TH1*> mcHists = exp->GetMCList();
    std::vector<TH1*> dataHists = exp->GetDataList();
    
    for (UInt_t hiter = 0; hiter < mcHists.size(); hiter++){
      
      int dim = mcHists[hiter]->GetNbinsX()*mcHists[hiter]->GetNbinsY();
      TH1* dataTemp = (TH1*)dataHists[hiter];
      TH1* mcTemp   = (TH1*)mcHists[hiter];

      for (int ibin =0; ibin < dim;ibin++ ){
	mcBins  .push_back(mcTemp->GetBinContent(ibin+1));
	dataBins.push_back(dataTemp->GetBinContent(ibin+1));
	difBins.push_back(mcTemp->GetBinContent(ibin+1) - dataTemp->GetBinContent(ibin+1));
      }
    }
  }
  
  int dim = difBins.size();  
  for (int i = 0; i < dim; i++){
    for (int j = 0; j < dim; j++){
      
      chi2 += difBins.at(i) * difBins.at(j) * 1E38 * 1E38 * (*this->FullCovar)(i,j);
      
    }
  }
  }

  return chi2;
};

void minimizerFCN::CreateFullCovarMatrix(){


  /*// Hack to get NBins for full covar
  int dim = 0;

  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    std::vector<TH1*> mcHists = exp->GetMCList();
    
    for (int hiter = 0; hiter < mcHists.size(); hiter++)
      dim += mcHists.at(hiter)->GetNbinsX()*mcHists.at(hiter)->GetNbinsY();
  
  }  
    
  // Create the full covar
  
  (this->FullCovar) = new TMatrixDSym(dim);
  //  LOG(MIN)<<"Created matrix with dim = "<<dim<<std::endl;
  int row  = 0;
  int col  = 0;
  int offset = 0;

  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    TH2D temp = (TH2D)exp->GetCovarMatrix();
    int nbins = temp.GetNbinsX();


    for (int i = 0; i < nbins; i++){
      for (int j = 0; j < nbins; j++){
	
	//	LOG(MIN)<<"Before Fill covar "<<row<<","<<col<<std::endl;

	(*this->FullCovar)(row, col) = (temp.GetBinContent(i+1,j+1));
	col++;
	// LOG(MIN)<<"Filling covar "<<row<<","<<col<<std::endl;
      }
      col = offset;
      row++;
    }
    
    // offset col for next exp
    col = nbins + offset;
    offset += nbins;
  }
  */

  return;

};
  


void minimizerFCN::LoadSamples(std::string cardFile) 
{
  LOG(MIN)<<"Initializing Samples"<<std::endl;

  // Read the card file here and load objects
  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);
    std::string token;
    int val = 0;
    std::string name;
    std::string type;
    std::string files;
    double norm;
    bool FoundSample = false;

    // check the type
    while(std::getline(stream, token, ' ')){
      // Strip any leading whitespace from the stream
      stream >> std::ws;

      // Ignore comments
      if (token.c_str()[0] == '#') continue;

      if (val == 0){
	if (token.compare("sample") !=0 ){
	  break;
	}
	FoundSample = true;
      } else if (val == 1) {
	name = token;
      } else if (val == 2) {
	type = token;
      } else if (val == 3) {
	files = token;
      } else if (val == 4) {
	std::istringstream stemp(token);
	stemp >> norm;
      } else break;
      val++;
    }
  
    if (!FoundSample) continue;

    bool LoadedSample = SampleUtils::LoadSample( &fChain, name, files, type, fakeData, FitBase::GetRW() );

    if (!LoadedSample) {
      ERR(FTL) << "Could not load sample provided: "<<name<<std::endl;
      continue;
    }
    
    sampleNames.push_back(name);
    sampleFiles.push_back(files);
    sampleTypes.push_back(type);
    sampleNorms.push_back(norm);

  }
  card.close();

};


// Used to override signal skipping functions
void minimizerFCN::ReconfigureSamples(bool fullconfig) const{

  int starttime = time(NULL);
  this->current_iteration = this->current_iteration + 1;  
  LOG(MIN) << "Starting Reconfigure iter. "<<this->current_iteration<<std::endl;

  // Loop over all Measurement Classes
  std::list<MeasurementBase*>::const_iterator iterSam = fChain.begin();
  for ( ; iterSam != fChain.end(); iterSam++){

    MeasurementBase* exp = (*iterSam);
    
    // If only norm has changed...
    if (!dialChanged and !fullconfig and !filledMC){
      exp->Renormalise();
      continue;
    }

    if (!fullconfig) exp->ReconfigureFast();
    else             exp->Reconfigure();
  }
  filledMC = true;
  LOG(MIN) << "-> Time Taken "<< time(NULL) - starttime << "s"<<std::endl;
  
}


void minimizerFCN::SetFakeData(std::string fakeOpt){
                                                                                     
  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    exp->SetFakeDataValues(fakeOpt);
  }
  return;
};



TH1D* minimizerFCN::GetXSecPlot(std::string type){

  bool data = true;
  if (!type.compare("MC")) data = false;

  std::vector<double> xsection;
  std::vector<double> xsection_errs;
  std::vector<std::string> samplenames;

  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    samplenames.push_back(exp->GetName());

    std::vector<double> vals;// = exp->GetXSec(type);
    xsection.push_back(vals[0]);
    xsection_errs.push_back(vals[1]);
  }

  int nsamples = xsection.size();
  std::string title = "sample_xsecs_";

  if (data) title += "data";
  else title += "MC";
  
  TH1D* xsecPlot = new TH1D(title.c_str(),title.c_str(), nsamples,0,nsamples);

  for (int i = 0; i < nsamples; i++){
    xsecPlot->SetBinContent(i+1, xsection.at(i));
    xsecPlot->SetBinError(i+1, xsection_errs.at(i));

    std::string sample = (samplenames.at(i));
    xsecPlot->GetXaxis()->SetBinLabel(i+1, (sample).c_str());
  }

  return xsecPlot;
}


void minimizerFCN::ThrowSamples(){

  std::list<MeasurementBase*>::const_iterator iter = fChain.begin();
  for ( ; iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    exp->ThrowCovariance();
  }
}


void minimizerFCN::ReconfigureSignal(){
  this->ReconfigureSamples(false);
  return;
}
 
void minimizerFCN::ReconfigureAllEvents() const{
  this->ReconfigureSamples(true);
  return;
}

void minimizerFCN::Write(){

  // Loop over individual experiments and save relevant information                                                                                                                                
  // Loop over all returned STL vectors (joint fits have more than one set of return values)                                                                                                       
  LOG(MIN)<<"Writing each of the data classes:"<<std::endl;
  for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    exp->Write();
  }

  return;
  // Save data and MC cross-section plots                                                                                                                                                          
  TH1D* tempMCXsec = GetXSecPlot("MC");
  tempMCXsec->Write();

  TH1D* tempDataXsec = GetXSecPlot("DATA");
  tempDataXsec->Write();

  if (useFullCovar){
    TH2D* temp = new TH2D(*FullCovar);

    // Make neater by removing empty bins                                                                                                                                                        
    int nbinsx = temp->GetNbinsX();
    int nbinsy = temp->GetNbinsY();
    TH2D* finalcov = new TH2D("cov","cov",nbinsx,0,nbinsx,nbinsy,0,nbinsy);
    for (int i =0; i < nbinsx; i++){
      for (int j = 0; j < nbinsy; j++){
	if (temp->GetBinContent(i+1,j+1) == 0.0) continue;
	finalcov->SetBinContent(i+1,j+1,temp->GetBinContent(i+1,j+1));
      }
    }

    // Save the output                                                                                                                                                                             
    finalcov->Write("FullChi2Covariance");
    delete temp;
    delete finalcov;
  }
};

