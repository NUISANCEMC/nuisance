#include "SplineWriter.h"
using namespace SplineUtils;

// Spline reader should have access to every spline.
// Should know when reconfigure is called what its limits are and adjust
// accordingly. Then should pass it the index required in the stack as
// &xvals[index], and &pars[parindex]

void SplineWriter::Write(std::string name) {
  // Create a TTree with each form and scan points in it.
  TTree *tr = new TTree(name.c_str(), name.c_str());

  // Loop over all splines and add a TString in the TTree for its inputs
  tr->Branch("Spline", &fSpline);
  tr->Branch("Type", &fType);
  tr->Branch("Form", &fForm);
  tr->Branch("Points", &fPoints);
  tr->Fill();
  tr->Write();

  delete tr;
}

void SplineWriter::AddWeightsToTree(TTree *tr) {
  // Add only the fitted spline coefficients to the ttree
  std::cout << "Saving Spline Weights to TTree = "
            << Form("SplineWeights[%d]/F", (int)fValList.size()) << std::endl;
  //  sleep(1);
  tr->Branch("SplineWeights", fWeightList,
             Form("SplineWeights[%d]/D", (int)fValList.size()));
}

void SplineWriter::ReadWeightsFromTree(TTree *tr) {
  tr->SetBranchAddress("SplineWeights", fWeightList);
}

void SplineWriter::AddCoefficientsToTree(TTree *tr) {
  // Add only the fitted spline coefficients to the ttree
  std::cout << "Saving Spline Coeff to TTree = "
            << Form("SplineCoeff[%d]/F", fNCoEff) << std::endl;
  //  sleep(1);
  tr->Branch("SplineCoeff", fCoEffStorer, Form("SplineCoeff[%d]/F", fNCoEff));
}

void SplineWriter::SetupSplineSet() {
  std::cout << "Setting up spline set" << std::endl;
  fDrawSplines = FitPar::Config().GetParB("draw_splines");
  // Create the coefficients double*
  fNCoEff = 0;
  for (size_t i = 0; i < fAllSplines.size(); i++) {
    fNCoEff += fAllSplines[i].GetNPar();
  }
  fCoEffStorer = new float[fNCoEff];

  std::cout << "NCoeff = " << fNCoEff << std::endl;

  // Calculate the grid of parsets
  // Setup the list of parameter coefficients.
  std::vector<double> nomvals = fRW->GetDialValues();
  fParVect.clear();
  fSetIndex.clear();

  fParVect.push_back(nomvals);
  fSetIndex.push_back(0);
  // fWeightList.push_back(1.0);
  fValList.push_back(std::vector<double>(1, 0.0));

  // Loop over all splines.
  for (size_t i = 0; i < fAllSplines.size(); i++) {

    // For each dial loop over all points within a given position
    std::vector<double> newvals = nomvals;

    std::vector<int>
        pos; // = SplineUtils::GetSplitDialPositions(fRW, fSpline[i]);
    std::vector<std::string> splitnames =
        GeneralUtils::ParseToStr(fSpline[i], ";");

    for (size_t j = 0; j < splitnames.size(); j++) {
      int temppos = fRW->GetDialPos(splitnames[j]);
      pos.push_back(temppos);
    }

    std::vector<std::vector<double> > vals =
        SplineUtils::GetSplitDialPoints(fPoints[i]);

    for (size_t j = 0; j < vals.size(); j++) {

      for (size_t k = 0; k < pos.size(); k++) {
        newvals[pos[k]] = vals[j][k];
      }
      fParVect.push_back(newvals);
      fValList.push_back(vals[j]);
      // fWeightList.push_back(1.0);
      fSetIndex.push_back(i + 1);
    }
  }

  fWeightList = new double[fValList.size()];
  for (uint i = 0; i < fValList.size(); i++) {
    fWeightList[i] = 1.0;
  }

  // Print out the parameter set
  NUIS_LOG(FIT, "Parset | Index | Pars --- ");
  for (size_t i = 0; i < fSetIndex.size(); i++) {
    NUIS_LOGN(FIT, " Set " << i << ". | " << fSetIndex[i] << " | ");
    if (LOG_LEVEL(FIT)) {
      for (size_t j = 0; j < fParVect[i].size(); j++) {
        NUIS_LOGN(FIT, " " << fParVect[i][j]);
      }
      NUIS_LOG(FIT, "");
    }
  }
}

void SplineWriter::GetWeightsForEvent(FitEvent *event) {
  // Get Starting Weight
  fRW->SetAllDials(&fParVect[0][0], fParVect[0].size());
  double nomweight = fRW->CalcWeight(event);
  event->RWWeight = nomweight;

  if (fDrawSplines) {
    std::cout << "Nominal Spline Weight = " << nomweight << std::endl;
  }
  fWeightList[0] = nomweight;

  // Loop over parameter sets
  for (size_t i = 1; i < fParVect.size(); i++) {
    // Update FRW
    fRW->SetAllDials(&fParVect[i][0], fParVect[i].size());

    // Calculate a weight for event
    double weight = fRW->CalcWeight(event);

    if (weight >= 0.0 and weight < 200) {
      // Fill Weight Set
      fWeightList[i] = weight / nomweight;
      if (fDrawSplines)
        std::cout << "Calculating values from weight set " << i << " "
                  << fParVect[i][0] << " = " << weight << " "
                  << weight / nomweight << std::endl;
    } else {
      fWeightList[i] = 1.0;
    }
  }
}

void SplineWriter::GetWeightsForEvent(FitEvent *event, double *weights) {
  // Get Starting Weight
  fRW->SetAllDials(&fParVect[0][0], fParVect[0].size());
  double nomweight = fRW->CalcWeight(event);
  event->RWWeight = nomweight;

  if (fDrawSplines) {
    std::cout << "Nominal Spline Weight = " << nomweight << std::endl;
  }
  weights[0] = nomweight;

  // Loop over parameter sets
  for (size_t i = 1; i < fParVect.size(); i++) {
    // Update FRW
    fRW->SetAllDials(&fParVect[i][0], fParVect[i].size());

    // Calculate a weight for event
    double weight = fRW->CalcWeight(event);

    if (weight >= 0.0 and weight < 200) {
      // Fill Weight Set
      weights[i] = weight / nomweight;
      if (fDrawSplines)
        std::cout << "Calculating values from weight set " << i << " "
                  << fParVect[i][0] << " = " << weight << " "
                  << weight / nomweight << std::endl;
    } else {
      weights[i] = 1.0;
    }
    fWeightList[i] = weights[i];
  }
}

void SplineWriter::ReconfigureSet(int iset) {
  fCurrentSet = iset;
  fRW->SetAllDials(&fParVect[iset][0], fParVect[iset].size());
}

double SplineWriter::GetWeightForThisSet(FitEvent *event, int iset) {
  if (iset != -1 and iset != fCurrentSet) {
    ReconfigureSet(iset);
  }
  return fRW->CalcWeight(event);
}

void SplineWriter::SetWeights(double *weights) {
  for (uint i = 0; i < fParVect.size(); i++) {
    fWeightList[i] = weights[i];
  }
}

void SplineWriter::FitSplinesForEvent(double *inputweights, float *coeff) {

  int n = fAllSplines.size();
  int coeffcount = 0;

  for (int i = 0; i < n; i++) {

    // DialVals
    std::vector<std::vector<double> > dialvals;
    std::vector<double> weightvals;
    bool hasresponse = false;

    for (size_t j = 0; j < fSetIndex.size(); j++) {
      if (fSetIndex[j] != i + 1)
        continue;

      dialvals.push_back(fValList[j]);
      double tempw = inputweights[j];
      weightvals.push_back(tempw);

      if (tempw != 1.0)
        hasresponse = true;
    }

    // Perform Fit
    if (hasresponse) {
      // std::cout << "Fitting Coeff" << std::endl;
      FitCoeff(&fAllSplines[i], dialvals, weightvals, &coeff[coeffcount],
               fDrawSplines);
    } else {
      for (int j = 0; coeffcount + j < fNCoEff; j++) {
        // std::cout << "Setting 0.0 response " << coeffcount + i << " " <<
        // fNCoEff <<  std::endl;
        coeff[coeffcount + j] = 0.0;
      }
    }

    // std::cout << "Adding coeffcount" << std::endl;
    // Offset coeffcount
    coeffcount += (fAllSplines[i]).GetNPar();
  }
  // std::cout << "FitEvent" << std::endl;
  return;
}

void SplineWriter::FitSplinesForEvent(TCanvas *fitcanvas, bool saveplot) {

  // Loop over splines
  //  int count = 0;
  int coeffcount = 0;
  int n = fAllSplines.size();

  std::vector<int> splinecounter;
  for (int i = 0; i < n; i++) {
    splinecounter.push_back(coeffcount);
    coeffcount += fAllSplines[i].GetNPar();
  }

  //  #pragma omp parallel for
  for (int i = 0; i < n; i++) {

    // Store X/Y Vals
    std::vector<std::vector<double> > dialvals;
    std::vector<double> weightvals;
    bool hasresponse = false;
    int npar = (fAllSplines[i]).GetNPar();
    coeffcount = splinecounter[i];

    for (size_t j = 0; j < fSetIndex.size(); j++) {
      if (fSetIndex[j] != i + 1)
        continue;

      dialvals.push_back(fValList[j]);
      double tempw = fWeightList[j];
      weightvals.push_back(tempw);

      if (tempw != 1.0)
        hasresponse = true;
    }

    // Make a new graph and fit coeff if response
    if (hasresponse) {
      FitCoeff(&fAllSplines[i], dialvals, weightvals, &fCoEffStorer[coeffcount],
               fDrawSplines);
    } else {
      for (int i = 0; i < npar; i++) {
        fCoEffStorer[coeffcount + i] = 0.0;
      }
    }
  }

  // Check overrides
  // if (saveplot) {
  //   coeffcount = 0;

  //   // Make new canvas to save stuff into
  //   if (fitcanvas) delete fitcanvas;
  //   fitcanvas = new TCanvas("c1", "c1", fAllSplines.size() * 400 , 600);
  //   fitcanvas->Divide(fAllSplines.size(), 1);

  //   // Clear out points
  //   for (size_t i = 0; i < fAllDrawnGraphs.size(); i++) {
  //     if (fAllDrawnGraphs[i]) delete fAllDrawnGraphs[i];
  //     if (fAllDrawnHists[i]) delete fAllDrawnHists[i];
  //   }
  //   fAllDrawnGraphs.clear();
  //   fAllDrawnHists.clear();

  //   for (size_t i = 0; i < fAllSplines.size(); i++) {

  //     fitcanvas->cd(i + 1);

  //     // Store X/Y Vals
  //     std::vector<std::vector<double> > dialvals;
  //     std::vector<double> weightvals;
  //     bool hasresponse = false;
  //     int npar = (fAllSplines[i]).GetNPar();

  //     for (size_t j = 0; j <  fSetIndex.size(); j++) {
  //       if ((UInt_t)fSetIndex[j] != (UInt_t)i + 1) continue;
  //       dialvals.push_back(fValList[j]);
  //       weightvals.push_back(fWeightList[j] - 0.0);
  //       if (fWeightList[j] != 1.0) hasresponse = true;
  //     }

  //     if (hasresponse) {

  //       TGraph* gr = new TGraph(dialvals.size(), dialvals, weightvals);
  //       fAllDrawnGraphs.push_back(gr);

  //       // Get XMax Min
  //       int n = xvals.size();
  //       double xmin = 99999.9;
  //       double xmax = -99999.9;
  //       for (int j = 0; j < n; j++) {
  //         if (xvals[j] > xmax) xmax = xvals[j];
  //         if (xvals[j] < xmin) xmin = xvals[j];
  //       }

  //       TH1D* hist = new TH1D("temp", "temp", 100, xmin, xmax);
  //       fAllDrawnHists.push_back(hist);

  //       for (int k = 0; k < 100; k++) {
  //         double xtemp = hist->GetXaxis()->GetBinCenter(k + 1);
  //         fAllSplines[i].Reconfigure(xtemp);
  //         double ytemp = fAllSplines[i].DoEval(&fCoEffStorer[coeffcount]);
  //         hist->SetBinContent(k + 1, ytemp);
  //       }

  //       // gr->Draw("APL");
  //       hist->SetLineColor(kRed);
  //       hist->Draw("HIST C");
  //       hist->SetTitle("Spline Response");
  //       // hist->GetYaxis()->SetRangeUser(0.0, 3.0);

  //       // gStyle->SetOptStat(0);
  //       hist->SetStats(0);
  //       gr->SetMarkerStyle(20);
  //       gr->SetTitle("True Weight Points");
  //       gr->Draw("P SAME");
  //       gPad->BuildLegend(0.6, 0.6, 0.85, 0.85);
  //       gPad->Update();

  //       hist->SetTitle(fSpline[i].c_str());
  //       hist->GetXaxis()->SetTitle("Dial Variation");
  //       hist->GetYaxis()->SetTitle("Event Weight");
  //       fitcanvas->Update();
  //     }
  //     coeffcount += npar;
  //   }
  // }
}

// Fitting
// ----------------------------------------------
void SplineWriter::FitCoeff(Spline *spl, std::vector<std::vector<double> > &v,
                            std::vector<double> &w, float *coeff, bool draw) {

  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  for (size_t i = 0; i < v.size(); i++) {
    x.push_back(v[i][0]);
    if (v[i].size() > 1)
      y.push_back(v[i][1]);
    if (v[i].size() > 2)
      z.push_back(v[i][2]);
  }

  switch (spl->GetType()) {

  // Polynominal Graph Fits
  case k1DPol1:
  case k1DPol2:
  case k1DPol3:
  case k1DPol4:
  case k1DPol5:
  case k1DPol6:

    FitCoeff1DGraph(spl, v.size(), &x[0], &w[0], coeff, draw);
    break;

  case k1DTSpline3:

    GetCoeff1DTSpline3(spl, x.size(), &x[0], &w[0], coeff, draw);
    break;

  case k2DPol6:
  case k2DGaus:
  case k2DTSpline3:
    FitCoeff2DGraph(spl, v.size(), &x[0], &y[0], &w[0], coeff, draw);
    break;

  default:
    break;
  }

#ifdef __MINUIT2_ENABLED__
  if (fDrawSplines) {
    fSplineFCNs[spl] = new SplineFCN(spl, v, w);
    fSplineFCNs[spl]->SaveAs("mysplinetest_" + spl->GetName() + ".pdf", coeff);
    sleep(1);
    delete fSplineFCNs[spl];
  }
#endif
}

void SplineWriter::FitCoeff1DGraph(Spline *spl, int n, double *x, double *y,
                                   float *coeff, bool draw) {

  TGraph *gr = new TGraph(n, x, y);
  double xmin = 99999.9;
  double xmax = -99999.9;
  for (int i = 0; i < n; i++) {
    if (x[i] > xmax)
      xmax = x[i];
    if (x[i] < xmin)
      xmin = x[i];
  }

  double xwidth = xmax - xmin;
  xmin = xmin - xwidth * 0.01;
  xmax = xmax + xwidth * 0.01;

  // Create a new function for fitting.
  TF1 *func = spl->GetFunction();

  // Run the actual spline fit
  StopTalking();

  // If linear fit with two points
  if (n == 2 and spl->GetType() == k1DPol1) {

    float m = (y[1] - y[0]) / (x[1] - x[0]);
    float c = y[0] - (0.0 - x[0]) * m;

    func->SetParameter(0, c);
    func->SetParameter(1, m);

  } else if (spl->GetType() == k1DPol1) {
    gr->Fit(func, "WQ");
  } else {
    gr->Fit(func, "FMWQ");
  }

  StartTalking();

  for (int i = 0; i < spl->GetNPar(); i++) {
    coeff[i] = func->GetParameter(i);
  }

  if (draw) {
    gr->Draw("APL");
    gPad->Update();
    gPad->SaveAs(("plot_test_" + spl->GetName() + ".pdf").c_str());
    std::cout << "Saving Graph" << std::endl;
    sleep(3);
  }

  // delete func;
  delete gr;
}

double SplineFCN::operator()(const double *x) const { return DoEval(x); }

double SplineFCN::DoEval(const double *x) const {

  float *fx = new float[fSpl->GetNPar()];
  for (int i = 0; i < fSpl->GetNPar(); i++) {
    fx[i] = x[i];
  } //

  double tot = 0;
  for (size_t i = 0; i < fVal.size(); i++) {

    int nonzero = 0;
    for (size_t j = 0; j < fVal[i].size(); j++) {
      fSpl->Reconfigure(fVal[i][j], j);
      // std::cout << "Reconfiguring " << fVal[i][j] << " " << j << std::endl;
      if (fVal[i][j] != 0.0)
        nonzero++;
    }
    if (uncorrelated and nonzero > 1)
      continue;

    double w = fSpl->DoEval(fx);
    double wdif = (w - fWeight[i]); // / (fWeight[i] * 0.05);
    tot += sqrt(wdif * wdif);
  }

  // delete fx;

  return tot;
}

void SplineFCN::UpdateWeights(std::vector<double> &w) {
  for (uint i = 0; i < w.size(); i++) {
    fWeight[i] = w[i];
  }
}

void SplineFCN::SetCorrelated(bool state) { uncorrelated = !state; }

void SplineFCN::SaveAs(std::string name, const float *fx) {

  if (fVal[0].size() > 2) {
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    c1->Divide(2, 1);
    TH1D *histmc =
        new TH1D("hist", "hist", fVal.size(), 0.0, double(fVal.size()));
    TH1D *histdt =
        new TH1D("histdt", "histdt", fVal.size(), 0.0, double(fVal.size()));

    for (size_t i = 0; i < fVal.size(); i++) {
      for (size_t j = 0; j < fVal[i].size(); j++) {
        fSpl->Reconfigure(fVal[i][j], j);
      }

      histmc->SetBinContent(i + 1, fSpl->DoEval(fx));
      histdt->SetBinContent(i + 1, fWeight[i]);
    }

    // histmc->Add(histdt,-1.0);

    c1->cd(1);
    histmc->SetTitle("Spline;Parameter Set;Weight");
    histmc->Draw("HIST");
    histdt->SetLineColor(kRed);
    histdt->Draw("SAME HIST");

    c1->cd(2);
    TH1D *histdif = (TH1D *)histmc->Clone();
    histdif->Add(histdt, -1.0);
    histdif->Draw("HIST");

    c1->Update();
    c1->SaveAs(name.c_str());
    delete c1;
  } else if (fVal[0].size() == 2) {

    TGraph2D *histmc = new TGraph2D();
    TGraph2D *histdt = new TGraph2D();
    TGraph2D *histdif = new TGraph2D();

    for (size_t i = 0; i < fVal.size(); i++) {
      for (size_t j = 0; j < fVal[i].size(); j++) {
        fSpl->Reconfigure(fVal[i][j], j);
      }

      histmc->SetPoint(histmc->GetN(), fVal[i][0], fVal[i][1],
                       fSpl->DoEval(fx));
      histdt->SetPoint(histdt->GetN(), fVal[i][0], fVal[i][1], fWeight[i]);
      histdif->SetPoint(histdif->GetN(), fVal[i][0], fVal[i][1],
                        fabs(fSpl->DoEval(fx) - fWeight[i]));
    }
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    c1->Divide(3, 1);
    c1->cd(1);
    histmc->SetTitle(("Spline;" + fSpl->GetName()).c_str());
    histmc->Draw("COLZ");
    gPad->Update();
    c1->cd(2);
    histdt->SetTitle(("Raw;" + fSpl->GetName()).c_str());
    histdt->Draw("COLZ");
    c1->cd(3);
    histdif->SetTitle(("Dif;" + fSpl->GetName()).c_str());
    histdif->Draw("COLZ");
    gPad->SetRightMargin(0.15);
    // gPad->SetLogz(1);

    gPad->Update();
    c1->SaveAs(name.c_str());
    delete c1;
  } else if (fVal[0].size() == 1) {

    TGraph *histmc = new TGraph();
    TGraph *histdt = new TGraph();
    TGraph *histdif = new TGraph();

    for (size_t i = 0; i < fVal.size(); i++) {
      for (size_t j = 0; j < fVal[i].size(); j++) {
        fSpl->Reconfigure(fVal[i][j], j);
      }

      histmc->SetPoint(histmc->GetN(), fVal[i][0], fSpl->DoEval(fx));
      histdt->SetPoint(histdt->GetN(), fVal[i][0], fWeight[i]);
      histdif->SetPoint(histdif->GetN(), fVal[i][0],
                        fabs(fSpl->DoEval(fx) - fWeight[i]));
    }
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    c1->Divide(2, 1);
    c1->cd(1);
    histmc->SetTitle(("Spline;" + fSpl->GetName()).c_str());
    histmc->Draw("AC");
    histmc->SetLineColor(kRed);

    histdt->SetTitle(("Raw;" + fSpl->GetName()).c_str());
    histdt->Draw("SAME P");
    histdt->SetMarkerStyle(20);
    gPad->Update();

    c1->cd(2);
    histdif->SetTitle(("Dif;" + fSpl->GetName()).c_str());
    histdif->Draw("APL");

    gPad->Update();
    c1->SaveAs(name.c_str());
    delete c1;
  }

  // gPad->SaveAs(name.c_str());
}

void SplineWriter::FitCoeff2DGraph(Spline *spl, int n, double *x, double *y,
                                   double *w, float *coeff, bool draw) {

#pragma omp critical
  {

    TF2 *f2 = (TF2 *)spl->GetFunction();

    TGraph2D *histmc = new TGraph2D(n, x, y, w);
    f2->SetNpx(400);

    StopTalking();
    histmc->Fit(f2, "FMWQ");
    StartTalking();

    for (int i = 0; i < spl->GetNPar(); i++) {
      coeff[i] = f2->GetParameter(i);
    }

    // delete f2;
    delete histmc;
  }
}

void SplineWriter::FitCoeffNDGraph(Spline *spl,
                                   std::vector<std::vector<double> > &v,
                                   std::vector<double> &w, float *coeff,
                                   bool draw) {
#ifdef __MINUIT2_ENABLED__

  if (fSplineFunctors.find(spl) != fSplineFunctors.end()) {
    delete fSplineFunctors[spl];
    fSplineFunctors.erase(spl);
  }

  if (fSplineFCNs.find(spl) != fSplineFCNs.end()) {
    delete fSplineFCNs[spl];
    fSplineFCNs.erase(spl);
  }

  if (fSplineMinimizers.find(spl) != fSplineMinimizers.end()) {
    delete fSplineMinimizers[spl];
    fSplineMinimizers.erase(spl);
  }

  if (fSplineMinimizers.find(spl) == fSplineMinimizers.end()) {
    std::cout << "Building new ND minimizer for " << spl << std::endl;
    fSplineFCNs[spl] = new SplineFCN(spl, v, w);

    // fSplineFCNs[spl] = new SplineFCN(spl, v, w);
    fSplineFunctors[spl] =
        new ROOT::Math::Functor(*fSplineFCNs[spl], spl->GetNPar());
    fSplineMinimizers[spl] =
        ROOT::Math::Factory::CreateMinimizer("Minuit2", "Combined");
    fSplineMinimizers[spl]->SetMaxFunctionCalls(1E8);
    fSplineMinimizers[spl]->SetMaxIterations(1E8);
    fSplineMinimizers[spl]->SetTolerance(1.E-6);
    fSplineMinimizers[spl]->SetStrategy(2);

    for (int j = 0; j < spl->GetNPar(); j++) {
      fSplineMinimizers[spl]->SetVariable(j, Form("Par_%i", j), 0.1, 0.1);
    }
  }
  // Create a new function for fitting.
  // StopTalking();

  // Update FCN
  fSplineFCNs[spl]->UpdateWeights(w);
  // fSplineMinimizers[spl]->SetDefaultOptions();
  // fSplineMinimizers[spl]->Clear();
  for (int j = 0; j < spl->GetNPar(); j++) {
    if (j != 0) {
      fSplineMinimizers[spl]->SetVariableValue(j, 0.1);
      // fSplineMinimizers[spl]->SetParameter(j, Form("Par_%i", j), 0.1, 0.1,
      // -100.0, 100.0 );

      // fSplineMinimizers[spl]->SetVariableValue(j, 0.1);
    }
  }
  // fSplineFCNs[spl]->SetCorrelated(false);
  // fSplineFunctors[spl] = new ROOT::Math::Functor( *fSplineFCNs[spl],
  // spl->GetNPar() );
  // fSplineMinimizers[spl]->SetFunction(*fSplineFunctors[spl]);
  // fSplineMinimizers[spl]->Minimize();

  fSplineFCNs[spl]->SetCorrelated(true);
  delete fSplineFunctors[spl];
  fSplineFunctors[spl] =
      new ROOT::Math::Functor(*fSplineFCNs[spl], spl->GetNPar());
  fSplineMinimizers[spl]->SetFunction(*fSplineFunctors[spl]);
  // ((TFitterMinuit*)fSplineMinimizers[spl])->CreateMinimizer(TFitterMinuit::kMigrad);
  fSplineMinimizers[spl]->Minimize();
  fSplineMinimizers[spl]->Minimize();

  // ((TFitterMinuit*)fSplineMinimizers[spl])->CreateMinimizer(TFitterMinuit::kMigrad);
  // fSplineMinimizers[spl]->Minimize();

  // delete minimizer;
  // StartTalking();

  // Now Get Parameters
  const double *values = fSplineMinimizers[spl]->X();
  for (int i = 0; i < spl->GetNPar(); i++) {

    std::cout << "Updated Coeff " << i << " " << values[i] << std::endl;
    coeff[i] = values[i];
  }

  // Save a sample
  fSplineFCNs[spl]->SaveAs("mysplinetest_" + spl->GetName() + ".pdf", coeff);
  sleep(1);

  // delete values;
  // delete minimizer;
#endif
}

// Spline extraction Functions
void SplineWriter::GetCoeff1DTSpline3(Spline *spl, int n, double *x, double *y,
                                      float *coeff, bool draw) {

  StopTalking();
  TSpline3 temp_spline = TSpline3("temp_spline", x, y, n);
  StartTalking();

  for (size_t i = 0; i < (UInt_t)n; i++) {

    double a, b, c, d, e;
    temp_spline.GetCoeff(i, a, b, c, d, e);

    coeff[i * 4] = y[i];
    coeff[i * 4 + 1] = c;
    coeff[i * 4 + 2] = d;
    coeff[i * 4 + 3] = e;
  }

  if (draw) {
    TGraph *gr = new TGraph(n, x, y);
    temp_spline.Draw("CA");
    gr->Draw("PL SAME");
    gPad->Update();
    gPad->SaveAs(("plot_test_" + spl->GetName() + ".pdf").c_str());

    delete gr;
  }

  return;
}
