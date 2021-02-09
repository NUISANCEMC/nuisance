// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "FitSplineHead.h"

FitSplineHead::FitSplineHead(TFile* infile, std::string name){
  current_offset = 1;
  Read((TTree*) infile->Get(name.c_str()));
}

double FitSplineHead::CalcWeight(const Double_t* incoeff){
  double weight = incoeff[0];
  double nom = weight;

  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    weight *= (*iter)->DoEval(incoeff) * nom;
  }
  return weight;
};

void FitSplineHead::Reconfigure(std::vector<int> dial_enums, std::vector<double> dial_values){
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    (*iter)->ReconfigureSpline(dial_enums, dial_values);
  }
};

void FitSplineHead::SetupEventWeights(BaseFitEvt* event){

  int ncoeff = 1;
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    ncoeff += (*iter)->npar;
  }

  event->ResetDialCoeff();
  event->CreateDialCoeff(ncoeff);
}

void FitSplineHead::AddSpline(FitSpline* spl){
  spl->SetOffset( this->current_offset );
  this->SplineObjects.push_back(spl);
  this->current_offset += spl->npar;
}

int FitSplineHead::GetCurrentOffset(){
  int ncoeff = 1;
  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    ncoeff += (*iter)->npar;
  }
  return ncoeff;
}


void FitSplineHead::Write(std::string inname){

  TTree* headtree = new TTree(inname.c_str(),inname.c_str());

  std::string ident = "";
  std::string dist  = "";
  int nenums = 0;
  int enumlist[1000];
  std::string points = "";

  headtree->Branch("ident",&ident);
  headtree->Branch("dist",&dist);
  headtree->Branch("nenums",&nenums,"nenums/I");
  headtree->Branch("enums",&enumlist,"enums[nenums]/I");
  headtree->Branch("points",&points);

  for (std::list<FitSpline*>::iterator iter = SplineObjects.begin();
       iter != SplineObjects.end(); iter++){
    FitSpline* spl = (*iter);
    
    ident  = spl->GetIdent();
    dist   = spl->GetDist();
    points = spl->GetPoints();
 
    std::vector<int> tempenums = spl->GetEnums();
    nenums = tempenums.size();
    for (int j = 0; j < nenums; j++){
      enumlist[j] = tempenums[j];
    }

    headtree->Fill();
  }

  headtree->Write();
}


void FitSplineHead::Read(TTree* tn){

  std::string* ident = new std::string();
  std::string* dist  = new std::string();
  int nenums = 0;
  int enumlist[1000];
  std::string* points = new std::string();

  tn->SetBranchAddress("ident",  &ident);
  tn->SetBranchAddress("dist",   &dist);
  tn->SetBranchAddress("nenums", &nenums);
  tn->SetBranchAddress("enums",  &enumlist);
  tn->SetBranchAddress("points", &points);

  for (int i = 0; i < tn->GetEntries(); i++){
    tn->GetEntry(i);

    std::vector<int> enumvect;
    for (int j = 0; j < nenums; j++){
      enumvect.push_back(enumlist[j]);
    }
    
    FitSpline* spl = new FitSpline(*ident, *dist, enumvect, *points);
    AddSpline(spl);
  }
}
