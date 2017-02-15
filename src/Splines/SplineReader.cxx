#include "SplineReader.h"

void SplineReader::AddSpline(std::string splname, std::string type, std::string form, std::string points){
  
  // Get the dial positions for the spline names
  std::vector<std::string> dials = GeneralUtils::ParseToStr(splname,":");
  std::vector<int> pos;
  for (int i = 0; i < dials.size(); i++){
    pos.push_back( fRW->GetDialPos(dials[i]) );
  }
  std::cout << "AddSpline " << splname << " " << type << " " << form << " " << points << std::endl;

  // Add the spline to the list of all forms
  fAllSplines.push_back( Spline(splname, form, pos) );
  fSpline.push_back(splname);
  fType.push_back(type);
  fForm.push_back(form);
  fPoints.push_back(points);

}

void SplineReader::Write(std::string name){
  // Create a TTree with each form and scan points in it.
  TTree* tr = new TTree(name.c_str(),name.c_str());

  // Loop over all splines and add a TString in the TTree for its inputs
  tr->Branch("Spline",&fSpline);
  tr->Branch("Type",&fType);
  tr->Branch("Form",&fForm);
  tr->Branch("Points",&fPoints);
  tr->Fill();
  tr->Write();

  delete tr;
}

void SplineReader::Read(TTree* tr){

  // Make temp vectors for tree read
  std::vector<std::string>* spl = 0;
  std::vector<std::string>* typ = 0;
  std::vector<std::string>* frm = 0;
  std::vector<std::string>* pnt = 0;
  
  // Read From Tree
  tr->SetBranchAddress("Spline", &spl);
  tr->SetBranchAddress("Type",   &typ);
  tr->SetBranchAddress("Form",   &frm);
  tr->SetBranchAddress("Points", &pnt);
  tr->GetEntry(0);

  // Add Splines to this empty reader
  for (int i = 0; i < spl->size(); i++){
    AddSpline(spl->at(i), typ->at(i), frm->at(i), pnt->at(i));
  }
  
  // Reset
  tr->ResetBranchAddresses();
}

void SplineReader::Reconfigure(){

  // Creates a dial set array which can be looped through and passed to each of the splines given the current fitweight state.
  

}

/*
double SplineReader::CalcWeight(FitEvent* event){

  // Grab Co-efficients from the event.

  // Loop over all dial values matched to each spline

  // Calculate a weight

  // return it

}
*/
void SplineReader::AddCoefficientsToTree(TTree* tr){
  // Add only the fitted spline coefficients to the ttree
  tr->Branch("SplineCoeff", fCoEffStorer, Form("SplineCoeff[%d]/D",fNCoEff));
}


void SplineReader::SetupSplineSet(){
  std::cout << "Setting up spline set" << std::endl;
  totalcount = 0;

  // Create the coefficients double*
  fNCoEff = 0;
  for (int i = 0; i < fAllSplines.size(); i++){
    fNCoEff += fAllSplines[i].GetNPar();
  }
  fCoEffStorer = new double[fNCoEff];


  // Calculate the grid of parsets
  // Setup the list of parameter coefficients.
  std::vector<double> nomvals = fRW->GetDialValues();
  fParVect.clear();
  fSetIndex.clear();

  // Loop over all splines.
  for (int i = 0; i < fAllSplines.size(); i++){

    // For each dial loop over all points within a given position
    std::vector<double> newvals = nomvals;

    // Create a new set of nom vals for that dial set, and attribute it to the spline index.
    int pos = fRW->GetDialPos(fSpline[i]);

    // Split Points
    std::vector<double> vals = GeneralUtils::ParseToDbl(fPoints[i],",");
    for (int j = 0; j < vals.size(); j++){
      newvals[pos] = vals[j];
      fParVect.push_back(newvals);
      fValList.push_back(vals[j]);
      fWeightList.push_back(1.0);
      fSetIndex.push_back(i);
    }

    // Split both dials->pos and points->doubles;
    // Loop over all indices and create parameter sets for that dial.
    // Also save the x,y sets for that parameter set.
  }

  // Print out the parameter set
  std::cout << "Full Parset" << std::endl;
  for (int i = 0; i < fSetIndex.size(); i++){
    std::cout << " Set " << i << ". " << fSetIndex[i] ;
    for (int j = 0; j < fParVect[i].size(); j++){
      std::cout << " " << fParVect[i][j];
    }
    std::cout << std::endl;
  }
}

void SplineReader::FitSplinesForEvent(FitEvent* event){

  // Loop over parameter sets
  for (int i = 0; i < fParVect.size(); i++){
    // Update FRW
    fRW->SetAllDials(&fParVect[i][0], fParVect[i].size());

    // Calculate a weight for event
    double weight = fRW->CalcWeight(event);

    // Fill Weight Set
    fWeightList[i] = weight;
  }

  // Loop over splines
  int count = 0;
  int coeffcount = 0;
  for (int i = 0; i < fAllSplines.size(); i++){

    // Store X/Y Vals
    std::vector<double> xvals;
    std::vector<double> yvals;
    bool hasresponse = false;
    int npar = (fAllSplines[i]).GetNPar();

    for (int j = 0; j <  fSetIndex.size(); j++){
      if (fSetIndex[j] != i) continue;
      xvals.push_back(fValList[j]);
      yvals.push_back(fWeightList[j]-1.0);	
      if (fWeightList[j] != 1.0) hasresponse = true;
    }

    // Make a new graph and fit coeff if response
    if (hasresponse){
      (fAllSplines[i]).FitCoeff(int(xvals.size()), &xvals[0], &yvals[0], &fCoEffStorer[coeffcount]);
    } else {
      for (int i = 0; i < npar; i++){
	fCoEffStorer[coeffcount + i] = 0.0;
      }
    }
    coeffcount += npar;
  }
}
