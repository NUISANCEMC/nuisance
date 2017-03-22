#include "Spline.h"
using namespace SplineUtils;

// Setup Functions
// ----------------------------------------------
Spline::Spline(std::string splname, std::string form, std::string points) {

  // Save Definition
  fName = splname;
  fForm = form;
  fPoints = points;
// functor = NULL;
  minimizer = NULL;

  // Run Checks
  // if ((UInt_t)fNDim != fSplineNames.size()) {
  //   ERR(FTL) << "Spline Dim:Names mismatch!" << std::endl;
  //   throw;
  // }

  // Setup Min Max for each Parameter
  fSplitNames = GeneralUtils::ParseToStr(splname, ";");
  std::vector< std::vector<double> > gridvals = SplineUtils::GetSplitDialPoints(fPoints);

  for (size_t i = 0; i < fSplitNames.size(); i++) {

    for (size_t j = 0; j < gridvals.size(); j++) {
      if (i == 0) fXScan.push_back(gridvals[j][0]);
      if (i == 1) fXScan.push_back(gridvals[j][1]);
    }

    double xmin = 9999.9;
    double xmax = -9999.9;
    for (size_t j = 0; j < gridvals.size(); j++) {
      if (gridvals[j][i] < xmin) xmin = gridvals[j][i];
      if (gridvals[j][i] > xmax) xmax = gridvals[j][i];
    }

    fVal.push_back(0.0);
    fValMin.push_back(xmin);
    fValMax.push_back(xmax);

    // Define TSpline3 1D iterators here
    if (i == 0) {
      iter_low = fXScan.begin();
      iter_high = fXScan.begin();
      iter_high++;
      off = 0;
    }
  }

  // Set form from list
  if      (!fForm.compare("1DPol1")) { Setup( k1DPol1, 1, 2 ); }
  else if (!fForm.compare("1DPol2")) { Setup( k1DPol2, 1, 3 ); }
  else if (!fForm.compare("1DPol3")) { Setup( k1DPol3, 1, 4 ); }
  else if (!fForm.compare("1DPol4")) { Setup( k1DPol4, 1, 5 ); }
  else if (!fForm.compare("1DPol5")) { Setup( k1DPol5, 1, 6 ); }
  else if (!fForm.compare("1DPol6")) { Setup( k1DPol6, 1, 7 ); }
  else if (!fForm.compare("1DTSpline3")) { Setup( k1DTSpline3, 1, fXScan.size() * 4 ); }
  else if (!fForm.compare("2DPol6")) { Setup( k2DPol6, 2, 28 ); }
  else if (!fForm.compare("2DGaus")) { Setup( k2DGaus, 2, 8 ); }
  else if (!fForm.compare("2DTSpline3")) { Setup (k2DTSpline3, 2, fXScan.size() * fYScan.size() * 8); }
  else {
    ERR(FTL) << "Unknown spline form : " << fForm << std::endl;
    throw;
  }

  std::cout << "Setup Spline " << fForm << " = " << fType << " " <<  fNPar << std::endl;
};


void Spline::Setup(int type, int ndim, int npar) {
  fType = type;
  fNDim = ndim;
  fNPar = npar;
}



// Reconfigure Functions
// ----------------------------------------------
void Spline::Reconfigure(float x, int index) {
  // std::cout << "Reconfigured spline : " << fName << " : " << fForm << " to be " << x << " " << index << std::endl;
  fVal[index] = x;
  fOutsideLimits = false;

  if (fVal[index] > fValMax[index]) fVal[index] = fValMax[index];
  if (fVal[index] < fValMin[index]) fVal[index] = fValMin[index];
  // std::cout << "Set at edge = " << fVal[index] << " " << index << std::endl;
}

void Spline::Reconfigure(std::string name, float x) {
  for (size_t i = 0; i < fSplitNames.size(); i++) {
    // std::cout << "-> Comparing in spline " << name << " to " << fSplitNames[i] << " = " << !fSplitNames[i].compare(name.c_str()) << std::endl;

    if (!fSplitNames[i].compare(name.c_str())) {
      // std::cout << "-> Reconfigured spline  : " << fSplitNames[i] << " " << name << " to be " << x << " " << i << std::endl;
      Reconfigure(x, i);
    }
  }

}


// Evaluation Functions
// ----------------------------------------------
double Spline::operator()(const Double_t* x, const Double_t* par) const {

  Float_t* tempx = new Float_t[fNDim];
  for (size_t i = 0; i < (UInt_t)fNDim; i++) {
    tempx[i] = x[i];
  }

  Float_t* tempp = new Float_t[fNPar];
  for (size_t i = 0; i < (UInt_t)fNPar; i++) {
    tempp[i] = par[i];
  }

  float val = DoEval(tempx, tempp);
  delete tempp;
  delete tempx;

  if (val < 0.0) val = 0.0;
  return val;
}


float Spline::operator()(const Float_t* x, const Float_t* par) const {
  float val = DoEval(x, par);
  if (val < 0.0) val = 0.0;
  return val;
}

// double Spline::operator()(const Double_t* x, const Double_t* y, const Double_t* par) const {
// Float_t temp[2];
// temp[0] = x[0];
// temp[1] = y[0];

// float val = DoEval(temp, par);
// if (val < 0.0) val = 0.0;
// return val;
// }



float Spline::DoEval(const Float_t* x, const Float_t* par) const {

  // Setup current fX to value
  for (size_t i = 0; i < (UInt_t) fNDim; i++) {
    fVal[i] = x[i];
    if (fVal[i] > fValMax[i]) fVal[i] = fValMax[i];
    if (fVal[i] < fValMin[i]) fVal[i] = fValMin[i];
    // std::cout << "Set " << i << " fVal to " << x[i] << std::endl;
  }

  double w = DoEval(&par[0], false);

  if (w < 0.0) w = 0.0;

  // Now evaluate spline how FitWeight will do it.
  return w;
}


float Spline::DoEval(const Float_t* par, bool checkresponse) const {

  if (!par)
    return 1.0;

  // std::cout << "Spline::DoEval = " << par << std::endl;
  // Check response
  if (checkresponse) {
    bool hasresponse = false;
    for (int i = 0; i < fNPar; i++) {
      if (par[i] != 0.0) {
        hasresponse = true;
        break;
      }
    }

    if (!hasresponse) {
      // std::cout << "No Response" << std::endl;
      return 1.0;
    }
  }

  // std::cout << "TYpe = " << fType << " "<< fForm << std::endl;
  // Now evaluate spline
  switch (fType) {
  case k1DPol1:     { return Spline1DPol1(par); }
  case k1DPol2:     { return Spline1DPol2(par); }
  case k1DPol3:     { return Spline1DPol3(par); }
  case k1DPol4:     { return Spline1DPol4(par); }
  case k1DPol5:     { return Spline1DPol5(par); }
  case k1DPol6:     { return Spline1DPol6(par); }
  case k1DTSpline3: { return Spline1DTSpline3(par); }
  case k2DPol6:     { return Spline2DPol(par, 6); }
  case k2DGaus:     { return Spline2DGaus(par); }
  case k2DTSpline3: { return Spline2DTSpline3(par); }
  }

  // Return nominal weight
  return 1.0;
};



// Spline Functions
// ----------------------------------------------

// 1D Functions
// ----------------------------------------------
float Spline::Spline1DPol1(const Float_t* par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp;
};

float Spline::Spline1DPol2(const Float_t* par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp + par[2] * xp * xp;
};

float Spline::Spline1DPol3(const Float_t* par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp;
};

float Spline::Spline1DPol4(const Float_t* par) const {
  float xp = fVal[0];
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp);
};

float Spline::Spline1DPol5(const Float_t* par) const {
  float xp = fVal[0];
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp + par[5] * xp * xp * xp * xp * xp);
};

float Spline::Spline1DPol6(const Float_t* par) const {
  float xp = fVal[0];

  float w = 0.0;
  // std::cout << "Pol Eval " << std::endl;
  for (int i = fNPar - 1; i > 0; i--) {
    w = xp * (par[0 + i] + w);
  }
  w += par[0];
  return w;
};





float Spline::GetMonomial(int p) const {

  // // Copied From ROOT
  // Int_t    i   = 0;
  // Double_t p1  = 1;
  // Double_t p2  = 0;
  // Double_t p3  = 0;
  // Double_t r   = 0;

  // switch (p) {
  // case 1:
  //   r = 1;
  //   break;
  // case 2:
  //   r =  x;
  //   break;
  // default:
  //   p2 = x;
  //   for (i = 3; i <= p; i++) {
  //     p3 = p2 * x;
  //     p1 = p2;
  //     p2 = p3;
  //   }
  //   r = p3;
  // }

  return 0.0;
}

float Spline::Spline2DTSpline3(const Float_t* par) const {

  // Find matching point
  std::vector<float>::iterator iter_low_x   = fXScan.begin();
  std::vector<float>::iterator iter_high_x  = fXScan.begin();
  std::vector<float>::iterator iter_low_y   = fYScan.begin();
  std::vector<float>::iterator iter_high_y  = fYScan.begin();
  iter_high_x++;
  iter_high_y++;

  off = 0;
  fX = fVal[0];
  fY = fVal[1];

  while ( (iter_high_x != fXScan.end() and
           iter_high_y != fYScan.end()) and
          (fX < (*iter_low_y) or
           fX >= (*iter_high) or
           fY < (*iter_low_y) or
           fY >= (*iter_low_y)) ) {
    off += 9;
    iter_low_x++;
    iter_high_x++;

    if (iter_high_x == fXScan.end()) {
      iter_low_x  =  fXScan.begin();
      iter_high_x =  fXScan.begin();
      iter_low_y++;
      iter_high_y++;
      std::cout << "Skipping to next tspline 3 rung " << *iter_low_y << std::endl;
    }
  }

  std::cout << "Evaluting TSpline3 at " << fX << " " << (*iter_low_x) << " " << fY << " " << (*iter_low_y) <<  std::endl;
  // sleep(1.0);
  float dx   = fX - (*iter_low_x);
  float dy   = fY - (*iter_low_y);

  float weight = (par[off] + dx * (par[off + 1] + dx * (par[off + 2] + dx * par[off + 3])));
  float weight2 = (par[off + 4] + dy * (par[off + 5] + dy * (par[off + 6] + dy * par[off + 7])));

  return weight * weight2 * par[off + 8];
};





float Spline::Spline2DPol(const Float_t* par, int n) const {

  // float wx = 2.0 * (fVal[0] - fValMin[0] - (fValMax[0] - fValMin[0]) / 2.0) / (fValMax[0] - fValMin[0]);
  // float wy = 2.0 * (fVal[1] - fValMin[1] - (fValMax[1] - fValMin[1]) / 2.0) / (fValMax[1] - fValMin[1]);

  float wx = (fVal[0] - fValMin[0]) / (fValMax[0] - fValMin[0]);
  float wy = (fVal[1] - fValMin[1]) / (fValMax[1] - fValMin[1]);


  int count = 0;
  bool speak = false;

  // float w = 0.0;
  // // std::cout << "Pol Eval " << std::endl;
  // for (int i = 7 - 1; i > 0; i--) {
  //   w = xp * (par[0 + i] + w);
  // }
  // w += par[0];

  // float w2 = 0.0;
  // // std::cout << "Pol Eval " << std::endl;
  // for (int i = 13 - 1; i > 0; i--) {
  //   w2 = xp * (par[7 + i] + w2);
  // }

  // return w1 * w2;






  // for (int i = 0; i < n; i++){

  //   if (i == 0)

  // }
// float w = 0.0;
//  w += par[count++];
//  w += par[count++] * wx;
//  w += par[count++] * wy;

//  w += par[count++] * wx * wy; // / 10.0;
//  w += par[count++] * wx * wx; // / 10.0;
//  w += par[count++] * wy * wy; // / 10.0;

//  w += par[count++] * wx * wx * wy; // / 100.0;
//  w += par[count++] * wx * wy * wy; // / 100.0;
//  w += par[count++] * wx * wx * wx; // / 100.0;
//  w += par[count++] * wy * wy * wy; // / 100.0;


//    w += par[count++] * wx * wx * wx * wx; // / 100.0;
//    w += par[count++] * wx * wx * wx * wy; // / 100.0;
//    w += par[count++] * wx * wx * wy * wy; // / 100.0;
//    w += par[count++] * wx * wy * wy * wy; // / 100.0;
//    w += par[count++] * wy * wy * wy * wy; // / 100.0;

//    w += par[count++] * wx * wx * wx * wx * wx; // / 1000.0;
//    w += par[count++] * wx * wx * wx * wx * wy; // / 1000.0;
//    w += par[count++] * wx * wx * wx * wy * wy; // / 1000.0;
//    w += par[count++] * wx * wx * wy * wy * wy; // / 1000.0;
//    w += par[count++] * wx * wy * wy * wy * wy; // / 1000.0;
//    w += par[count++] * wy * wy * wy * wy * wy; // / 1000.0;

//      w += par[count++] * wx * wx * wx * wx * wx * wx; // / 10000.0;
//      w += par[count++] * wx * wx * wx * wx * wx * wy; // / 10000.0;
//      w += par[count++] * wx * wx * wx * wx * wy * wy; // / 10000.0;
//      w += par[count++] * wx * wx * wx * wy * wy * wy; // / 10000.0;
//      w += par[count++] * wx * wx * wy * wy * wy * wy; // / 10000.0;
//      w += par[count++] * wx * wy * wy * wy * wy * wy; // / 10000.0;
//      w += par[count++] * wy * wy * wy * wy * wy * wy; // / 10000.0;

  // float w = 0.0;
  // int power = 6;
  // for (int i = 0; i < power; i++){
  //   int xpower = power;
  //   int ypower = 0;
  //   while (ypower <= power){
  //     w += par[count++] * pow(wx,xpower) * pow(wy, ypower);
  //     xpower--;
  //     ypower++;
  //   }
  // }


  float w = 0.0;

  w += par[count++] * wx;
  w += par[count++] * wy;

  w += par[count++] * wx * wy; // / 10.0;
  w += par[count++] * wx * wx; // / 10.0;
  w += par[count++] * wy * wy; // / 10.0;

  w += par[count++] * wx * wx * wy; // / 100.0;
  w += par[count++] * wx * wy * wy; // / 100.0;
  w += par[count++] * wx * wx * wx; // / 100.0;
  w += par[count++] * wy * wy * wy; // / 100.0;


  w += par[count++] * wx * wx * wx * wx; // / 100.0;
  w += par[count++] * wx * wx * wx * wy; // / 100.0;
  w += par[count++] * wx * wx * wy * wy; // / 100.0;
  w += par[count++] * wx * wy * wy * wy; // / 100.0;
  w += par[count++] * wy * wy * wy * wy; // / 100.0;

  w += par[count++] * wx * wx * wx * wx * wx; // / 1000.0;
  w += par[count++] * wx * wx * wx * wx * wy; // / 1000.0;
  w += par[count++] * wx * wx * wx * wy * wy; // / 1000.0;
  w += par[count++] * wx * wx * wy * wy * wy; // / 1000.0;
  w += par[count++] * wx * wy * wy * wy * wy; // / 1000.0;
  w += par[count++] * wy * wy * wy * wy * wy; // / 1000.0;

  w += par[count++] * wx * wx * wx * wx * wx * wx; // / 10000.0;
  w += par[count++] * wx * wx * wx * wx * wx * wy; // / 10000.0;
  w += par[count++] * wx * wx * wx * wx * wy * wy; // / 10000.0;
  w += par[count++] * wx * wx * wx * wy * wy * wy; // / 10000.0;
  w += par[count++] * wx * wx * wy * wy * wy * wy; // / 10000.0;
  w += par[count++] * wx * wy * wy * wy * wy * wy; // / 10000.0;
  w += par[count++] * wy * wy * wy * wy * wy * wy; // / 10000.0;



  if (wx != 0.0 and speak) {
    for (int i = 0; i < count; i++) {
      std::cout << "Evaluated " << fName << " spline coeff " << i << " = " << par[i] << std::endl;
    }
    std::cout << "End Weight = " << w << " " << wx << " " << wy << std::endl;
    // sleep(1);
  }
  // Add up all coefficients.
  // for (int i = 0; i <= n; i++) {
  // wx += pow(x, i) * par[count++];
  // }
  // for (int i = 0; i <= n; i++) {
  // wy += pow(y, i) * par[count++];
  // }

  return w;
}

float Spline::Spline2DGaus(const Float_t* par) const {

  double Norm = 5.0 + par[1] * 20.0;
  double Tilt = par[2] * 10.0; //vals[kPosTilt];
  double Pq0  = 1.0 + par[3] * 1.0; //vals[kPosPq0];
  double Wq0  = 0.5 + par[4] * 1.0; //vals[kPosWq0];
  double Pq3  = 1.0 + par[5] * 1.0; //vals[kPosPq3];
  double Wq3  = 0.5 + par[6] * 1.0; //vals[kPosWq3];
  double q0 = (fVal[0] - fValMin[0]) / (fValMax[0] - fValMin[0]);
  double q3 = (fVal[1] - fValMin[1]) / (fValMax[1] - fValMin[1]);

  //  double w = Norm;
  //  w *= TMath::Gaus(q0, Pq0, Wq0);
  //  w *= TMath::Gaus(q3, Pq3*sin(Tilt), Wq3);

  double a = cos(Tilt) * cos(Tilt) / (2 * Wq0 * Wq0);
  a += sin(Tilt) * sin(Tilt) / (2 * Wq3 * Wq3);

  double b = - sin(2 * Tilt) / (4 * Wq0 * Wq0);
  b += sin(2 * Tilt) / (4 * Wq3 * Wq3);

  double c = sin(Tilt) * sin(Tilt) / (2 * Wq0 * Wq0);
  c += cos(Tilt) * cos(Tilt) / (2 * Wq3 * Wq3);

  double w = Norm;
  w *= exp(-a  * (q0 - Pq0) * (q0 - Pq0));
  w *= exp(+2.0 * b * (q0 - Pq0) * (q3 - Pq3));
  w *= exp(-c  * (q3 - Pq3) * (q3 - Pq3));

  // Norm = par[7];
  // Tilt = par[8]*50.0;//vals[kPosTilt];
  // Pq0  = par[9]*10.0;//vals[kPosPq0];
  // Wq0  = par[10]*10.0;//vals[kPosWq0];
  // Pq3  = par[11]*10.0; //vals[kPosPq3];
  // Wq3  = par[12]*10.0; //vals[kPosWq3];

  // //  double w = Norm;
  // //  w *= TMath::Gaus(q0, Pq0, Wq0);
  // //  w *= TMath::Gaus(q3, Pq3*sin(Tilt), Wq3);

  // a = cos(Tilt)*cos(Tilt) / (2*Wq0*Wq0);
  // a += sin(Tilt)*sin(Tilt) / (2*Wq3*Wq3);

  // b = - sin(2*Tilt) / (4*Wq0*Wq0);
  // b += sin(2*Tilt) / (4*Wq3*Wq3);

  // c = sin(Tilt)*sin(Tilt) / (2*Wq0*Wq0);
  // c += cos(Tilt)*cos(Tilt) / (2*Wq3*Wq3);

  // double w2 = Norm;
  // w2 *= exp(-a  * (q0 - Pq0)*(q0 - Pq0));
  // w2 *= exp(+2.0*b * (q0 - Pq0)*(q3 - Pq3));
  // w2 *= exp(-c  * (q3 - Pq3)*(q3 - Pq3));


// std::cout << "GausWeight = " << w << std::endl;
  return w;
}



float Spline::Spline1DTSpline3(const Float_t* par) const {

  // Find matching point
  iter_low  = fXScan.begin();
  iter_high = fXScan.begin();
  iter_high++;
  off = 0;
  fX = fVal[0];

  while ( iter_high != fXScan.end() and
          (fX < (*iter_low) or fX >= (*iter_high)) ) {
    off += 4;
    iter_low++;
    iter_high++;
  }

  // std::cout << "Evaluting TSpline3 at " << fX << " " << (*iter_low) << std::endl;
  // sleep(1.0);
  float dx   = fX - (*iter_low);
  float weight = (par[off] + dx * (par[off + 1] + dx * (par[off + 2] + dx * par[off + 3])));

  return weight;
};


// 2D Functions
// ----------------------------------------------







