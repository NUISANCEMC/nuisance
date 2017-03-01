#include "SplineWriter.h"


// Spline reader should have access to every spline.
// Should know when reconfigure is called what its limits are and adjust accordingly.
// Then should pass it the index required in the stack as &xvals[index], and &pars[parindex]

void SplineWriter::Write(std::string name) {
  // Create a TTree with each form and scan points in it.
  TTree* tr = new TTree(name.c_str(), name.c_str());

  // Loop over all splines and add a TString in the TTree for its inputs
  tr->Branch("Spline", &fSpline);
  tr->Branch("Type", &fType);
  tr->Branch("Form", &fForm);
  tr->Branch("Points", &fPoints);
  tr->Fill();
  tr->Write();

  delete tr;
}

void SplineWriter::AddCoefficientsToTree(TTree* tr) {
  // Add only the fitted spline coefficients to the ttree
  std::cout << "Saving Spline Coeff to TTree = " << Form("SplineCoeff[%d]/D", fNCoEff) << std::endl;
  sleep(1);
  tr->Branch("SplineCoeff", fCoEffStorer, Form("SplineCoeff[%d]/D", fNCoEff));
}


void SplineWriter::SetupSplineSet() {
  std::cout << "Setting up spline set" << std::endl;

  // Create the coefficients double*
  fNCoEff = 0;
  for (int i = 0; i < fAllSplines.size(); i++) {
    fNCoEff += fAllSplines[i].GetNPar();
  }
  fCoEffStorer = new double[fNCoEff];

  std::cout << "NCoeff = " << fNCoEff << std::endl;

  // Calculate the grid of parsets
  // Setup the list of parameter coefficients.
  std::vector<double> nomvals = fRW->GetDialValues();
  fParVect.clear();
  fSetIndex.clear();

  // Loop over all splines.
  for (int i = 0; i < fAllSplines.size(); i++) {

    // For each dial loop over all points within a given position
    std::vector<double> newvals = nomvals;

    // Create a new set of nom vals for that dial set, and attribute it to the spline index.
    int pos = fRW->GetDialPos(fSpline[i]);

    // Split Points
    std::vector<double> vals = GeneralUtils::ParseToDbl(fPoints[i], ",");
    for (int j = 0; j < vals.size(); j++) {
      newvals[pos] = vals[j];
      fParVect.push_back(newvals);
      fValList.push_back(vals[j]);
      fWeightList.push_back(1.0);
      fSetIndex.push_back(i);
    }
  }

  // Print out the parameter set
  std::cout << "Parset | Index | Pars --- " << std::endl;
  for (int i = 0; i < fSetIndex.size(); i++) {
    std::cout << " Set " << i << ". | " << fSetIndex[i] << " | ";
    for (int j = 0; j < fParVect[i].size(); j++) {
      std::cout << " " << fParVect[i][j];
    }
    std::cout << std::endl;
  }
}

void SplineWriter::FitSplinesForEvent(FitEvent* event) {

  // Loop over parameter sets
  for (int i = 0; i < fParVect.size(); i++) {
    // Update FRW
    fRW->SetAllDials(&fParVect[i][0], fParVect[i].size());

    // Calculate a weight for event
    double weight = fRW->CalcWeight(event);

    // Fill Weight Set
    fWeightList[i] = weight;
    std::cout << "Calculating values from weight set " << i << " " << fParVect[i][0] << " = " << weight << std::endl;

  }

  // Loop over splines
  int count = 0;
  int coeffcount = 0;
  for (int i = 0; i < fAllSplines.size(); i++) {

    // Store X/Y Vals
    std::vector<double> xvals;
    std::vector<double> yvals;
    bool hasresponse = false;
    int npar = (fAllSplines[i]).GetNPar();

    for (int j = 0; j <  fSetIndex.size(); j++) {
      if (fSetIndex[j] != i) continue;
      xvals.push_back(fValList[j]);
      yvals.push_back(fWeightList[j] - 0.0);
      if (fWeightList[j] != 1.0) hasresponse = true;
    }

    // Make a new graph and fit coeff if response
    if (hasresponse) {
      (fAllSplines[i]).FitCoeff(int(xvals.size()), &xvals[0], &yvals[0], &fCoEffStorer[coeffcount], fDrawSplines);
    } else {
      for (int i = 0; i < npar; i++) {
        fCoEffStorer[coeffcount + i] = 0.0;
      }
    }

    // Make a new plot
    if (fDrawSplines and hasresponse) {
      TGraph* gr = new TGraph(xvals.size(), &xvals[0], &yvals[0]);

      // Get XMax Min
      int n = xvals.size();
      double xmin = 99999.9;
      double xmax = -99999.9;
      for (int i = 0; i < n; i++) {
        if (xvals[i] > xmax) xmax = xvals[i];
        if (xvals[i] < xmin) xmin = xvals[i];
      }

      double xwidth = xmax - xmin;
      xmin = xmin - xwidth * 0.01;
      xmax = xmax + xwidth * 0.01;

      TH1D* hist = new TH1D("temp", "temp", 100, xmin, xmax);
      for (int k = 0; k < 100; k++) {
        double xtemp = hist->GetXaxis()->GetBinCenter(k + 1);
        double ytemp = fAllSplines[i].DoEval(&xtemp, &fCoEffStorer[coeffcount]);
        hist->SetBinContent(k + 1, ytemp);
        // std::cout << "Set Temp " << k << " " << ytemp << std::endl;
      }

      // gr->Draw("APL");
      hist->SetLineColor(kRed);
      hist->Draw("HIST C");
      gr->SetMarkerStyle(20);
      gr->Draw("P SAME");

      gPad->Update();
      gPad->SaveAs(("F1eval_" + fSpline[i] + ".pdf").c_str());

      delete gr;
      // delete f1;
    }

    coeffcount += npar;
  }


  // Check overrides
  if (fDrawSplines) {
    coeffcount = 0;
    for (int i = 0; i < fAllSplines.size(); i++) {


      // Store X/Y Vals
      std::vector<double> xvals;
      std::vector<double> yvals;
      bool hasresponse = false;
      int npar = (fAllSplines[i]).GetNPar();

      for (int j = 0; j <  fSetIndex.size(); j++) {
        if (fSetIndex[j] != i) continue;
        xvals.push_back(fValList[j]);
        yvals.push_back(fWeightList[j] - 0.0);
        if (fWeightList[j] != 1.0) hasresponse = true;
      }

      if (hasresponse) {

        TGraph* gr = new TGraph(xvals.size(), &xvals[0], &yvals[0]);

        // Get XMax Min
        int n = xvals.size();
        double xmin = 99999.9;
        double xmax = -99999.9;
        for (int i = 0; i < n; i++) {
          if (xvals[i] > xmax) xmax = xvals[i];
          if (xvals[i] < xmin) xmin = xvals[i];
        }

        double xwidth = xmax - xmin;
        xmin = xmin - xwidth * 0.01;
        xmax = xmax + xwidth * 0.01;

        TH1D* hist = new TH1D("temp", "temp", 100, xmin, xmax);
        for (int k = 0; k < 100; k++) {
          double xtemp = hist->GetXaxis()->GetBinCenter(k + 1);
          double ytemp = fAllSplines[i].DoEval(&xtemp, &fCoEffStorer[coeffcount]);
          hist->SetBinContent(k + 1, ytemp);
          // std::cout << "Set Temp " << k << " " << ytemp << std::endl;
        }

        // gr->Draw("APL");
        hist->SetLineColor(kRed);
        hist->Draw("HIST C");
        gr->SetMarkerStyle(20);
        gr->Draw("P SAME");

        gPad->Update();
        gPad->SaveAs(("F2_eval_" + fSpline[i] + ".pdf").c_str());

        delete gr;
        // delete f1;
      }
      coeffcount += npar;
    }
  }
}
