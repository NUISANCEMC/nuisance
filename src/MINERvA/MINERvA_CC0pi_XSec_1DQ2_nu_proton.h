#ifndef MINERVA_1DQ2_nu_proton_H_SEEN
#define MINERVA_1DQ2_nu_proton_H_SEEN

#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>

// ROOT includes
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TDecompSVD.h>


#ifdef __NEUT_ENABLED__
// NEUT includes
#include "neutvect.h"
#include "neutpart.h"
#include "neutfsipart.h"
#include "neutfsivert.h"
#endif

#ifdef __T2KREW_ENABLED__
// T2KReWeight includes
#include "T2KReWeight.h"
#include "T2KNeutReWeight.h"
#include "T2KNIWGReWeight.h"
#endif

// Fit Includes
#include "Measurement1D.h"
#include "FitUtils.h"
using namespace std;

class MINERvA_CC0pi_XSec_1DQ2_nu_proton : public Measurement1D {
public:

  MINERvA_CC0pi_XSec_1DQ2_nu_proton(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile);
  virtual ~MINERvA_CC0pi_XSec_1DQ2_nu_proton() {};

  bool isSignal(FitEvent* event);
  void FillEventVariables(FitEvent *event);

 private:

  Double_t Coplanar;
  Double_t totIntFlux;


  TH1D* muonHist;

  TH1D* bgPDG;
  TH1D* mcPDG;

  TH1D* bgHist;
  TH1D* bgFine;


  // Event Variables
  double  Enu  ;
  double  CosThetaMu  ;
  double ThetaMu;
  bool bad_particle;
  double  Emu  ;
  double  Tmu  ;
  double  Q2mu  ;
  double  q3  ;
  double  Q2true  ;
  double  Tp  ;
  double  Ep  ;
  double  Q2p  ;
  double Pmuon;
  int Mode;
  int signal;
  int Np;
  int Nn;
  double Weight;
  int FSI;

  bool muon_found;
  bool proton_found;
  double pT;

};

#endif
