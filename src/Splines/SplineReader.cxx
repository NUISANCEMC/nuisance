#include "SplineReader.h"


// Spline reader should have access to every spline.
// Should know when reconfigure is called what its limits are and adjust accordingly.
// Then should pass it the index required in the stack as &xvals[index], and &pars[parindex]

void SplineReader::AddSpline(nuiskey splinekey) {

  // Read info from key
  std::string splname     = splinekey.GetS("name");
  std::string type        = splinekey.GetS("type");
  std::string form        = splinekey.GetS("form");
  std::string points      = splinekey.GetS("points");
  std::string extrapolate = splinekey.GetS("extrapolate");

  //  std::cout << "AddSpline " << splname << " " << type << " " << form << " " << points << std::endl;

  // Add the spline to the list of all forms
  fAllSplines.push_back( Spline(splname, form, points) );
  fSpline.push_back(splname);
  fType.push_back(type);
  fForm.push_back(form);
  fPoints.push_back(points);


}


void SplineReader::Read(TTree* tr) {

  std::vector<std::string>* tempspline = 0;
  std::vector<std::string>* temptype = 0;
  std::vector<std::string>* tempform = 0;
  std::vector<std::string>* temppoints = 0;


  // Loop over all splines and add a TString in the TTree for its inputs
  tr->SetBranchAddress("Spline", &tempspline);
  tr->SetBranchAddress("Type",   &temptype);
  tr->SetBranchAddress("Form",   &tempform);
  tr->SetBranchAddress("Points", &temppoints);
  tr->GetEntry(0);

  // Copy over
  for (size_t i = 0; i < tempspline->size(); i++){
    fSpline.push_back(tempspline->at(i));
    fType.push_back(temptype->at(i));
    fForm.push_back(tempform->at(i));
    fPoints.push_back(temppoints->at(i));
  }

  // Loop through and add splines from read type.
  for (size_t i = 0; i < fSpline.size(); i++) {
    LOG(SAM) << "Registering Input Spline " << fSpline[i] << " " << fForm[i] << " " << fPoints[i] << std::endl;
    fAllSplines.push_back( Spline(fSpline[i], fForm[i], fPoints[i]) );
  }
}


void SplineReader::Reconfigure(std::map< std::string, double >& vals) {

  // std::cout << "NEW SPLINE READER =========" << std::endl;
  for (std::map<std::string, double>::iterator iter = vals.begin(); 
      iter != vals.end(); iter++){

    // std::cout << "Found " << iter->first << " in map handed to reader." << std::endl;
    for (size_t i = 0; i < fSpline.size(); i++){
      // std::cout << "Passing it to : " << fSpline[i] << std::endl;
      // if (!fSpline[i].compare(iter->first.c_str())){
	      // std::cout << "Reconfiguring Value inside Reader to be " << fSpline[i] << " " << iter->second << std::endl;
        fAllSplines[i].Reconfigure(iter->first, iter->second);
      // }
    }
  }

  fNeedsReconfigure = false;
}

bool SplineReader::NeedsReconfigure() {
  return fNeedsReconfigure;
}

void SplineReader::SetNeedsReconfigure(bool val) {
  fNeedsReconfigure = val;
}

double SplineReader::CalcWeight(float* coeffs) {

  int off = 0;
  double rw_weight = 1.0;

  // Form Offset list
  std::vector<int> offsets;
  for (size_t i = 0; i < fAllSplines.size(); i++) {
    offsets.push_back(off);
    off +=  fAllSplines[i].GetNPar();
  }

  // #pragma omp parrallel for
  for (size_t i = 0; i < fAllSplines.size(); i++) {

    // std::cout << "Evaluating spline " << fAllSplines[i].GetName() << " at coeff offset " << off <<  "(" << coeffs << ")" << std::endl;
    // for (int j = 0; j < fAllSplines[i].GetNPar(); j++){
    // std::cout << "Coeff " << j+off << " " << coeffs[off+j] << std::endl;
    // }

    double w = fAllSplines[i].DoEval( &coeffs[offsets[i]] );
    rw_weight *= w;
    
    // std::cout << "Spline RW Weight = " << rw_weight << " " << w << std::endl;
  }

  if (rw_weight <= 0.0) rw_weight = 1.0;
  // std::cout << "Returning spline rw_weight = " << rw_weight << std::endl;

  return rw_weight;
}



int SplineReader::GetNPar(){
  int n = 0;
  for (size_t i = 0; i < fAllSplines.size(); i++){
    n += fAllSplines[i].GetNPar();
  }
  return n;
}







