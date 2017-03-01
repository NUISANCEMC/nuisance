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

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "TRandom3.h"

#include "BaseFitEvt.h"

struct LetterBackronym {
  LetterBackronym(size_t n, std::string const &b, float p = 1.0,
                  std::string const &t = "") {
    NUsed = n;
    Backkie = b;
    ProbAccept = p;
    TagLine = t;
  };
  size_t NUsed;
  float ProbAccept;
  std::string Backkie;
  std::string TagLine;
};

__attribute__((constructor)) void nuisance_init(void) {
  std::vector<std::vector<LetterBackronym> > Letters;

  for (size_t i = 0; i < 8; ++i) {
    Letters.push_back(std::vector<LetterBackronym>());
  }

  Letters[0].push_back(LetterBackronym(2, "Neutrino"));
  Letters[0].push_back(LetterBackronym(3, "NUIsance", 0.2));

  Letters[2].push_back(LetterBackronym(1, "Interaction"));

  Letters[3].push_back(LetterBackronym(1, "Systematics"));
  Letters[3].push_back(LetterBackronym(
      1, "Synthesiser", 0.2, "Playing on the comparisons you want to see"));

  Letters[4].push_back(LetterBackronym(2, "ANalyser"));
  Letters[4].push_back(LetterBackronym(1, "Aggregating", 0.5));
  Letters[4].push_back(LetterBackronym(3, "from A-Neutrino sCattering", 1,
                                       "You can always find a frame"));

  Letters[5].push_back(
      LetterBackronym(1, "New", 1, "The freshest comparisons"));

  Letters[6].push_back(LetterBackronym(1, "by Comparing"));
  Letters[6].push_back(LetterBackronym(1, "Constraints from"));

  Letters[7].push_back(LetterBackronym(1, "Experiments"));

  std::vector<std::string> TagLines;
  TagLines.push_back("Fit and compare.");

  std::stringstream back("");

  TRandom3 tr;
  tr.SetSeed();

  for (size_t i = 0; i < 8;) {
    LetterBackronym const &let = Letters[i][tr.Integer(Letters[i].size())];
    if (tr.Uniform() > let.ProbAccept) {
      continue;
    }
    back << let.Backkie << " ";
    i += let.NUsed;
    if (let.TagLine.length()) {
      TagLines.push_back(let.TagLine);
    }
  }

  std::string Name = "Nuisance";
  std::string TagL = TagLines[tr.Integer(TagLines.size())];

  std::vector<std::pair<std::string, std::pair<std::string, std::string> > >
      OneBlob;

  OneBlob.push_back(
      std::make_pair("NUISANCE", std::make_pair("", "FiXing your Neutrinos")));

  if (tr.Uniform() < 0.01) {
    std::pair<std::string, std::pair<std::string, std::string> > const &blob =
        OneBlob[tr.Integer(OneBlob.size())];
    Name = blob.first;
    back.str("");
    back << blob.second.first;
    TagL = blob.second.second;
  }

  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
               "%%%%%%%%%%%%%%%"
               "%%"
            << std::endl
            << "%%  Welcome to " << Name << ": \033[5m" << back.str()
            << "\033[0m-- " << TagL << std::endl
            << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
               "%%%%%%%%%%%%%%%"
               "%%"
            << std::endl;
}

BaseFitEvt::BaseFitEvt() {
#ifdef __NEUT_ENABLED__
  fNeutVect = NULL;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = NULL;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = NULL;
#endif

  fXVar = 0.0;
  fYVar = 0.0;
  fZVar = 0.0;
  Mode = 0;
  E = 0.0;
  Weight = 0.0;
  Signal = false;
  Index = -1;
  BinIndex = -1;

  dial_coeff = NULL;
  fSplineRead = NULL;
};

BaseFitEvt::~BaseFitEvt(){};

BaseFitEvt::BaseFitEvt(const BaseFitEvt *obj) {
  fXVar = obj->fXVar;
  fYVar = obj->fYVar;
  fZVar = obj->fZVar;
  this->Mode = obj->Mode;
  this->E = obj->E;
  this->Weight = obj->Weight;
  this->Signal = obj->Signal;
  this->Index = obj->Index;
  this->BinIndex = obj->Index;

#ifdef __NEUT_ENABLED__
  fNeutVect = obj->fNeutVect;
#endif

#ifdef __NUWRO_ENABLED__
  fNuwroEvent = obj->fNuwroEvent;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif

  // Delete own elements
  if (this->ndial_coeff > 0){
    this->ndial_coeff = 0;
    delete this->dial_coeff;
  }
  
  if (obj->ndial_coeff > 0){
    ndial_coeff = obj->ndial_coeff;
    dial_coeff = new double[ndial_coeff];

    for (int i = 0; i < ndial_coeff; i++){
      this->dial_coeff[i] = obj->dial_coeff[i];
    }
  }
};

void BaseFitEvt::ResetDialCoeff(){
  if (this->ndial_coeff > 0){
    this->ndial_coeff = 0;
    delete this->dial_coeff;
  }
}
void BaseFitEvt::CreateDialCoeff(int n){
  ResetDialCoeff();

  ndial_coeff = n;
  dial_coeff = new double[ndial_coeff];
}

void BaseFitEvt::AddSplineCoeffToTree(TTree* tn){
  tn->Branch("NCoeff",&ndial_coeff,"NCoeff/I");
  tn->Branch("DialCoeff", dial_coeff, "DialCoeff[NCoeff]/D");
}

void BaseFitEvt::SetSplineCoeffAddress(TTree* tn){

  tn->SetBranchAddress("NCoeff",&ndial_coeff);

  tn->GetEntry(0);
  CreateDialCoeff(ndial_coeff);
  
  tn->SetBranchAddress("DialCoeff",dial_coeff);
  
}

void BaseFitEvt::ResetWeight(){
  InputWeight = 1.0;
  // Reset Spline Reader
}

void BaseFitEvt::FillCoeff(double* vals){
  for (int i = 0; i < ndial_coeff; i++){
    dial_coeff[i] = vals[i];
  }
}

int BaseFitEvt::GetNCoeff(){
  return ndial_coeff;
}

double BaseFitEvt::GetCoeff(int i){
  return dial_coeff[i];
}
