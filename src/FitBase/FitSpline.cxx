// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "FitSpline.h"

//*************************************************
FitSpline::FitSpline(){
//*************************************************
  
  this->id     = "";
  this->form   = "";
  this->npar   = 0;
  this->ndim   = 0;
  this->spline = 0;
  this->offset    = 1;
  this->needs_fit = false;

}

//*************************************************
FitSpline::FitSpline(std::string ident, std::string dist,
		     std::vector<int> list_enums, std::string points_def ){
//*************************************************

  this->id   = ident;
  this->form = dist;
  this->var_enums = list_enums;

  //  this->x_vals = FitBase::FillScanPoints(points_def);
  this->npar   = 0;
  this->ndim   = 0;
  this->spline = 0;
  this->offset    = 1;
  this->points = points_def;
  
  // Point DEF is given as TYPEA:1,2,3;TYPEB:1,2,3
  // ETC
  std::vector<std::string> parsed_points = PlotUtils::FillVectorSFromString(points_def,";");
  for (UInt_t i = 0; i < parsed_points.size(); i++){
    std::string temp = parsed_points.at(i);
    std::vector<std::string> split_temp = PlotUtils::FillVectorSFromString(temp, ":"); 

    std::string choose_type   = split_temp.at(0);
    std::string choose_points = split_temp.at(1);

    std::vector<double> points_temp = PlotUtils::FillVectorDFromString(choose_points, ",");
    std::vector<double> points_final;

    if      (!choose_type.compare("PNTS")) points_final = points_temp;
    else if (!choose_type.compare("SCAN")){
      std::cout<<"RUNNING SCAN"<<std::endl;
      int npoints = int(points_temp[0]);
      double low  = double(points_temp[1]);

      double high = double(points_temp[2]);
      std::cout<<"Npoints "<<npoints<<" "<<low<<" "<<high<<std::endl;
      for (int i = 0; i < npoints; i++){
	std::cout<<"Pushing back "<< low + (i+0.)*(high - low)/(npoints-1.0+0.)<<std::endl;
	points_final.push_back( low + (i+0.)*(high - low)/(npoints-1.0+0.) );
      }
    }

    this->x.push_back(0.0);
    this->x_vals.push_back(points_final);
    this->x_low.push_back( points_final[0] );
    this->x_high.push_back( points_final[points_final.size()-1] );

  }


  this->SetType(dist);
}



//*************************************************  
void FitSpline::ReconfigureSpline(std::vector<int> dial_enums,
				  std::vector<double> dial_values){
//*************************************************
  
  for (unsigned int i = 0; i < var_enums.size(); i++){
    for (unsigned int j = 0; j < dial_enums.size(); j++){
      
      if (var_enums[i] == dial_enums[j]){
	x[i] = dial_values[j];

	if (x[i] > x_high[i]) x[i] = x_high[i];
	else if (x[i] < x_low[i]) x[i] = x_low[i];
	
      }
    }
  }
  
};

//*************************************************  
double FitSpline::operator()(const Double_t* x, const Double_t* par) const {
//*************************************************  
  double weight = this->DoEval(x,par, false);
  if (weight <= 0.0) weight = 0.0;
  return weight;
};

//*************************************************  
double FitSpline::DoEval(const double* par) const {
//*************************************************
  double weight = this->DoEval(&x[0], &par[0]);
  if (weight <= 0.0) weight = 0.0;
  return weight;
};

//*************************************************  
void FitSpline::SetType(int t, int p, int d, bool f){
//*************************************************  
  spline = t;
  npar = p;
  ndim = d;
  needs_fit = f;
}


//*************************************************

// Adding new spline functions
// 1. Add a new value to the enumeration in the header.
// 2. Setup a string comparison call here using
//      SetType( enum, ndim, npar );
// 3. Implement the function evaluation in the DoEval function.

//*************************************************  
void FitSpline::SetType(std::string type){
//*************************************************
  
  if      (!type.compare(  "1Dpol1"  )){ SetType( k1DPol1, 1, 1 ); }
  else if (!type.compare(  "1Dpol2"  )){ SetType( k1DPol2, 2, 1 ); }
  else if (!type.compare(  "1Dpol3"  )){ SetType( k1DPol3, 3, 1 ); }
  else if (!type.compare(  "1Dpol4"  )){ SetType( k1DPol4, 4, 1 ); }
  else if (!type.compare(  "1Dpol5"  )){ SetType( k1DPol5, 5, 1 ); }
  else if (!type.compare(  "1Dpol6"  )){ SetType( k1DPol6, 6, 1 ); }
  
  else if (!type.compare(  "1Dpol1C" )){ SetType( k1DPol1C, 2, 1 ); }
  else if (!type.compare(  "1Dpol2C" )){ SetType( k1DPol2C, 3, 1 ); }
  else if (!type.compare(  "1Dpol3C" )){ SetType( k1DPol3C, 4, 1 ); }
  else if (!type.compare(  "1Dpol4C" )){ SetType( k1DPol4C, 5, 1 ); }
  else if (!type.compare(  "1Dpol5C" )){ SetType( k1DPol5C, 6, 1 ); }

  else if (!type.compare( "1Dpol5C_LX" )){ SetType( k1DPol5C_LX, 7, 1 ); }

  else if (!type.compare( "1Dpol10" )){ SetType( k1DPol10, 10, 1 ); }
  else if (!type.compare( "1Dpol25" )){ SetType( k1DPol25, 25, 1 ); }

  else if (!type.compare( "2Dpol6"  )){ SetType( k2DPol6, 9, 2 ); } 
  else if (!type.compare( "1DTSpline3")){
    SetType( k1DTSpline3, x_vals[0].size()*4, 1, false );
  }
  else { std::cout << "Unkown dial value: "<<type<<std::endl; exit(-1); }
  return;
}

//*************************************************  
double FitSpline::DoEval(const double* x, const double* par, const bool use_offset) const {
//*************************************************

  // Fill Vector with limits
  std::vector<double> y = std::vector<double>(this->ndim, 0);
  for (UInt_t i = 0; i < ndim; i++){
    y[i] = x[i];
    if (x[i] < x_low[i])       y[i] = x_low[i];
    else if (x[i] > x_high[i]) y[i] = x_high[i];
  }

  // Get Offset in array
  int off = 0;
  if (use_offset) off = this->offset;
  switch( this->spline ){

    // N Polynomial sets without constant
  case (k1DPol1): {
    std::cout<<"Calcing pol1 with "<<y[0]<<" "<<par[0+off]<<std::endl;
    return ( 1.0 + par[0+off] * y[0] ); }
    
  case (k1DPol2): { return ( 1.0 + par[0+off] * y[0] +
			     par[1+off] * y[0] * y[0] ); }
    
  case (k1DPol3): { return ( 1.0 + par[0+off] * y[0] +
			     par[1+off] * y[0] * y[0] +
			     par[2+off] * y[0] * y[0] * y[0] );}
    
  case (k1DPol4): { return ( 1.0 + par[0+off] * y[0] +
			     par[1+off] * y[0] * y[0] +
			     par[2+off] * y[0] * y[0] * y[0] +
			     par[3+off] * pow(y[0],4) ); }
    
  case (k1DPol5): { return ( 1.0 + par[0+off] * y[0] +
			     par[1+off] * y[0] * y[0] +
			     par[2+off] * y[0] * y[0] * y[0] +
			     par[3+off] * pow(y[0],4) +
			     par[4+off] * pow(y[0],5) );  }

  case (k1DPol6): { return ( 1.0 + par[0+off] * y[0] +
			     par[1+off] * y[0] * y[0] +
			     par[2+off] * y[0] * y[0] * y[0] +
			     par[3+off] * pow(y[0],4) +
			     par[4+off] * pow(y[0],5) +
			     par[5+off] * pow(y[0],6) );  }
    
    // N Polynomial sets (with constant)
  case (k1DPol1C): { return ( par[0+off] +
			      par[1+off] * y[0] ); }
    
  case (k1DPol2C): { return ( par[0+off] +
			      par[1+off] * y[0] +
			      par[2+off] * y[0] * y[0] ); }
    
  case (k1DPol3C): { return ( par[0+off] +
			      par[1+off] * y[0] +
			      par[2+off] * y[0] * y[0] +
			      par[3+off] * y[0] * y[0] * y[0] ); }

  case (k1DPol4C): { return ( par[0+off] +
			      par[1+off] * y[0] +
			      par[2+off] * y[0] * y[0] +
			      par[3+off] * y[0] * y[0] * y[0] +
			      par[4+off] * pow(y[0],4) ); }

  case (k1DPol5C): { return ( par[0+off] +
			      par[1+off] * y[0] +
			      par[2+off] * y[0] * y[0] +
			      par[3+off] * y[0] * y[0] * y[0] +
			      par[4+off] * pow(y[0],4) +
			      par[5+off] * pow(y[0],5) );  }

  case (k1DPol5C_LX): { return ((par[0+off] +
				 par[1+off] * y[0] +
				 par[2+off] * y[0] * y[0] +
				 par[3+off] * y[0] * y[0] * y[0] +
				 par[4+off] * pow(y[0],4) +
				 par[5+off] * pow(y[0],5) ) *
				double(y[0] > par[6+off] - 4.0)); }

  case (k1DPol10): { return ((par[0+off] +
			      par[1+off] * y[0] +
			      par[2+off] * y[0] * y[0] +
			      par[3+off] * y[0] * y[0] * y[0] +
			      par[4+off] * pow(y[0],4) +
			      par[5+off] * pow(y[0],5)  +
			      par[6+off] * pow(y[0],6)  +
			      par[7+off] * pow(y[0],7)  +
			      par[8+off] * pow(y[0],8)  +
			      par[9+off] * pow(y[0],9))); }

  // Silly 25th order polynomial (ROOT can't fit this anyway)
  case (k1DPol25): {
    double weight = par[0+off];
    for (UInt_t i = 1; i < npar; i++){
      weight += par[i+off] * pow(y[0],i);
    }
    return weight;
  }

  case (k2DPol6):{
    double weight = 1.0;
    return weight;
  }
    
    // TSpline3 --- REQUIRES COEFF BE FILLED into spline3
  case (k1DTSpline3): {
 
    // iterator over knot values and find width
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
  }

    // DEFAULT OPTION
  default:
    std::cerr<<"Evaluating function enum not found!"<<std::endl;
    exit(-1);
    return 1.0;
  }
};


std::vector<double> FitSpline::GetSplineCoeff(double* weights){

  std::vector<double> coeff;

  switch(this->spline){


  // TSPLINE 3 DIAL COEFFICIENT SET -----------------
  case (k1DTSpline3): {

    std::vector<double> xval;
    std::vector<double> yval;

    // CREATE TEMP OBJECT
    TSpline3 temp_spline = TSpline3("temp_spline", &((x_vals[0])[0]), weights, x_vals[0].size());
    for (UInt_t i = 0; i < x_vals[0].size(); i++){


      /// EXTRA ZERO SUPPRESSION, we must save co-effecients for each point scanned
      /// to remain consistent across events. (-1) flag is used 
      if (weights[i] <= 0.0 and i < x_vals[0].size()-1 and weights[i+1] <= 0.0){

	coeff.push_back(  0.0 );
	coeff.push_back(  0.0 );
	coeff.push_back(  0.0 );
	coeff.push_back(  0.0 );

      /// Get Normal Coefficients for the point
      } else {
      
	coeff.push_back(weights[i]);
	double x,y, b,c,d;
	
	temp_spline.GetCoeff(i, x, y, b, c, d);
	
	coeff.push_back( b );
	coeff.push_back( c );
	coeff.push_back( d );
      
      }
    }
    break;
  }

  /// DEFAULT
  default:
    std::cout<<"DIAL HAS NO COEFFICIENT SET Implementation"<<std::endl;
    std::cout<<"Dial Enum = "<<this->spline<<std::endl;
    exit(-1);
    break;
  }

  
  if (coeff.empty()){
    std::cerr<<"Error no dial coefficients saved from GetDialCoeff()"<<std::endl;
    exit(-1);
  }
  
  return coeff;
}
