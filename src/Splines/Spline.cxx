#include "Spline.h"


Spline::Spline(std::string splname, std::string form,
               std::vector<double> x) {

  // Initial setup
  fSplineNames = GeneralUtils::ParseToStr(splname, ":");
  fName = splname;
  fForm = form;

  fXScan.clear();
  for (size_t i = 0; i < x.size(); i++){
    fXScan.push_back(x[i]);
  }

  fX = 0.0;

  // Set X Min/Max from scan points
  fXMax = -99999;
  fXMin = 99999;
  for (size_t i = 0; i < fXScan.size(); i++) {
    if (fXScan[i] > fXMax) fXMax = fXScan[i];
    if (fXScan[i] < fXMin) fXMin = fXScan[i];
  }

  // Setup iters
  iter_low = fXScan.begin();
  iter_high = fXScan.begin();
  iter_high++;
  off = 0;

  // Set form from list
  if      (!fForm.compare("1DPol1")) { Setup( k1DPol1, 1, 2 ); }
  else if (!fForm.compare("1DPol2")) { Setup( k1DPol2, 1, 3 ); }
  else if (!fForm.compare("1DPol3")) { Setup( k1DPol3, 1, 4 ); }
  else if (!fForm.compare("1DPol4")) { Setup( k1DPol4, 1, 5 ); }
  else if (!fForm.compare("1DPol5")) { Setup( k1DPol5, 1, 6 ); }
  else if (!fForm.compare("1DPol6")) { Setup( k1DPol6, 1, 7 ); }
  else if (!fForm.compare("1DTSpline3")) { Setup( k1DTSpline3, 1, fXScan.size() * 4 ); }
  else {
    ERR(FTL) << "Unknown spline form : " << fForm << std::endl;
    throw;
  }

  // Run Checks
  if (fNDim != fSplineNames.size()) {
    ERR(FTL) << "Spline Dim:Names mismatch!" << std::endl;
    throw;
  }

};


void Spline::Setup(int type, int ndim, int npar) {
  fType = type;
  fNDim = ndim;
  fNPar = npar;
}


double Spline::operator()(const Double_t* x, const Double_t* par) const {

  Float_t tempx;
  tempx = x[0];

  Float_t* tempp = new Float_t[fNPar];
  for (size_t i = 0; i < fNPar; i++){
    tempp[i] = par[i];
  }

  float val = DoEval(&tempx, tempp);
  delete tempp;

  if (val < 0.0) val = 0.0;
  return val;
}


float Spline::operator()(const Float_t* x, const Float_t* par) const {
  float val = DoEval(x, par);
  if (val < 0.0) val = 0.0;
  return val;
}

/*
void Spline::Reconfigure(double x) {
  //   std::cout << "Reconfigured spline : " << fName << " : " << fForm << " to be " << x << std::endl;
  fX = x;
  fOutsideLimits = false;

  if (fX > fXMax) fX = fXMax;
  if (fX < fXMin) fX = fXMin;
}
*/

void Spline::Reconfigure(float x) {
  //  std::cout << "Reconfigured spline : " << fName << " : " << fForm << " to be " << x << std::endl;
  fX = x;
  fOutsideLimits = false;

  if (fX > fXMax) fX = fXMax;
  if (fX < fXMin) fX = fXMin;
}
/*
double Spline::DoEval(const Double_t* x, const Double_t* par) const {

  // Setup current fX to value
  fX = x[0];
  if (fX > fXMax) fX = fXMax;
  if (fX < fXMin) fX = fXMin;

  double w = DoEval(&par[0], false);

  if (w < 0.0) w = 0.0;

  // Now evaluate spline how FitWeight will do it.
  return w;
}
*/

float Spline::DoEval(const Float_t* x, const Float_t* par) const {

  // Setup current fX to value
  fX = x[0];
  if (fX > fXMax) fX = fXMax;
  if (fX < fXMin) fX = fXMin;

  double w = DoEval(&par[0], false);

  if (w < 0.0) w = 0.0;

  // Now evaluate spline how FitWeight will do it.
  return w;
}
/*
double Spline::DoEval(const Double_t* par, bool checkresponse) const {
  
  Float_t* temp = new Float_t[fNPar];
  for (size_t i = 0; i < fNPar; i++){
    temp[i] = par[i];
  }
  double val = DoEval(temp, checkresponse);
  delete temp;
  return val;
}
*/

float Spline::DoEval(const Float_t* par, bool checkresponse) const {

  if (!par) return 1.0;

  //  std::cout << "Spline::DoEval = " << par << std::endl;
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

  // Now evaluate spline
  switch (fType) {
  case k1DPol1:     { return Spline1DPol1(par); }
  case k1DPol2:     { return Spline1DPol2(par); }
  case k1DPol3:     { return Spline1DPol3(par); }
  case k1DPol4:     { return Spline1DPol4(par); }
  case k1DPol5:     { return Spline1DPol5(par); }
  case k1DPol6:     { return Spline1DPol6(par); }
  case k1DTSpline3: { return Spline1DTSpline3(par); }
  }

  // Return nominal weight
  return 1.0;
};



// Spline Functions
// ----------------------------------------------
float Spline::Spline1DPol1(const Float_t* par) const {
  float xp = fX;
  //std::cout << "Eval 1DPol1 with " << par[0] << " " << par[1]  << " " << xp << " " << par[0] + par[1] * xp << std::endl;
  return par[0] + par[1] * xp;
};

float Spline::Spline1DPol2(const Float_t* par) const {
  float xp = fX;
  return par[0] + par[1] * xp + par[2] * xp * xp;
};

float Spline::Spline1DPol3(const Float_t* par) const {
  float xp = fX;
  return par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp;
};

float Spline::Spline1DPol4(const Float_t* par) const {
  float xp = fX;
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp);
};

float Spline::Spline1DPol5(const Float_t* par) const {
  float xp = fX;
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp + par[5] * xp * xp * xp * xp * xp);
};

float Spline::Spline1DPol6(const Float_t* par) const {
  float xp = fX;

  float w = 0.0;
  // std::cout << "Pol Eval " << std::endl;
  for (int i = fNPar-1; i > 0; i--){
    w = xp * (par[0+i] + w);
  }
  w += par[0]; 
  return w;
};



float Spline::Spline1DTSpline3(const Float_t* par) const {

  //  std::cout << "Doing Spline Eval " << fX << " " << par << std::endl;
  // Find matching point
  iter_low  = fXScan.begin();
  iter_high = fXScan.begin();
  iter_high++;
  off = 0;
  
  while ( iter_high != fXScan.end() and 
	  (fX < (*iter_low) or fX >= (*iter_high)) ) {
    off += 4;
    iter_low++;
    iter_high++;
  }

  float dx   = fX - (*iter_low);
  float weight = (par[off] + dx * (par[off + 1] + dx * (par[off + 2] + dx * par[off + 3])));

  return weight;
};


void Spline::FitCoeff(int n, double* x, double* y, float* coeff, bool draw) {

  switch (fType) {

  // Polynominal Graph Fits
  case k1DPol1:
  case k1DPol2:
  case k1DPol3:
  case k1DPol4:
  case k1DPol5:
  case k1DPol6:
    FitCoeff1DGraph(n, x, y, coeff, draw);
    break;

  // Spline Fits use TSpline3 Class
  case k1DTSpline3:
    GetCoeff1DTSpline3(n, x, y, coeff, draw);
    break;
  }

}




// Fitting Functions
void Spline::FitCoeff1DGraph(int n, double* x, double* y, float* coeff, bool draw) {

  TGraph* gr = new TGraph(n, x, y);
  double xmin = 99999.9;
  double xmax = -99999.9;
  for (int i = 0; i < n; i++) {
    if (x[i] > xmax) xmax = x[i];
    if (x[i] < xmin) xmin = x[i];
  }

  double xwidth = xmax - xmin;
  xmin = xmin - xwidth * 0.01;
  xmax = xmax + xwidth * 0.01;

  // Create a new function for fitting.
  TF1* func = new TF1("f1", this, -30, 30, this->GetNPar());
  func->SetNpx(400);
  func->FixParameter(0, 1.0); // Fix so 1.0 at nominal

  // Run the actual spline fit
  StopTalking();
  
  // If linear fit with two points
  if (n == 2 and fType == k1DPol1){

    float m = (y[1] - y[0]) / (x[1] - x[0]);
    float c = y[0] - (0.0 - x[0]) * m;

    func->SetParameter(0, c);
    func->SetParameter(1, m);

  } else if (fType == k1DPol1){
    gr->Fit(func, "WQ");
  } else {
    gr->Fit(func, "FMWQ");
  }


  StartTalking();

  for (int i = 0; i < this->GetNPar(); i++) {
    coeff[i] = func->GetParameter(i);
  }

  if (draw) {
    gr->Draw("APL");
    gPad->Update();
    gPad->SaveAs(("plot_test_" + fName + ".pdf").c_str());
    std::cout << "Saving Graph" << std::endl;
    sleep(3);
  }

  delete func;
  delete gr;
}


// Spline extraction Functions
void Spline::GetCoeff1DTSpline3(int n, double* x, double* y, float* coeff, bool draw) {

  StopTalking();
  TSpline3 temp_spline = TSpline3("temp_spline", x, y, n);
  StartTalking();

  for (size_t i = 0; i < n; i++) {

    double a, b, c, d, e;
    temp_spline.GetCoeff(i, a, b, c, d, e);

    coeff[i * 4]   = y[i];
    coeff[i * 4 + 1] = c;
    coeff[i * 4 + 2] = d;
    coeff[i * 4 + 3] = e;
  }

  if (draw) {
    TGraph* gr = new TGraph(n, x, y);
    temp_spline.Draw("CA");
    gr->Draw("PL SAME");
    gPad->Update();
    gPad->SaveAs(("plot_test_" + fName + ".pdf").c_str());
    // sleep(3);
    delete gr;
  }

  return;
}


