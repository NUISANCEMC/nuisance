#include "Spline.h"
using namespace SplineUtils;

// Setup Functions
// ----------------------------------------------
Spline::Spline(std::string splname, std::string form, std::string points) {

  // Save Definition
  fName = splname;
  fForm = form;
  fPoints = points;
  fROOTFunction = NULL;

  // Setup Min Max for each Parameter
  fSplitNames = GeneralUtils::ParseToStr(splname, ";");
  std::vector<std::vector<double> > gridvals =
      SplineUtils::GetSplitDialPoints(fPoints);

  for (size_t i = 0; i < fSplitNames.size(); i++) {

    for (size_t j = 0; j < gridvals.size(); j++) {
      if (i == 0)
        fXScan.push_back(gridvals[j][0]);
      if (i == 1)
        fXScan.push_back(gridvals[j][1]);
    }

    double xmin = 9999.9;
    double xmax = -9999.9;
    for (size_t j = 0; j < gridvals.size(); j++) {
      if (gridvals[j][i] < xmin)
        xmin = gridvals[j][i];
      if (gridvals[j][i] > xmax)
        xmax = gridvals[j][i];
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
  if (!fForm.compare("1DPol1")) {
    Setup(k1DPol1, 1, 2);
  } else if (!fForm.compare("1DPol2")) {
    Setup(k1DPol2, 1, 3);
  } else if (!fForm.compare("1DPol3")) {
    Setup(k1DPol3, 1, 4);
  } else if (!fForm.compare("1DPol4")) {
    Setup(k1DPol4, 1, 5);
  } else if (!fForm.compare("1DPol5")) {
    Setup(k1DPol5, 1, 6);
  } else if (!fForm.compare("1DPol6")) {
    Setup(k1DPol6, 1, 7);
  } else if (!fForm.compare("1DTSpline3")) {
    Setup(k1DTSpline3, 1, fXScan.size() * 4);
  } else if (!fForm.compare("2DPol6")) {
    Setup(k2DPol6, 2, 28);
  } else if (!fForm.compare("2DGaus")) {
    Setup(k2DGaus, 2, 8);
  } else if (!fForm.compare("2DTSpline3")) {
    Setup(k2DTSpline3, 2, fXScan.size() * fYScan.size() * 8);
  } else {
    NUIS_ABORT("Unknown spline form : " << fForm);
  }

  // Run Checks
  if ((UInt_t)fNDim != fSplitNames.size()) {
    NUIS_ABORT("Spline Dim:Names mismatch!");
  }

  NUIS_LOG(SAM, "Setup Spline " << fForm << " = " << fType << " " << fNPar);
};

void Spline::Setup(int type, int ndim, int npar) {
  fType = type;
  fNDim = ndim;
  fNPar = npar;
}

// Reconfigure Functions
// ----------------------------------------------
void Spline::Reconfigure(float x, int index) {
  // std::cout << "Reconfigured spline : " << fName << " : " << fForm << " to be
  // " << x << " " << index << std::endl;
  fVal[index] = x;
  fOutsideLimits = false;

  if (fVal[index] > fValMax[index])
    fVal[index] = fValMax[index];
  if (fVal[index] < fValMin[index])
    fVal[index] = fValMin[index];
  // std::cout << "Set at edge = " << fVal[index] << " " << index << std::endl;
}

void Spline::Reconfigure(std::string name, float x) {
  for (size_t i = 0; i < fSplitNames.size(); i++) {
    // std::cout << "-> Comparing in spline " << name << " to " <<
    // fSplitNames[i] << " = " << !fSplitNames[i].compare(name.c_str()) <<
    // std::endl;

    if (!fSplitNames[i].compare(name.c_str())) {
      // std::cout << "-> Reconfigured spline  : " << fSplitNames[i] << " " <<
      // name << " to be " << x << " " << i << std::endl;
      Reconfigure(x, i);
    }
  }
}

// Evaluation Functions
// ----------------------------------------------
double Spline::operator()(const Double_t *x, const Double_t *par) {

  Float_t *tempx = new Float_t[fNDim];
  for (size_t i = 0; i < (UInt_t)fNDim; i++) {
    tempx[i] = x[i];
  }

  Float_t *tempp = new Float_t[fNPar];
  for (size_t i = 0; i < (UInt_t)fNPar; i++) {
    tempp[i] = par[i];
  }

  float val = DoEval(tempx, tempp);
  delete tempp;
  delete tempx;

  if (val < 0.0)
    val = 0.0;
  return val;
}

double Spline::DoEvalPar(const double *x, const double *p) const {
  Float_t *tempx = new Float_t[fNDim];
  for (size_t i = 0; i < (UInt_t)fNDim; i++) {
    tempx[i] = x[i];
  }

  Float_t *tempp = new Float_t[fNPar];
  for (size_t i = 0; i < (UInt_t)fNPar; i++) {
    tempp[i] = p[i];
  }

  float val = DoEval(tempx, tempp);
  delete tempp;
  delete tempx;

  if (val < 0.0)
    val = 0.0;
  return val;
}

float Spline::operator()(const Float_t *x, const Float_t *par) const {
  float val = DoEval(x, par);
  if (val < 0.0)
    val = 0.0;
  return val;
}

float Spline::DoEval(const Float_t *x, const Float_t *par) const {

  // Setup current fX to value
  for (size_t i = 0; i < (UInt_t)fNDim; i++) {
    fVal[i] = x[i];
    if (fVal[i] > fValMax[i])
      fVal[i] = fValMax[i];
    if (fVal[i] < fValMin[i])
      fVal[i] = fValMin[i];
  }

  double w = DoEval(&par[0], false);

  if (w < 0.0)
    w = 0.0;

  // Now evaluate spline how FitWeight will do it.
  return w;
}

float Spline::DoEval(const Float_t *par, bool checkresponse) const {

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
  case k1DPol1: {
    return Spline1DPol1(par);
  }
  case k1DPol2: {
    return Spline1DPol2(par);
  }
  case k1DPol3: {
    return Spline1DPol3(par);
  }
  case k1DPol4: {
    return Spline1DPol4(par);
  }
  case k1DPol5: {
    return Spline1DPol5(par);
  }
  case k1DPol6: {
    return Spline1DPol6(par);
  }
  case k1DTSpline3: {
    return Spline1DTSpline3(par);
  }
  case k2DPol6: {
    return Spline2DPol(par, 6);
  }
  case k2DGaus: {
    return Spline2DGaus(par);
  }
  case k2DTSpline3: {
    return Spline2DTSpline3(par);
  }
  }

  // Return nominal weight
  return 1.0;
};

// Spline Functions
// ----------------------------------------------

// 1D Functions
// ----------------------------------------------
float Spline::Spline1DPol1(const Float_t *par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp;
};

float Spline::Spline1DPol2(const Float_t *par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp + par[2] * xp * xp;
};

float Spline::Spline1DPol3(const Float_t *par) const {
  float xp = fVal[0];
  return par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp;
};

float Spline::Spline1DPol4(const Float_t *par) const {
  float xp = fVal[0];
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp);
};

float Spline::Spline1DPol5(const Float_t *par) const {
  float xp = fVal[0];
  return (par[0] + par[1] * xp + par[2] * xp * xp + par[3] * xp * xp * xp +
          par[4] * xp * xp * xp * xp + par[5] * xp * xp * xp * xp * xp);
};

float Spline::Spline1DPol6(const Float_t *par) const {
  float xp = fVal[0];

  float w = 0.0;
  // std::cout << "Pol Eval " << std::endl;
  for (int i = fNPar - 1; i > 0; i--) {
    w = xp * (par[0 + i] + w);
  }
  w += par[0];
  return w;
};

float Spline::Spline1DTSpline3(const Float_t *par) const {

  // Find matching point
  iter_low = fXScan.begin();
  iter_high = fXScan.begin();
  iter_high++;
  off = 0;
  fX = fVal[0];

  while (iter_high != fXScan.end() and
         (fX < (*iter_low) or fX >= (*iter_high))) {
    off += 4;
    iter_low++;
    iter_high++;
  }

  float dx = fX - (*iter_low);
  float weight = (par[off] + dx * (par[off + 1] +
                                   dx * (par[off + 2] + dx * par[off + 3])));

  return weight;
};

// 2D Functions
// ----------------------------------------------
float Spline::Spline2DPol(const Float_t *par, int n) const {

  float wx = (fVal[0] - fValMin[0]) / (fValMax[0] - fValMin[0]);
  float wy = (fVal[1] - fValMin[1]) / (fValMax[1] - fValMin[1]);
  float w = 0.0;
  int count = 0;

  w += par[count++];
  w += par[count++] * wx;
  w += par[count++] * wy;

  w += par[count++] * wx * wx;
  w += par[count++] * wx * wy;
  w += par[count++] * wy * wy;

  w += par[count++] * wx * wx * wx;
  w += par[count++] * wx * wx * wy;
  w += par[count++] * wx * wy * wy;
  w += par[count++] * wy * wy * wy;

  w += par[count++] * wx * wx * wx * wx;
  w += par[count++] * wx * wx * wx * wy;
  w += par[count++] * wx * wx * wy * wy;
  w += par[count++] * wx * wy * wy * wy;
  w += par[count++] * wy * wy * wy * wy;

  w += par[count++] * wx * wx * wx * wx * wx;
  w += par[count++] * wx * wx * wx * wx * wy;
  w += par[count++] * wx * wx * wx * wy * wy;
  w += par[count++] * wx * wx * wy * wy * wy;
  w += par[count++] * wx * wy * wy * wy * wy;
  w += par[count++] * wy * wy * wy * wy * wy;

  w += par[count++] * wx * wx * wx * wx * wx * wx;
  w += par[count++] * wx * wx * wx * wx * wx * wy;
  w += par[count++] * wx * wx * wx * wx * wy * wy;
  w += par[count++] * wx * wx * wx * wy * wy * wy;
  w += par[count++] * wx * wx * wy * wy * wy * wy;
  w += par[count++] * wx * wy * wy * wy * wy * wy;
  w += par[count++] * wy * wy * wy * wy * wy * wy;

  return w;
}

float Spline::Spline2DGaus(const Float_t *par) const {

  double Norm = 5.0 + par[1] * 20.0;
  double Tilt = par[2] * 10.0;
  double Pq0 = 1.0 + par[3] * 1.0;
  double Wq0 = 0.5 + par[4] * 1.0;
  double Pq3 = 1.0 + par[5] * 1.0;
  double Wq3 = 0.5 + par[6] * 1.0;
  double q0 = (fVal[0] - fValMin[0]) / (fValMax[0] - fValMin[0]);
  double q3 = (fVal[1] - fValMin[1]) / (fValMax[1] - fValMin[1]);

  double a = cos(Tilt) * cos(Tilt) / (2 * Wq0 * Wq0);
  a += sin(Tilt) * sin(Tilt) / (2 * Wq3 * Wq3);

  double b = -sin(2 * Tilt) / (4 * Wq0 * Wq0);
  b += sin(2 * Tilt) / (4 * Wq3 * Wq3);

  double c = sin(Tilt) * sin(Tilt) / (2 * Wq0 * Wq0);
  c += cos(Tilt) * cos(Tilt) / (2 * Wq3 * Wq3);

  double w = Norm;
  w *= exp(-a * (q0 - Pq0) * (q0 - Pq0));
  w *= exp(+2.0 * b * (q0 - Pq0) * (q3 - Pq3));
  w *= exp(-c * (q3 - Pq3) * (q3 - Pq3));

  return w;
}

float Spline::Spline2DTSpline3(const Float_t *par) const {

  // Find matching point
  std::vector<float>::iterator iter_low_x = fXScan.begin();
  std::vector<float>::iterator iter_high_x = fXScan.begin();
  std::vector<float>::iterator iter_low_y = fYScan.begin();
  std::vector<float>::iterator iter_high_y = fYScan.begin();
  iter_high_x++;
  iter_high_y++;

  off = 0;
  fX = fVal[0];
  fY = fVal[1];

  while ((iter_high_x != fXScan.end() and iter_high_y != fYScan.end()) and
         (fX < (*iter_low_y) or fX >= (*iter_high) or fY < (*iter_low_y) or
          fY >= (*iter_low_y))) {
    off += 9;
    iter_low_x++;
    iter_high_x++;

    if (iter_high_x == fXScan.end()) {
      iter_low_x = fXScan.begin();
      iter_high_x = fXScan.begin();
      iter_low_y++;
      iter_high_y++;
      std::cout << "Skipping to next tspline 3 rung " << *iter_low_y
                << std::endl;
    }
  }

  std::cout << "Evaluting TSpline3 at " << fX << " " << (*iter_low_x) << " "
            << fY << " " << (*iter_low_y) << std::endl;
  // sleep(1.0);
  float dx = fX - (*iter_low_x);
  float dy = fY - (*iter_low_y);

  float weight = (par[off] + dx * (par[off + 1] +
                                   dx * (par[off + 2] + dx * par[off + 3])));
  float weight2 =
      (par[off + 4] +
       dy * (par[off + 5] + dy * (par[off + 6] + dy * par[off + 7])));

  return weight * weight2 * par[off + 8];
};

TF1 *Spline::GetFunction() {

  if (!fROOTFunction) {
    if (fNDim == 1) {
      std::cout << "Creating new 1D Function";
      fROOTFunction = new TF1("f1", this, -30.0, 30.0, this->GetNPar());
    }

    if (fNDim == 2) {
      std::cout << "Creating new 2D Function" << std::endl;
      // ROOT::Math::IParametricFunctionMultiDim* func; // = new
      // ROOT::Math::IParametricFunctionMultiDim(this);
      fROOTFunction = new TF2("f2", SplineUtils::Func2DWrapper, -30.0, 30.0,
                              -30.0, -30.0, this->GetNPar());
    }
  }

  // Reset ROOT function before returning.
  fROOTFunction->SetParameter(0, 1.0);
  for (int i = 1; i < this->GetNPar(); i++) {
    fROOTFunction->SetParameter(i, 0.1);
  }

  if (fNDim == 2) {
    if (SplineUtils::gSpline != this)
      SplineUtils::gSpline = this;
  }

  return (TF1 *)fROOTFunction;
}

namespace SplineUtils {
Spline *gSpline = NULL;
}

double SplineUtils::Func2DWrapper(double *x, double *p) {
  return (*gSpline)(x, p);
}
