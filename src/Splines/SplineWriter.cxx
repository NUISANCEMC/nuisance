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
  std::cout << "Saving Spline Coeff to TTree = " << Form("SplineCoeff[%d]/F", fNCoEff) << std::endl;
  //  sleep(1);
  tr->Branch("SplineCoeff", fCoEffStorer, Form("SplineCoeff[%d]/F", fNCoEff));
}


void SplineWriter::SetupSplineSet() {
  std::cout << "Setting up spline set" << std::endl;

  // Create the coefficients double*
  fNCoEff = 0;
  for (int i = 0; i < fAllSplines.size(); i++) {
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
  fWeightList.push_back(1.0);
  fValList.push_back(0.0);

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
      fSetIndex.push_back(i+1);
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

  fRW->SetAllDials(&fParVect[0][0], fParVect[0].size());
  double nomweight = fRW->CalcWeight(event);

  event->RWWeight = nomweight;
  if (fDrawSplines){
    std::cout << "Nominal Spline Weight = " << nomweight << std::endl;
  }

  // Loop over parameter sets
  for (int i = 1; i < fParVect.size(); i++) {
    // Update FRW
    fRW->SetAllDials(&fParVect[i][0], fParVect[i].size());

    // Calculate a weight for event
    double weight = fRW->CalcWeight(event);


    if (weight >= 0.0 and weight < 200){
      // Fill Weight Set
      fWeightList[i] = weight/nomweight;
      if (fDrawSplines) std::cout << "Calculating values from weight set " << i << " " << fParVect[i][0] << " = " << weight << " " << weight/nomweight << std::endl;
    } else {
      fWeightList[i] = 1.0;
    }

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
      if (fSetIndex[j] != i+1) continue;
      xvals.push_back(fValList[j]);
      yvals.push_back(fWeightList[j] - 0.0);
      if (fWeightList[j] != 1.0) hasresponse = true;
    }

    // Make a new graph and fit coeff if response
    if (hasresponse) {
      (fAllSplines[i]).FitCoeff(int(xvals.size()), &xvals[0], &yvals[0], &fCoEffStorer[coeffcount], fDrawSplines);
    } else {
      //      std::cout << "Spline " << fSpline[i] << " has no response. " << std::endl;
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
      for (int j = 0; j < n; j++) {
        if (xvals[j] > xmax) xmax = xvals[j];
        if (xvals[j] < xmin) xmin = xvals[j];
      }

      double xwidth = xmax - xmin;
      xmin = xmin - 0.01;
      xmax = xmax + 0.01;

      TH1D* hist = new TH1D("temp", "temp", 100, xmin, xmax);
      for (int k = 0; k < 100; k++) {
        double xtemp = hist->GetXaxis()->GetBinCenter(k + 1);
	fAllSplines[i].Reconfigure(xtemp);
        double ytemp = fAllSplines[i].DoEval(&fCoEffStorer[coeffcount]);
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
        if (fSetIndex[j] != i+1) continue;
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
        for (int j = 0; j < n; j++) {
          if (xvals[j] > xmax) xmax = xvals[j];
          if (xvals[j] < xmin) xmin = xvals[j];
        }

	//        double xwidth = xmax - xmin;
	// xmin = xmin - xwidth * 0.01;
        //xmax = xmax + xwidth * 0.01;

        TH1D* hist = new TH1D("temp", "temp", 100, xmin, xmax);
	//	for (int k = 0; k < fAllSplines[i].GetNPar(); k++){
	//	  std::cout << fAllSplines[i].GetName() << " Coeff " << k << " = " << fCoEffStorer[coeffcount + k] << std::endl;
	//	}

        for (int k = 0; k < 100; k++) {
          double xtemp = hist->GetXaxis()->GetBinCenter(k + 1);
	  fAllSplines[i].Reconfigure(xtemp);
          double ytemp = fAllSplines[i].DoEval(&fCoEffStorer[coeffcount]);
          hist->SetBinContent(k + 1, ytemp);
	  
	  //          std::cout << "Set Temp " << k << " " << ytemp << " First Coeff = " << fCoEffStorer[coeffcount] << std::endl;
        }

        // gr->Draw("APL");
        hist->SetLineColor(kRed);
        hist->Draw("HIST C");
        hist->SetTitle("Spline Response");
        hist->GetYaxis()->SetRangeUser(0.0,3.0);
        // gStyle->SetOptStat(0);
        hist->SetStats(0);
        gr->SetMarkerStyle(20);
        gr->SetTitle("True Weight Points");
        gr->Draw("P SAME");
        gPad->BuildLegend();
        gPad->Update();

        hist->SetTitle(fSpline[i].c_str());
        hist->GetXaxis()->SetTitle("Dial Variation");
        hist->GetYaxis()->SetTitle("Event Weight");
        gPad->Update();
        gPad->SaveAs(("F2_eval_" + fSpline[i] + ".pdf").c_str());

        delete gr;
	std::cout << "Saved hist for " << fSpline[i] << std::endl;
        sleep(5);
        // delete f1;
      }
      coeffcount += npar;
    }
  }
}
