#ifndef weightRPA_h
#define weightRPA_h

#include <fstream>  //ifstream
#include <iostream> //cout

#include <TString.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TFile.h>
#include "math.h"
#include "assert.h"
//For Compatibility with ROOT compiler
//uncomment the following:
//#define ROOT

/*!
 *  Code example to extract the RPA effect central weight
 *  and its uncertainties from the prepared files.
 *  Heidi Schellman (Oregon State) and Rik Gran (Minnesota Duluth)
 *  for use in MINERvA experiment analysis
 *  must compile with the ROOT libraries
 *  g++ `root-config --glibs --cflags` -O3 weightRPAtest.cxx -o weightRPA
 

 *    The underlying model is from the IFIC Valencia group
 *    see (public) minerva docdb:12688 for the full physics discussion

 */


// NOTE UNITS ARE GEV in the calculation
// make sure you convert MeV to GeV before calling these functions


// Class for getting  RPA paramaters inputs from a given file
// Includes methods that return all five RPA weights at once
//   (is the most cpu efficient way to get them)
// Or return just the central value
//   (skipping the uncertainties code completely if only cv wanted)
// Or return each CV and uncertainty one at a time
//   (is the least cpu efficient, repeats some calculations 5 times)

class weightRPA {
public:
  //Constructor: Read in params from a filename
  weightRPA(const TString  f) { read(f); }    //Read in params from file
  
  TString  filename;
  TFile* fRPAratio;
  TH2D *hRPArelratio;
  TH2D *hRPAnonrelratio;
  TH1D *hQ2relratio;
  TH1D *hQ2nonrelratio;
  TArrayD *TADrpapolyrel;
  Double_t *rpapolyrel ;
  TArrayD *TADrpapolynonrel;
  Double_t *rpapolynonrel;
  static const int CENTRAL=0;
  static const int LOWQ2 = 1;
  static const int HIGHQ2 = 2;

  // true to take from histogram, false use parameterization
  static const bool Q2histORparam = true;  

  // MINERvA holds kinematics in MeV, but all these functions require GeV
  // So make sure you pass them in GeV.
  inline double getWeight(const double mc_q0, const double mc_q3, double * weights);  //in GeV
  inline double getWeight(const double mc_q0, const double mc_q3); //in GeV
  inline double getWeight(const double mc_q0, const double mc_q3, int type, int sign); //in GeV
  inline double getWeight(const double Q2); //in GeV^2
  inline double getWeightLowQ2(const double mc_q0, const double mc_q3, const int sign);
  inline double getWeightHighQ2(const double mc_q0, const double mc_q3, const int sign);
  inline double getWeightQ2(const double mc_Q2, const bool relORnonrel=true);
  //Initializer
  inline void read(const TString  f);
  
  // q0 and q3 in GeV, type = 1 for low Q2, 2 for high Q2, 0 for central
  //double getWeightInternal(const double mc_q0, const double mc_q3,int type, int sign);

  private:
  inline double getWeightInternal(const double mc_Q2);
  
  inline double getWeightInternal(const double mc_q0, const double mc_q3, const int type, const int sign);
  inline double getWeightInternal(const double mc_q0, const double mc_q3, double *weights=0);
  inline double getWeightQ2parameterization(const double mc_Q2, const bool relORnonrel);
  inline double getWeightQ2fromhistogram(const double mc_Q2, const bool relORnonrel);
  
  
};

void weightRPA::read(const TString  f)
//Read in the params doubles from a file
//argument: valid filename
{
  fRPAratio = TFile::Open(f,"READONLY");
  if (fRPAratio){
    hRPArelratio = (TH2D*)fRPAratio->Get("hrelratio");
    hRPAnonrelratio = (TH2D*)fRPAratio->Get("hnonrelratio");
    hQ2relratio = (TH1D*)fRPAratio->Get("hQ2relratio");
    hQ2nonrelratio = (TH1D*)fRPAratio->Get("hQ2nonrelratio");
    TADrpapolyrel = (TArrayD*)fRPAratio->Get("rpapolyrel");
    rpapolyrel = TADrpapolyrel->GetArray();
    TADrpapolynonrel = (TArrayD*)fRPAratio->Get("rpapolynonrel");
    rpapolynonrel = TADrpapolynonrel->GetArray();
    hRPArelratio->Print();
    std::cout << "have read in ratios from file " << f <<std::endl;
  } else {
    //File could not be read
    std::cout << "File could not be read" << std::endl;
  }
}


 

 
double weightRPA::getWeightInternal(const double mc_q0, const double mc_q3,  double *weights){
  assert(hRPArelratio);
  
  if (weights != 0){
    for (int i = 0; i < 5; i++){
      weights[i] = 1.0;
    }
  }
  
  // the construction here is that the histogram bins
  // line up in mev-sized steps e.g. from 0.018 to 0.019
  // and the value stored is the value at bin-center.
  
  // double gevmev = 0.001 ;
  const Double_t gevlimit = 3.; // upper limit for 2d
  Int_t rpamevlimit = gevlimit*1000.;
  //Double_t Q2gev = mc_Q2*gevmev*gevmev;
  Double_t Q2gev = mc_q3*mc_q3-mc_q0*mc_q0;
  Int_t q3bin = Int_t(mc_q3*1000.);
  Int_t q0bin = Int_t(mc_q0*1000.);
  if(mc_q0 >= gevlimit) q0bin = rpamevlimit - 1;
  if(mc_q3 >= gevlimit) q3bin = rpamevlimit - 1;
  
  // Nieves does not calculate anything below binding energy.
  // I don't know what GENIE does, but lets be soft about this.
  // Two things lurking here at once.
  // One, we are taking out a 10 MeV offset between GENIE and Valencia.
  // Second, we are protecting against being asked for a weight that is too low in q0.
  // It actually shouldn't happen for real GENIE events,
  // but this protection does something that doesn't suck, just in case.
  // you would see the artifact in a plot for sure, but better than writing 1.0.
 
  // CWret after talking to Rik in Nov 2018 at MINERvA CM
  // 2.12.8 sets this to 27 because change of Q definition: need to offset GENIE and Nieves Eb even more
#if __GENIE_VERSION__ >= 210
  Int_t q0offsetValenciaGENIE = 27;
#else 
  Int_t q0offsetValenciaGENIE = 10;
#endif


  if(mc_q0 < 0.018) q0bin = 18+q0offsetValenciaGENIE;
  Double_t thisrwtemp = hRPArelratio->GetBinContent(q3bin,q0bin-q0offsetValenciaGENIE);
  
  // now trap bogus entries.  Not sure why they happen, but set to 1.0 not 0.0
  if(thisrwtemp <= 0.001)thisrwtemp = 1.0;
  
  // events in genie but not in valencia should get a weight
  // related to a similar q0 from the bulk distribution.
  if(mc_q0 < 0.15 && thisrwtemp > 0.9){
    thisrwtemp = hRPArelratio->GetBinContent(q3bin+150, q0bin-q0offsetValenciaGENIE);
  }
  

  
  //Double_t *mypoly;
  //mypoly = rpapoly;
  
  if(Q2gev >= 9.0){
    thisrwtemp = 1.0;
  }
  else if(Q2gev > 3.0) {
    // hiding option, use the Q2 parameterization everywhere
    // } else if(Q2gev > 3.0 || rwRPAQ2) {
    // turn rwRPAQ2 all the way on to override the 2D histogram
    // illustrates the old-style Q2 suppression many folks still use.

    thisrwtemp = getWeightQ2(Q2gev,true);
    
    //    double powerQ2 = 1.0;
    //thisrwtemp = 0.0;
    //for(int ii=0; ii<10; ii++){
    //  thisrwtemp += rpapoly[ii]*powerQ2;
    //  powerQ2 *= Q2gev;
    //}
    //std::cout << "test temp " << thisrwtemp << " " << rpamypoly[2] << std::endl;
  }
  
  if(!(thisrwtemp >= 0.001 && thisrwtemp <= 2.0))thisrwtemp = 1.0;
  
  // hiding option, turn off the enhancement.
  //if(rwoffSRC && thisrwtemp > 1.0)thisrwtemp = 1.0;

  if (0 == weights) return thisrwtemp;
  // if this was called without passing an array,
  // the user didn't want us to calculate the +/- 1-sigma bounds
  // so the above line returned before even trying.

  weights[0] = thisrwtemp;
  
  //if (type == 0) return thisrwtemp;
  
  //if (type == 1) {
    // Construct the error bands on the low Q2 suppression.
  // Double_t thisrwSupP1 = 1.0;
  // Double_t thisrwSupM1 = 1.0;
  
  if( thisrwtemp < 1.0){
    // make the suppression stronger or weaker to muon capture uncertainty
    // rwRPAonesig is either +1 or -1, which is 0.25 (25%).
    // possible to be re-written to produce 2 and 3 sigma.
    
    weights[1] = thisrwtemp + 1.0 * (0.25)*(1.0 - thisrwtemp);
    weights[2] = thisrwtemp - 1.0 * (0.25)*(1.0 - thisrwtemp);
    
   
    
  }
  else{
    weights[1] = thisrwtemp;
    weights[2] = thisrwtemp;
  }

  //std::cout << "check " << thisrwtemp << " " << weights[1] << " " << weights[2] << std::endl;

  
    
    
  // Construct the rest of the error bands on the low Q2 suppression.
  // this involves getting the weight from the non-relativistic ratio
  
  //if (type == 2){
    
  Double_t thisrwEnhP1 = 1.0;
  Double_t thisrwEnhM1 = 1.0;
  
  // make enhancement stronger or weaker to Federico Sanchez uncertainty
  // this does NOT mean two sigma, its overloading the option.
  Double_t thisrwextreme = hRPAnonrelratio->GetBinContent(q3bin,q0bin-q0offsetValenciaGENIE);
  // now trap bogus entries.  Not sure why they happen, but set to 1.0 not 0.0
  if(thisrwextreme <= 0.001)thisrwextreme = 1.0;
  
  if(mc_q0 < 0.15 && thisrwextreme > 0.9){
    thisrwextreme = hRPAnonrelratio->GetBinContent(q3bin+150, q0bin-q0offsetValenciaGENIE);
  }
  
  //std::cout << "ext " << thisrwextreme << " " << thisrwtemp << std::endl;

  // get the same for the Q2 dependent thing,
  // but from the nonrelativistic polynomial
  
  if(Q2gev >= 9.0){
    thisrwextreme = 1.0;
  }
  else if(Q2gev > 3.0 ) {
    thisrwextreme = getWeightQ2(Q2gev,false);
    //double powerQ2 = 1.0;
    //thisrwextreme = 0.0;
    //for(int ii=0; ii<10; ii++){
    //  thisrwextreme += rpapolynonrel[ii]*powerQ2;
    //  powerQ2 *= Q2gev;
    //}
    //std::cout << "test extreme " << thisrwextreme << " " << mypolynonrel[2] << std::endl;
  }

  if(!(thisrwextreme >= 0.001 && thisrwextreme <= 2.0))thisrwextreme = 1.0;

  //std::cout << "test extreme " << Q2gev << " " << thisrwextreme << " " << thisrwtemp << std::endl;
  
  Double_t RelToNonRel = 0.6;
  
  // based on some distance between central value and extreme
  thisrwEnhP1 = thisrwtemp + RelToNonRel * (thisrwextreme-thisrwtemp);
  Double_t thisrwEnhP1max = thisrwextreme;
  
  if(Q2gev < 0.9)thisrwEnhP1 += 1.5*(0.9 - Q2gev)*(thisrwEnhP1max - thisrwEnhP1);
  // sanity check, don't let the upper error bound go above the nonrel limit.
  if(thisrwEnhP1 > thisrwEnhP1max)thisrwEnhP1 = thisrwEnhP1max;
  // don't let positive error bound be closer than 3% above the central value
  // will happen at very high Q2 and very close to Q2 = 0
  if(thisrwEnhP1 < thisrwtemp + 0.03)thisrwEnhP1 = thisrwtemp + 0.03;
  
  thisrwEnhM1 = thisrwtemp - RelToNonRel * (thisrwextreme-thisrwtemp);
  // don't let negative error bound be closer than 3% below the central value
  if(thisrwEnhM1 > thisrwtemp - 0.03)thisrwEnhM1 = thisrwtemp - 0.03;
  // even still, don't let the lower error bound go below 1.0 at high-ish Q2
  if(Q2gev > 1.0 && thisrwEnhM1 < 1.0)thisrwEnhM1 = 1.0;
  
  // whew.  so now return the main weight
  // and return the array of all five weights in some array
  // thisrwtemp, thisrwSupP1, thisrwSupM1, thisrwEnhP1, thisrwEnhM1
  
  //if (sign == 1) return thisrwEnhP1;
  //if (sign == -1) return thisrwEnhM1;
    
  weights[3] = thisrwEnhP1;
  weights[4] = thisrwEnhM1;

  // still return the central value
  return thisrwtemp;
  
}


double weightRPA::getWeight(const double mc_q0, const double mc_q3){

  return getWeightInternal(mc_q0, mc_q3);

}

double weightRPA::getWeight(const double mc_q0, const double mc_q3, double *weights){

  return getWeightInternal(mc_q0, mc_q3, weights);

}

double weightRPA::getWeight(const double mc_q0, const double mc_q3, int type, int sign){

  return getWeightInternal(mc_q0, mc_q3, type, sign);

}

double weightRPA::getWeight(const double mc_Q2){

  return getWeightQ2(mc_Q2);
  
}

double weightRPA::getWeightInternal(const double mc_q0, const double mc_q3, int type, int sign){

   double weights[5] = {1., 1., 1., 1., 1.};
   double cv = getWeightInternal(mc_q0, mc_q3, weights);

   if(type==0)return cv;
   else if(type==weightRPA::LOWQ2 && sign == 1)return weights[1];
   else if(type==weightRPA::LOWQ2 && sign == -1)return weights[2];
   else if(type==weightRPA::HIGHQ2 && sign == 1)return weights[3];
   else if(type==weightRPA::HIGHQ2 && sign == -1)return weights[4];
   //else {
   //  // should never happen?  Bork ?
   //  return cv;  //?
   //}
   
   return cv;
}

double weightRPA::getWeightQ2(const double mc_Q2, const bool relORnonrel){

  if(mc_Q2 < 0.0)return 1.0;  // this is Q2 actually, not sure hw
  if(mc_Q2 > 9.0)return 1.0;

  // this function needs to know two options.
  // does user want rel (cv) or nonrel
  // does user want to use the histogram or parameterization

  if(Q2histORparam)return getWeightQ2fromhistogram(mc_Q2, relORnonrel);
  else return getWeightQ2parameterization(mc_Q2, relORnonrel);
    
}

double weightRPA::getWeightQ2parameterization(const double mc_Q2, const bool relORnonrel){

  if(mc_Q2 < 0.0)return 1.0;
  if(mc_Q2 > 9.0)return 1.0;
  
    // this one returns just the polynomial Q2 version
  // for special tests.  Poor answer for baseline MINERvA QE events.
  // No uncertainty assigned to this usecase at this time.
  //double gevmev = 0.001;  // minerva sends in MeV.
  double Q2gev = mc_Q2;
  double powerQ2 = 1.0;
  double thisrwtemp = 0.0;
  thisrwtemp = 0.0;
  for(int ii=0; ii<10; ii++){
    if(relORnonrel)thisrwtemp += rpapolyrel[ii]*powerQ2;
    else thisrwtemp += rpapolynonrel[ii]*powerQ2;
    powerQ2 *= Q2gev;
  }
  return thisrwtemp;

  
}

double weightRPA::getWeightQ2fromhistogram(const double mc_Q2, const bool relORnonrel){

  if(mc_Q2 < 0.0)return 1.0;
  if(mc_Q2 > 9.0) return 1.0;

  if(relORnonrel)return hQ2relratio->GetBinContent( hQ2relratio->FindBin(mc_Q2) );
  else return hQ2nonrelratio->GetBinContent( hQ2nonrelratio->FindBin(mc_Q2) );

  // interpolation might be overkill for such a finely binned histogram, 0.01%
  // but the extra cpu cycles maybe small.
  // save it here for some future use.
  //if(relORnonrel)return  hQ2relratio->Interpolate(mc_Q2);
  //else return  hQ2nonrelratio->Interpolate(mc_Q2);
  
}


double weightRPA::getWeightLowQ2(const double mc_q0, const double mc_q3, int sign){
  return getWeightInternal(mc_q0,mc_q3,weightRPA::LOWQ2,sign);
}

double weightRPA::getWeightHighQ2(const double mc_q0, const double mc_q3, int sign){
  return getWeightInternal(mc_q0,mc_q3,weightRPA::HIGHQ2,sign);
}


#endif

