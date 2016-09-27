#include "jointFCN.h"
#include <stdio.h> 
#include "FitUtils.h"

jointFCN::jointFCN(std::string cardfile,  TFile *outfile){
  
  out       = outfile;
  card      = cardfile;
  FitPar::Config().out = out;
  
  LoadSamples(card);

  useFullCovar = false; // default          
  this->current_iteration = 0;
  filledMC = false;
  randGen = new TRandom3();
  randGen->SetSeed(time(NULL));

  out->cd();
  iteration_tree = NULL;
  dialvals = NULL;
  ndials = 0;
  
};

jointFCN::~jointFCN() {
  for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    delete exp;
  }
  DestroyIterationTree();
  if (dialvals) delete dialvals;
};


void jointFCN::CreateIterationTree(std::string name, FitWeight* rw){

  if (iteration_tree) delete iteration_tree;
  iteration_tree = new TTree(name.c_str(), name.c_str());
  iteration_tree->Branch("likelihood",&likelihood,"likelihood/D");
  
  std::vector<std::string> dials = rw->GetDialNames();
  ndials = dials.size();
  dialvals = new double[ndials];
  
  for (int i = 0; i < ndials; i++){
    iteration_tree->Branch( dials[i].c_str(), &dialvals[i],
			    (dials[i] + "/D").c_str() );
  }
}

void jointFCN::DestroyIterationTree(){
  if (!iteration_tree){
    delete iteration_tree;
  }
}

void jointFCN::WriteIterationTree(){
  if (!iteration_tree){
    ERR(FTL) << "Can't save empty iteration tree!" << endl;
    throw;
  }
  iteration_tree->Write();
}

void jointFCN::FillIterationTree(FitWeight* rw) const{

  if (!iteration_tree){
    ERR(FTL) << "Trying to fill iteration_tree when it is NULL!" << endl;
    throw;
  }
  
  rw->GetAllDials( dialvals, ndials );
  iteration_tree->Fill();
}




double jointFCN::DoEval(const double *x) const {
 
  // WEIGHT ENGINE
  dialChanged = FitBase::GetRW()->HasRWDialChanged(x);
  FitBase::GetRW()->UpdateWeightEngine(x);
  if (dialChanged) {
    FitBase::GetRW()->Reconfigure();
    FitBase::EvtManager().ResetWeightFlags();
  }
  //else {
  //    std::cout<<"DIALS HAVENT CHANGED"<<std::endl;
  //    sleep(2);
  //}

  // SORT SAMPLES
  ReconfigureSamples();

  // GET TEST STAT
  likelihood = GetLikelihood();

  // PRINT PROGRESS
  LOG(FIT) << "Current Stat (iter. "<< this->current_iteration << ") = "<<likelihood<<std::endl;

  
  if (iteration_tree) FillIterationTree(FitBase::GetRW());
  return likelihood;
}


int jointFCN::GetNDOF() {

  int nDOF = 0;

  for (std::list<MeasurementBase*>::iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    nDOF += exp->GetNDOF();
  }
  
  return nDOF;
}

double jointFCN::GetLikelihood() const {

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

void jointFCN::CreateFullCovarMatrix(){


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
  


void jointFCN::LoadSamples(std::string cardFile) 
{
  LOG(MIN)<<"Initializing Samples"<<std::endl;

  // Read the card file here and load objects
  std::string line;
  std::ifstream card(cardFile.c_str(), ifstream::in);

  while(std::getline(card, line, '\n')){
    std::istringstream stream(line);

    // Skip Empties
    stream >> std::ws;
    if (line.c_str()[0] == '#') continue;
    if (line.empty()) continue;

    // Parse line
    std::vector<std::string> samplevect = PlotUtils::FillVectorSFromString(line," ");

    // Skip non sample lines
    if (samplevect[0].compare("sample")) continue;

    // Get Name
    std::string name  = samplevect[1];

    // Get Input
    std::string files = samplevect[2];

    // Get Type [ if blank = DEFAULT ]
    std::string type = "DEFAULT";
    if (samplevect.size() > 3)
      type  = samplevect[3];

    // Get Norm [ if blank = 1.0 ]
    double norm = 1.0;
    if (samplevect.size() > 4){
      std::istringstream stemp(samplevect[4]);
      stemp >> norm;
    }

    // Create Sample Class
    out->cd();
    LOG(MIN) << "Loading up sample: "<<name<<" << " <<files << " ("<<type<<")"<<std::endl;
    bool LoadedSample = SampleUtils::LoadSample( &fChain, name, files, type, fakeData, FitBase::GetRW() );

    if (!LoadedSample) {
      ERR(FTL) << "Could not load sample provided: "<<name<<std::endl;
      ERR(FTL) << "Check spelling with that in src/FCN/SampleList.cxx" << endl;
      throw;
    }
    
    sampleNames.push_back(name);
    sampleFiles.push_back(files);
    sampleTypes.push_back(type);
    sampleNorms.push_back(norm);

  }
  card.close();
  
};


// Used to override signal skipping functions
void jointFCN::ReconfigureSamples(bool fullconfig) const{

  int starttime = time(NULL);
  this->current_iteration = this->current_iteration + 1;  
  LOG(MIN) << "Starting Reconfigure iter. "<<this->current_iteration<<std::endl;

  // Loop over all Measurement Classes
  std::list<MeasurementBase*>::const_iterator iterSam = fChain.begin();
  for ( ; iterSam != fChain.end(); iterSam++){

    MeasurementBase* exp = (*iterSam);

    /*
    std::cout<<"dialChanged = "<<dialChanged<<std::endl;
    std::cout<<"filledMC = "<<filledMC<<std::endl;
    std::cout<<"fullconfig = "<<fullconfig<<std::endl;
    */    

    if (dialChanged or !filledMC or fullconfig){
      if (!fullconfig and filledMC) exp->ReconfigureFast();
      else                          exp->Reconfigure();
    // If RW Not needed just do normalisation
    } else {      
      exp->Renormalise();
    }    
  }
  filledMC = true;
  LOG(MIN) << "-> Time Taken "<< time(NULL) - starttime << "s"<<std::endl;
  
}


void jointFCN::SetFakeData(std::string fakeOpt){
             
  for (std::list<MeasurementBase*>::const_iterator iter = fChain.begin(); iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    exp->SetFakeDataValues(fakeOpt);
  }
  return;
};


std::list<MeasurementBase*> jointFCN::GetSampleList(){
  return fChain;
}

TH1D* jointFCN::GetXSecPlot(std::string type){

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


void jointFCN::ThrowSamples(){

  std::list<MeasurementBase*>::const_iterator iter = fChain.begin();
  for ( ; iter != fChain.end(); iter++){
    MeasurementBase* exp = *iter;
    exp->ThrowCovariance();
  }
}


void jointFCN::ReconfigureSignal(){
  this->ReconfigureSamples(false);
  return;
}
 
void jointFCN::ReconfigureAllEvents() const{
  this->ReconfigureSamples(true);
  return;
}

void jointFCN::Write(){

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

