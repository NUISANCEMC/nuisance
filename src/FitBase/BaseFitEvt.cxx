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


#include <ctime>
#include <cstdlib>
#include <cmath>

#include <sstream>

#include "TRandom3.h"

#include "BaseFitEvt.h"


struct LetterBackronym {
  LetterBackronym(size_t n, std::string const &b, float p=1.0, std::string const &t=""){
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

  std::vector< std::vector<LetterBackronym> > Letters;

  for(size_t i = 0; i < 8; ++i){
    Letters.push_back(std::vector<LetterBackronym>());
  }

  Letters[0].push_back(LetterBackronym(2,"Neutrino"));
  Letters[0].push_back(LetterBackronym(3,"NUIsance", 0.2));

  Letters[2].push_back(LetterBackronym(1,"Interaction"));

  Letters[3].push_back(LetterBackronym(1,"Systematics"));
  Letters[3].push_back(LetterBackronym(1,"Synthesiser", 0.2, "Playing on the comparisons you want to see"));


  Letters[4].push_back(LetterBackronym(2,"ANalyser"));
  Letters[4].push_back(LetterBackronym(1,"Aggregating", 0.5));
  Letters[4].push_back(LetterBackronym(3,"from A-Neutrino sCattering", 1, "You can always find a frame"));

  Letters[5].push_back(LetterBackronym(1,"New", 1, "The freshest comparisons"));

  Letters[6].push_back(LetterBackronym(1,"by Comparing"));
  Letters[6].push_back(LetterBackronym(1,"Constraints from"));

  Letters[7].push_back(LetterBackronym(1,"Experiments"));


  std::vector<std::string> TagLines;
  TagLines.push_back("Fit and compare.");


  std::stringstream back("");

  TRandom3 tr;
  tr.SetSeed();

  for(size_t i = 0; i < 8;){
    LetterBackronym const& let = Letters[i][tr.Integer(Letters[i].size())];
    if(tr.Uniform() > let.ProbAccept){
      continue;
    }
    back << let.Backkie << " ";
    i += let.NUsed;
    if(let.TagLine.length()){
      TagLines.push_back(let.TagLine);
    }
  }
  std::cout <<
"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
"%%" << std::endl
  << "%%  Welcome to Nuisance: " << back.str() << "-- "
  << TagLines[tr.Integer(TagLines.size())] << std::endl <<
"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
"%%" << std::endl;
}

BaseFitEvt::BaseFitEvt(){

#ifdef __NEUT_ENABLED__
  neut_event  = NULL;
#endif

#ifdef __NUWRO_ENABLED__
  nuwro_event = NULL;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = NULL;
#endif

  X_VAR = 0.0;
  Y_VAR = 0.0;
  Z_VAR = 0.0;
  Mode = 0;
  E = 0.0;
  Weight = 0.0;
  Signal = false;
  Index = -1;
  BinIndex = -1;

  dial_coeff = NULL;
};

BaseFitEvt::~BaseFitEvt(){
};


BaseFitEvt::BaseFitEvt(const BaseFitEvt* obj){

  this->X_VAR    = obj->X_VAR;
  this->Y_VAR    = obj->Y_VAR;
  this->Z_VAR    = obj->Z_VAR;
  this->Mode     = obj->Mode;
  this->E    = obj->E;
  this->Weight   = obj->Weight;
  this->Signal   = obj->Signal;
  this->Index    = obj->Index;
  this->BinIndex = obj->Index;

#ifdef __NEUT_ENABLED__
  neut_event  = obj->neut_event;
#endif

#ifdef __NUWRO_ENABLED__
  nuwro_event = obj->nuwro_event;
#endif

#ifdef __GENIE_ENABLED__
  genie_event = obj->genie_event;
#endif

  if (obj->dial_coeff){
    if (obj->dial_coeff->GetSize() > 0){
      dial_coeff = new TArrayD(*obj->dial_coeff);
    }
  }
};
