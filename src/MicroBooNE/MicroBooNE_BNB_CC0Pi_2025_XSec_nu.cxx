// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <fstream>
#include <set>

#include "MicroBooNE_BNB_CC0Pi_2025_XSec_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TMatrixD.h"

namespace {
  constexpr int MASS_NUMBER_40AR = 40;
  constexpr int PROTON = 2212;
  constexpr int MU_MINUS = 13;

  constexpr double TARGET_MASS = 37.215526; // 40Ar, GeV
  constexpr double NEUTRON_MASS = 0.93956541; // GeV
  constexpr double PROTON_MASS = 0.93827208; // GeV
  constexpr double MUON_MASS = 0.10565837; // GeV
  constexpr double BINDING_ENERGY = 0.02478; // 40Ar, GeV

/*
  void compute_stvs( const TVector3& p3mu, const TVector3& p3p,
    double& delta_pT, double& delta_phiT, double& delta_alphaT,
    double& delta_pL, double& pn, double& delta_pTx, double& delta_pTy )
  {
    delta_pT = (p3mu + p3p).Perp();

    delta_phiT = std::acos( (-p3mu.X()*p3p.X() - p3mu.Y()*p3p.Y())
      / (p3mu.XYvector().Mod() * p3p.XYvector().Mod()) );

    TVector2 delta_pT_vec = (p3mu + p3p).XYvector();
    delta_alphaT = std::acos( (-p3mu.X()*delta_pT_vec.X()
      - p3mu.Y()*delta_pT_vec.Y())
      / (p3mu.XYvector().Mod() * delta_pT_vec.Mod()) );

    double Emu = std::sqrt(std::pow(MUON_MASS, 2) + p3mu.Mag2());
    double Ep = std::sqrt(std::pow(PROTON_MASS, 2) + p3p.Mag2());
    double R = TARGET_MASS + p3mu.Z() + p3p.Z() - Emu - Ep;

    // Estimated mass of the final remnant nucleus (CCQE assumption)
    double mf = TARGET_MASS - NEUTRON_MASS + BINDING_ENERGY;
    delta_pL = 0.5*R - (std::pow(mf, 2) + std::pow(delta_pT, 2)) / (2.*R);

    pn = std::sqrt( std::pow(delta_pL, 2) + std::pow(delta_pT, 2) );

    // Components of the 2D delta_pT vector (see arXiv:1910.08658)

    // We assume that the neutrino travels along the +z direction (also done
    // in the other expressions above)
    TVector3 zUnit( 0., 0., 1. );

    // Defines the x direction for the components of the delta_pT vector
    TVector2 xTUnit = zUnit.Cross( p3mu ).XYvector().Unit();

    delta_pTx = xTUnit.X()*delta_pT_vec.X() + xTUnit.Y()*delta_pT_vec.Y();

    // Defines the y direction for the components of the delta_T vector
    TVector2 yTUnit = ( -p3mu ).XYvector().Unit();

    delta_pTy = yTUnit.X()*delta_pT_vec.X() + yTUnit.Y()*delta_pT_vec.Y();
  }

  double compute_stvs( FitEvent* ev, const std::string& var ) {
    TVector3 p3mu = ev->GetHMFSParticle( MU_MINUS )->fP.Vect();
    TVector3 p3p = ev->GetHMFSParticle( PROTON )->fP.Vect();

    // Convert to GeV
    p3mu *= 1e-3;
    p3p *= 1e-3;

    double delta_pT, delta_phiT, delta_alphaT, delta_pL;
    double pn, delta_pTx, delta_pTy;

    compute_stvs( p3mu, p3p, delta_pT, delta_phiT, delta_alphaT,
      delta_pL, pn, delta_pTx, delta_pTy );

    if ( var == "delta_pT" ) return delta_pT;
    else if ( var == "delta_phiT" ) return delta_phiT;
    else if ( var == "delta_alphaT" ) return delta_alphaT;
    else if ( var == "delta_pL" ) return delta_pL;
    else if ( var == "pn" ) return pn;
    else if ( var == "delta_pTx" ) return delta_pTx;
    else if ( var == "delta_pTy" ) return delta_pTy;
    return 0.;
  }
*/
/*
std::map<int, double> BinWidthMap = {
 {1,  0.07},
 {2,  0.03},
 {3,  0.85},
 {4,  0.07},
 {5,  0.03},
 {6,  0.04},
 {7,  0.81},
 {8,  0.0378},
 {9,  0.0162},
 {10, 0.0216},
 {11, 0.4374},
 {12, 0.018},
 {13, 0.018},
 {14, 0.0324},
 {15, 0.2736},
 {16, 0.0238},
 {17, 0.0238},
 {18, 0.017},
 {19, 0.2584},
 {20, 0.0196},
 {21, 0.0196},
 {22, 0.014},
 {23, 0.2128},
 {24, 0.014},
 {25, 0.014},
 {26, 0.01},
 {27, 0.02},
 {28, 0.132},
 {29, 0.0304},
 {30, 0.016},
 {31, 0.0136},
 {32, 0.092},
 {33, 0.0228},
 {34, 0.012},
 {35, 0.0102},
 {36, 0.0258},
 {37, 0.04325}
};
*/
std::map<int, double> BinWidthMap = {

{1,   0.063},
{2,   0.077},
{3,   0.063},
{4,   0.077},
{5,   0.027},
{6,   0.018},
{7,   0.015},
{8,   0.015},
{9,   0.012},
{10,   0.015},
{11,   0.018},
{12,   0.048},
{13,   0.024},
{14,   0.016},
{15,   0.02},
{16,   0.012},
{17,   0.016},
{18,   0.012},
{19,   0.012},
{20,   0.1},
{21,   0.05},
{22,   0.015},
{23,   0.015},
{24,   0.012},
{25,   0.008},
{26,   0.33},
{27,   0.033},
{28,   0.033},
{29,   0.0165},
{30,   0.0165},
{31,   0.011},
{32,   0.28125},
{33,   0.01125},
{34,   0.0075},
{35,   2.185},
{36,   0.0575},
{37,   0.0575}

};


}

                                            
  MicroBooNE_BNB_CC0Pi_2025_XSec_nu::MicroBooNE_BNB_CC0Pi_2025_XSec_nu( nuiskey samplekey )
{

  std::cout<<"inside::MicroBooNE_BNB_CC0Pi_2025_XSec_nu"<< std::endl;
  
  fSettings = LoadSampleSettings( samplekey );
  std::string name = fSettings.GetS( "name" );

   std::cout<<"Input Sample Name: "<< name<< std::endl;

  // The main histograms use the bin number on the x-axis

  std::string data_file_name;
  std::string binning_file_name;
  std::string Type; 

  if ( !name.compare("MicroBooNE_BNB_CC0Pi_2025_XSec_2D_nu") ) {
      is2D = true; 
      fSettings.SetYTitle( "#sigma (cm^{2}/^{40}Ar)" );
      fSettings.SetXTitle( "bin number" );
    
       data_file_name = FitPar::GetDataBase()+  "MicroBooNE/CC0pi_2025/2D/Extracted2DCrossSection_MicroBooNE_CC0PiSelection_BinningScheme1.root" ;
    
       binning_file_name =  FitPar::GetDataBase() + "MicroBooNE/CC0pi_2025/2D/bin_defsCC0pi_2025.txt" ;
    
    Type = "2D Costheta vs MuonMomentum : 37 bins";
  }
  else if (!name.compare("MicroBooNE_BNB_CC0Pi_2025_XSec_1Dpmu_nu")) {
        is2D = false; 
  
        data_file_name = FitPar::GetDataBase()+ "MicroBooNE/CC0pi_2025/1D/Extracted1DCrossSection_MicroBooNE_CC0PiSelection_2025_MuonMomentum.root" ;
  
        binning_file_name =  FitPar::GetDataBase()+ "MicroBooNE/CC0pi_2025/1D/Muon_1D_CC0Pi_2025_bining_MuonMomentum.txt" ;

        Type = "1D MuonMomentum : 14 bins";
    
  } 
   else if (!name.compare("MicroBooNE_BNB_CC0Pi_2025_XSec_1Dcostheta_nu")) {
    is2D = false; 
  
     data_file_name =  FitPar::GetDataBase()
    +  "MicroBooNE/CC0pi_2025/1D/Extracted1DCrossSection_MicroBooNE_CC0PiSelection_2025_MuonCosTheta.root" ;
    
     binning_file_name =  FitPar::GetDataBase()
    + "MicroBooNE/CC0pi_2025/1D/Muon_1D_CC0Pi_2025_bining_MuonCosTheta.txt" ;
    
    Type = "1D Costheta : 29 bins";
    
  }
  
  else {
  std::cout<<" DIDN't FIND Name :  "<< name << std::endl; 
    assert( false );
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "reference: https://arxiv.org/abs/2507.00921\n" \
                        "Signal: CC0pi (yr 2025)\n" \
                         + Type + "\n";
                        
  std::cout<<"descrip - Sample:: "<< descrip<<std::endl;

  fSettings.SetDescription( descrip );
  fSettings.SetTitle( name );
  fSettings.SetAllowedTypes( "FULL", "FIX/FULL" );
  fSettings.SetEnuRange( 0.0, 10 ); /// using range we flux intergered 
  fSettings.DefineAllowedTargets( "Ar" );
  fSettings.DefineAllowedSpecies( "numu" );
  FinaliseSampleSettings();

  // Scale factor for the flux-averaged total cross section
  // (10^{-38} cm^2 / Ar) in each bin
  fScaleFactor = GetEventHistogram()->Integral( "width" )
    * MASS_NUMBER_40AR / fNEvents / TotalIntegratedFlux();

  // Get bin definitions
  this->LoadBinDefinitions(binning_file_name);

  //std::string data_file_name( FitPar::GetDataBase()
  //  +  "MicroBooNE/CC0pi_2025/2D/Extracted2DCrossSection_MicroBooNE_CC0PiSelection_BinningScheme1.root" );
  if (is2D) {
    std::cout << "is2D is set to true (2D mode enabled)." << std::endl;
   } else {
       std::cout << "is2D is set to false (1D mode enabled)." << std::endl;
   }

    
  std::cout<<"Getting Root file from: "<< data_file_name<< std::endl;

  // Load the additional smearing matrix
  TMatrixD* A_C = StatUtils::GetMatrixFromRootFile( data_file_name,
    "AC_Matrix" );
  fAddSmear.reset( A_C );

  // Load the measured data points
  fDataHist = PlotUtils::GetTH1DFromRootFile( data_file_name,
    "h_ExtractedCrossSectionUnfoldedData" );
   bool check_fDataHist = CheckHist(fDataHist);
   
    //BinWidth_norm_2D(fDataHist);

   
    if (!check_fDataHist) {
     std::cout<<"~~ERROR~~ Something wrong with fDataHist"<< std::endl; 
   }
   else { std::cout<<"Passed:fDataHist"<< std::endl; }
    
    fDataHist->SetNameTitle( (fSettings.GetName() + "_data").c_str(),
    fSettings.GetFullTitles().c_str() );

  // Also retrieve the total covariance matrix and its inverse (pre-computed in
  // the data release ROOT file for convenience)
  //this->SetCovarFromRootFile( data_file_name, "cov_total" );
  fFullCovar = StatUtils::GetCovarFromRootFile( data_file_name, "TotalCov_AfterUnfolding" );
  //covar = StatUtils::GetCovarFromRootFile( data_file_name,
  //  "inverse_cov_total" );
   covar = StatUtils::GetInvert(fFullCovar, true);

  //fDecomp = StatUtils::GetDecomp( fFullCovar );
  TDecompChol chol( *fFullCovar );
  chol.Decompose();
  fDecomp = new TMatrixDSym( fFullCovar->GetNrows(),
    chol.GetU().GetMatrixArray(), "" );

  // Push the diagonals of fFullCovar onto the data histogram
  //StatUtils::SetDataErrorFromCov( fDataHist, fFullCovar, 1.0, false );

  // Setup fMCHist from data
  fMCHist = dynamic_cast< TH1D* >( fDataHist->Clone() );
  fMCHist->SetNameTitle( (fSettings.GetName() + "_MC").c_str(),
   fSettings.GetFullTitles().c_str() );
  fMCHist->Reset();

  fMCStat = dynamic_cast< TH1D* >( fMCHist->Clone() );
  
   bool check_fMCHist = CheckHist(fMCHist);
 
    if (!check_fMCHist) {
     std::cout<<"~~ERROR~~ Something wrong with fMCHist"<< std::endl; 
   }
   else { std::cout<<"Passed:fMCHist"<< std::endl; }
    
  
  fMCStat->Reset();

  // Since we're using a 1D histogram with bin number along the x-axis, it
  // doesn't make sense to subdivide the bins. Rather than doing that
  // automatically, I just copy the original data histogram binning here. Thus,
  // MCFine ends up using the same bins as regular MC.
  fMCFine = dynamic_cast< TH1D* >( fMCHist->Clone() );
  fMCFine->SetNameTitle( (fSettings.GetName() + "_MC_FINE").c_str(),
    fSettings.GetFullTitles().c_str() );
  fMCFine->Reset();

  // Set up the MC modes histogram
  fMCHist_Modes = new TrueModeStack( (fSettings.GetName() + "_MODES").c_str(),
    "True Channels", fMCHist );
  fMCHist_Modes->SetTitleX( fDataHist->GetXaxis()->GetTitle() );
  fMCHist_Modes->SetTitleY( fDataHist->GetYaxis()->GetTitle() );
  this->SetAutoProcessTH1( fMCHist_Modes, kCMD_Reset, kCMD_Norm, kCMD_Write );

  //this->FinaliseMeasurement();
}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
bool MicroBooNE_BNB_CC0Pi_2025_XSec_nu::isSignal( FitEvent* event ) {
   return SignalDef::MicroBooNE::isCC1Mu0pi_2025(event, EnuMin, EnuMax);
}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::FillEventVariables( FitEvent* event ) {

  // Clear out the vector of passing bins, which may have already been filled
  // for the previous event
  fPassingBins.clear();

  //if ( event->NumFSParticle(MU_MINUS) == 0 ) return;
   if(isSignal( event )==false) return; 

  // Loop over each of the bin definitions. Keep track of the bins that
  // pass all cuts (and should thus be filled in this event)
  for ( size_t b = 0u; b < fBinDefinitions.size(); ++b ) {

    // Start out by assuming that the event passes all cuts
    bool passed_cuts = true;

    // Apply all cuts
    const auto& my_cuts = fBinDefinitions.at( b );
    for ( const auto& cut : my_cuts ) {
      passed_cuts &= cut.evaluate( event );
    }

    // If all cuts were passed, add this bin to the vector of indices for bins
    // that should be filled
    if ( passed_cuts ) fPassingBins.push_back( b );

  } // loop over bins

}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::LoadBinDefinitions() {

  std::cout<<"inside::LoadBinDefinitions"<<std::endl;

  std::cout<<"FitPar::GetDataBase() = " << FitPar::GetDataBase() << std::endl;

  std::string binning_file_name( FitPar::GetDataBase()
    + "MicroBooNE/CC0pi_2025/2D/bin_defsCC0pi_2025.txt" );

  std::ifstream bin_file( binning_file_name );
  std::string dummy_str;
  int bin_type, dummy_int;
  size_t num_true_bins;

  // TODO: add I/O error handling hereg
  // Skip the output TDirectoryFile name and the input TTree name
  bin_file >> dummy_str >> dummy_str >> num_true_bins;

  const std::string delimiter( "&&" );
  for ( size_t tb = 0u; tb < num_true_bins; ++tb ) {

    // Skip the true bin type and block index
    bin_file >> bin_type >> dummy_int;

    // Use two calls to std::getline using a double quote delimiter
    // in order to get the contents of the next double-quoted string
    std::string bin_def;
    std::getline( bin_file, bin_def, '\"' );
    std::getline( bin_file, bin_def, '\"' );

    // Skip bins of type == 1 (background true bins)
    if ( bin_type == 1 ) continue;

    // Skip the text before the first "&&" (here we assume that it is a simple
    // bool for the signal definition)
    std::string all_cuts = bin_def.substr(
      bin_def.find(delimiter) + delimiter.length() );

    // Create an empty vector of MyCut objects to start defining the new bin
    fBinDefinitions.emplace_back();

    // Loop over the rest of the cuts and check each one
    size_t pos = 0u;
    do {
      // Advance to the next individual cut, removing it from the temporary
      // copy of the list of cuts
      pos = all_cuts.find( delimiter );
      std::string cut = all_cuts.substr( 0, pos );
      all_cuts.erase( 0, pos + delimiter.length() );

      // Parse the cut
      std::string var_name;
      std::string comp_op;
      double cut_val;
      std::stringstream cut_ss( cut );

      cut_ss >> var_name >> comp_op >> cut_val;

      std::cout<<"inputs for fBinDefinitions  var_name  = " << var_name<< " comp_op = "<< comp_op << " cu_val =  "<< cut_val<< std::endl;

      // Interpret the variable for this cut and set up a function
      // object that will calculate it from the input FitEvent
      std::function< double(FitEvent*) > getter;
       if ( var_name == "mc_p3_mu.Mag()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().Mag() / 1e3; // GeV
        };
      }
      else if ( var_name == "mc_p3_mu.CosTheta()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().CosTheta();
        };
      }
      else NUIS_ABORT( "Unrecognized cut variable " + var_name );
     
      // Test the cut based on the appropriate comparison operator
      std::function< bool(double) > tester;
      if ( comp_op == ">=" ) {
        tester = [cut_val]( double x ) -> bool {
          return x >= cut_val;
        };
      }
      else if ( comp_op == "<" ) {
        tester = [cut_val]( double x ) -> bool {
          return x < cut_val;
        };
      }
      else NUIS_ABORT( "Unrecognized comparison operator " + comp_op );

      // Add the finished cut to the definition for the current bin

      fBinDefinitions.back().emplace_back( getter, tester );

    } while ( pos != std::string::npos );

  } // loop over true bins

}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::LoadBinDefinitions(std::string binning_file_name) {


  std::cout<<"inside::LoadBinDefinitions"<<std::endl;

  std::cout<<"FitPar::GetDataBase() = " << FitPar::GetDataBase() << std::endl;
  std::cout<<"Using Binning file :  " << binning_file_name << std::endl;

  std::ifstream bin_file( binning_file_name );
  std::string dummy_str;
  int bin_type, dummy_int;
  size_t num_true_bins;

  // TODO: add I/O error handling hereg
  // Skip the output TDirectoryFile name and the input TTree name
  bin_file >> dummy_str >> dummy_str >> num_true_bins;

 std::cout<<" num_true_bins = "<< num_true_bins << std::endl;

  const std::string delimiter( "&&" );
  for ( size_t tb = 0u; tb < num_true_bins; ++tb ) {

    // Skip the true bin type and block index
    bin_file >> bin_type >> dummy_int;

    // Use two calls to std::getline using a double quote delimiter
    // in order to get the contents of the next double-quoted string
    std::string bin_def;
    std::getline( bin_file, bin_def, '\"' );
    std::getline( bin_file, bin_def, '\"' );

    // Skip bins of type == 1 (background true bins)
    if ( bin_type == 1 ) continue;

    // Skip the text before the first "&&" (here we assume that it is a simple
    // bool for the signal definition)
    std::string all_cuts = bin_def.substr(
      bin_def.find(delimiter) + delimiter.length() );

    // Create an empty vector of MyCut objects to start defining the new bin
    fBinDefinitions.emplace_back();

    // Loop over the rest of the cuts and check each one
    size_t pos = 0u;
    do {
      // Advance to the next individual cut, removing it from the temporary
      // copy of the list of cuts
      pos = all_cuts.find( delimiter );
      std::string cut = all_cuts.substr( 0, pos );
      all_cuts.erase( 0, pos + delimiter.length() );

      // Parse the cut
      std::string var_name;
      std::string comp_op;
      double cut_val;
      std::stringstream cut_ss( cut );

      cut_ss >> var_name >> comp_op >> cut_val;

      std::cout<<"inputs for fBinDefinitions  var_name  = " << var_name<< " comp_op: "<< comp_op << " cu_val: "<< cut_val<< std::endl;

      // Interpret the variable for this cut and set up a function
      // object that will calculate it from the input FitEvent
      std::function< double(FitEvent*) > getter;
       if ( var_name == "mc_p3_mu.Mag()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().Mag() / 1e3; // GeV
        };
      }
      else if ( var_name == "mc_p3_mu.CosTheta()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().CosTheta();
        };
      }
      else NUIS_ABORT( "Unrecognized cut variable " + var_name );
     
      // Test the cut based on the appropriate comparison operator
      std::function< bool(double) > tester;
      if ( comp_op == ">=" ) {
        tester = [cut_val]( double x ) -> bool {
          return x >= cut_val;
        };
      }
      else if ( comp_op == "<" ) {
        tester = [cut_val]( double x ) -> bool {
          return x < cut_val;
        };
      }
      else NUIS_ABORT( "Unrecognized comparison operator " + comp_op );

      // Add the finished cut to the definition for the current bin

      fBinDefinitions.back().emplace_back( getter, tester );

    } while ( pos != std::string::npos );

  } // loop over true bins

}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::FillHistograms() {

  if ( !Signal ) return;

  // The histograms have bin number as the x-axis variable, so just fill them
  // using the bin index of each bin that passed all cuts
  for ( const auto& bin : fPassingBins ) {
    NUIS_LOG(DEB, "Fill MCHist: " << bin << ", " << Weight);

    fMCHist->Fill( bin, Weight );
    fMCStat->Fill( bin, 1.0 );
    if ( fMCHist_Modes ) fMCHist_Modes->Fill( Mode, bin, Weight );

    fMCFine->Fill( bin, Weight );
    if ( fMCFine_Modes ) fMCFine_Modes->Fill( Mode, bin, Weight );
  }

}


void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::ConvertEventRates() {
  // Do the standard conversion
  Measurement1D::ConvertEventRates();

  // TODO: Restore use of A_C once you make sure you aren't double-counting it
  //// Build a column vector using the predicted cross sections

  
  int num_bins = fMCHist->GetNbinsX();
  
  TMatrixD pred( num_bins, 1 );
  
  for ( int b = 0; b < num_bins; ++b ) {
    pred( b, 0 ) = fMCHist->GetBinContent( b + 1 );
  }

  //// Apply the additional smearing matrix to create a new prediction
   TMatrixD new_pred( *fAddSmear, TMatrixD::kMult, pred ); //turning this off 
  
  //// Update the MC prediction histogram with the smeared version
  // Also BinWith norm

if(is2D == true){
for ( int b = 0; b < num_bins; ++b ) {
    double xsec = new_pred( b, 0 );
    fMCHist->SetBinContent( b + 1, xsec / BinWidthMap[b + 1]); // / BinWidthMap[b + 1] 
  }
}
else {
for ( int b = 0; b < num_bins; ++b ) {
    double xsec = new_pred( b, 0 );
    fMCHist->SetBinContent( b + 1, xsec); // 
  }
    //fMCHist->Scale(1.0,"width"); // binwidth norm 1D 
}

  
}

double MicroBooNE_BNB_CC0Pi_2025_XSec_nu::GetLikelihood() {

  if ( fNoData || !fDataHist ) return 0.;

   std::cout<<"inside ::MicroBooNE_BNB_CC0Pi_2025_XSec_nu::GetLikelihood "<< std::endl;
   
   auto CheckingfDataHist = CheckHist(fDataHist );
   auto CheckingMCHist = CheckHist(fMCHist );
    
   
  // Apply Masking to MC if Required.
  if ( fIsMask and fMaskHist ) {
    NUIS_ERR(FTL, "Bin masks not yet supported by"
      " the MicroBooNE_BNB_CC0Pi_2025_XSec_nu sample" );
    //PlotUtils::MaskBins(fMCHist, fMaskHist);
  } 
  
  
  

  // Likelihood Calculation
  double stat = 0.;
  if ( fIsChi2 ) {
    if ( fIsNS ) {
      NUIS_ERR(FTL, "Norm-shape covariance not yet supported by"
        " the MicroBooNE_BNB_CC0Pi_2025_XSec_nu sample" );
    }
    stat = StatUtils::GetChi2FromCov( fDataHist, fMCHist, covar, NULL, 1.0,
      1.0, fIsWriting ? fResidualHist : NULL );
  }

  fLikelihood = stat;

  return stat;
}


void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::BinWidth_norm_2D(TH1D *input){

int num_bins = input->GetNbinsX();
  for ( int b = 0; b < num_bins; ++b ) {
    double output = input->GetBinContent( b + 1 );
     input->SetBinContent( b + 1, output/BinWidthMap[b + 1]);
  }

}