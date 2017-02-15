#include "Spline.h"


Spline::Spline(std::string splname, std::string form, std::vector<int> pos){
  fName = splname;
  fForm = form;

  // Set form from list
  if      (!fForm.compare("1DPol1")){ Setup( k1DPol1, 1, 2 ); }
  else if (!fForm.compare("1DPol2")){ Setup( k1DPol2, 1, 3 ); }
  else if (!fForm.compare("1DPol3")){ Setup( k1DPol3, 1, 4 ); }
  else if (!fForm.compare("1DPol4")){ Setup( k1DPol4, 1, 5 ); }
  else if (!fForm.compare("1DPol5")){ Setup( k1DPol5, 1, 6 ); }
  else if (!fForm.compare("1DPol6")){ Setup( k1DPol6, 1, 7 ); }
  else if (!fForm.compare("1DTSpline3_5P")){ Setup( k1DTSpline3_5P, 1, 5*4 ); }

  // Setup position grabber
  fXPos = new int[fNDim];
  for (int i = 0; i < pos.size(); i++){
    fXPos[i] = pos[i];
  }

};

void Spline::Setup(int type, int ndim, int npar){
  fType = type;
  fNDim = ndim;
  fNPar = npar;
}

double Spline::operator()(const Double_t* x, const Double_t* par) const {
  double val = DoEval(x,par);
  if (val < -1.0) val = -1.0;
  return val;
}

double Spline::DoEval(const Double_t* x, const Double_t* par) const {
  switch(fType){
  case k1DPol1: { return Spline1DPol1(x,par); }
  case k1DPol2: { return Spline1DPol2(x,par); }
  case k1DPol3: { return Spline1DPol3(x,par); }
  case k1DPol4: { return Spline1DPol4(x,par); }
  case k1DPol5: { return Spline1DPol5(x,par); }
  case k1DPol6: { return Spline1DPol6(x,par); }
  case k1DTSpline3_5P: { return Spline1DTSpline3_5P(x,par); }
  }  
  return 1.0;
};

void Spline::FitCoeff(const int n, const double* x, const double* y, double* coeff) {

  // Polynominal Graph Fits
  switch(fType){
  case k1DPol1:
  case k1DPol2:
  case k1DPol3:
  case k1DPol4:
  case k1DPol5:
  case k1DPol6:

    std::cout << "Fitting with " << this->GetNPar() << std::endl;
    TGraph* gr = new TGraph(n, x, y);
    TF1* func = new TF1("f1", this, -30.0, 30.0, this->GetNPar());
    func->SetNpx(400);
    gr->Fit(func,"FMWQ");

    for (int i = 0; i < this->GetNPar(); i++){
      coeff[i] = func->GetParameter(i);
    }

    //    gr->Draw("APL");
    //    gPad->Update();
    //    gPad->SaveAs(("plot_test_" + fName + ".pdf").c_str());
    //    sleep(2);

    break;
  }

}


// Spline Functions
// ----------------------------------------------
double Spline::Spline1DPol1(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return par[0] + par[1]*xp;
};

double Spline::Spline1DPol2(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return par[0] + par[1]*xp + par[2]*xp*xp;
};

double Spline::Spline1DPol3(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return par[0] + par[1]*xp + par[2]*xp*xp + par[3]*xp*xp*xp;
};

double Spline::Spline1DPol4(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return (par[0] + par[1]*xp + par[2]*xp*xp + par[3]*xp*xp*xp +
	  par[4]*xp*xp*xp*xp);
};

double Spline::Spline1DPol5(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return (par[0] + par[1]*xp + par[2]*xp*xp + par[3]*xp*xp*xp +
          par[4]*xp*xp*xp*xp + par[5]*xp*xp*xp*xp*xp);
};

double Spline::Spline1DPol6(const Double_t* x, const Double_t* par) const {
  double xp = x[fXPos[0]];
  return (par[0] + par[1]*xp + par[2]*xp*xp + par[3]*xp*xp*xp +
          par[4]*xp*xp*xp*xp + par[5]*xp*xp*xp*xp*xp +
	  par[6]*xp*xp*xp*xp*xp*xp);
};

	       

double Spline::Spline1DTSpline3_5P(const Double_t* x, const Double_t* par) const {

  return 1.0;
  /*


  std::vector<double>::iterator iter_low  = x_vals[0].begin();
  std::vector<double>::iterator iter_high = x_vals[0].begin();
  iter_high++;
  int count = off;

  while( iter_high != x_vals[0].end() ){
    if (y[0] >= (*iter_low) and y[0] < (*iter_high)){
      break;
    }
    count += 4;
    iter_low++;
    iter_high++;
  }

  double dx   = y[0] - (*iter_low);
  double weight = (par[count]+dx*(par[count+1]+dx*(par[count+2]+dx*par[count+3])));

  if (weight != weight) weight = 0.0;
  return weight;
  */

  return 1.0;
};
