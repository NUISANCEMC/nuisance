#include "SplineMerger.h"

void SplineMerger::AddSplineSetFromFile(TFile* file){

  TTree* tr = (TTree*) file->Get("spline_reader");
  std::vector<std::string>* tempspline = 0;
  std::vector<std::string>* temptype = 0;
  std::vector<std::string>* tempform = 0;
  std::vector<std::string>* temppoints = 0;


  // Loop over all splines and add a TString in the TTree for its inputs                                                                                            
  tr->SetBranchAddress("Spline", &tempspline);
  tr->SetBranchAddress("Type",   &temptype);
  tr->SetBranchAddress("Form",   &tempform);
  tr->SetBranchAddress("Points", &temppoints);
  tr->GetEntry(0);

  // Copy over                                                                                                                                                                                                                             
  for (size_t i = 0; i < tempspline->size(); i++){
    fSpline.push_back(tempspline->at(i));
    fType.push_back(temptype->at(i));
    fForm.push_back(tempform->at(i));
    fPoints.push_back(temppoints->at(i));
  }

  delete tr;


  // Now Get the coefficients setup.
  size_t index = fSplineTreeList.size();
  fSplineTreeList.push_back( (TTree*) file->Get("spline_tree") );

}

void SplineMerger::SetupSplineSet(){

  // Define NCoEff
  fNCoEff = 0;
  for (size_t i = 0; i < fSplineSizeList.size(); i++){
    fNCoEff += fSplineSizeList[i];
  }

  // Define Storer
  fCoEffStorer = new float[fNCoEff];

  // Loop over each TTree and set spline address
  int off = 0;
  for (size_t i = 0; i < fSplineSizeList.size(); i++){
    float* add = &(fCoEffStorer[off]);
    TTree* tree = (fSplineTreeList[i]);
    tree->SetBranchAddress("SplineCoeff", add);
    off += fSplineSizeList[i];
  }

}


void SplineMerger::GetEntry(int entry){
  for (size_t i = 0; i < fSplineTreeList.size(); i++){
    fSplineTreeList[i]->GetEntry(entry);
  }
}

void SplineMerger::Write(std::string name){

  // Create a TTree with each form and scan points in it.                                                                                                                                                                                  
  TTree* tr = new TTree(name.c_str(), name.c_str());

  // Loop over all splines and add a TString in the TTree for its inputs                                                                                                                                                                   
  tr->Branch("Spline", &fSpline);
  tr->Branch("Type", &fType);
  tr->Branch("Form", &fForm);
  tr->Branch("Points", &fPoints);
  tr->Fill();
  tr->Write();

  delete tr;
}

void SplineMerger::AddCoefficientsToTree(TTree* tree){
  tree->Branch("SplineCoeff", fCoEffStorer, Form("SplineCoeff[%d]/F", fNCoEff));
}

void SplineMerger::FillMergedSplines(int entry){
  GetEntry(entry);

  size_t count = 0;
  for (size_t i = 0; i < fSplineSizeList.size(); i++){
    for (size_t j = 0; j < fSplineSizeList[i]; j++){
      fCoEffStorer[count] = fSplineAddressList[i][j];
      count++;
    }
  }
}

