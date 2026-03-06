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

/////////////////////////////////////////////////////////////////////
// 2D Binning Map for Bin Width Norming when the input is a function of Bin Number 
/////////////////////////////////////////////////////////////////////

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
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////                                            
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
    
       Type = "2D Costheta vs MuonMomentum (bin widths defined by Bin Number) : 37 bins";
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
    
    Type = "1D Costheta : (29 bins)";
    
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
  fScaleFactor =  MASS_NUMBER_40AR * GetEventHistogram()->Integral( "width" ) / (double)fNEvents / TotalIntegratedFlux();
  
    

  // Get bin definitions
  this->LoadBinDefinitions(binning_file_name);

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

  if ( event->NumFSParticle(MU_MINUS) == 0 ) return;
   //if(isSignal( event )==false) return; 

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

  // The histograms have bin number as the x-axis variable in 2D, so just fill them
  // using the bin index of each bin that passed all cuts
  // Added hack for 1D - input 1D histograms aren't a function of bin number 
  //therefore as a workaround :  from bin number to bin center , filling value used are bin center
  
  for ( const auto& bin : fPassingBins ) {
   if(is2D){
  
    NUIS_LOG(DEB, "Fill MCHist: " << bin << ", " << Weight);
    //std::cout<<"bin "<< bin << std::endl;
    fMCHist->Fill( bin, Weight );
    fMCStat->Fill( bin, 1.0 );
    if ( fMCHist_Modes ) fMCHist_Modes->Fill( Mode, bin, Weight );
    fMCFine->Fill( bin, Weight );
    if ( fMCFine_Modes ) fMCFine_Modes->Fill( Mode, bin, Weight );
  }
   else{
    double bin_center = fMCHist->GetXaxis()->GetBinCenter( bin + 1 );
    NUIS_LOG(DEB, "Fill MCHist: " << bin << ", " << Weight);
    //std::cout<<"bin "<< bin << std::endl;
    fMCHist->Fill( bin_center, Weight );
    fMCStat->Fill( bin_center, 1.0 );
    if ( fMCHist_Modes ) fMCHist_Modes->Fill( Mode, bin_center, Weight );
    fMCFine->Fill( bin, Weight );
    if ( fMCFine_Modes ) fMCFine_Modes->Fill( Mode, bin_center, Weight );
   } 
   
  }

}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void MicroBooNE_BNB_CC0Pi_2025_XSec_nu::ConvertEventRates() {
  // Do the standard conversion
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
     //std::cout<<"After add smearing  :: "<< new_pred( b, 0 ) << std::endl;
     fMCHist->SetBinContent( b + 1, xsec); // / BinWidthMap[b + 1] 
   }
 }
 else {
 for ( int b = 0; b < num_bins; ++b ) {
     double xsec = new_pred( b, 0 );
     //std::cout<<"After add smearing  :: "<< new_pred( b, 0 ) << std::endl;
     fMCHist->SetBinContent( b + 1, xsec); // 
   }
 
 }

 //
//////////////////////////////////////
// Assuming that 2D is a function of bin num , and 1D is not therefore , and extra step is need to bin width Norm the 2D , 
///////////////////////////////////
 Measurement1D::ConvertEventRates();

 if(is2D == true){
  for ( int b = 0; b < num_bins; ++b ) {
    double xsec = fMCHist->GetBinContent(b+1);
    fMCHist->SetBinContent( b + 1, xsec / BinWidthMap[b + 1]); // / BinWidthMap[b + 1] 
  }
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
