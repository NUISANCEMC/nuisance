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

#include "InteractionModes.h"
#include "MicroBooNEBlockHandler.h"
#include "MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu.h"
#include "TMatrixD.h"

// Anonymous namespace
namespace {
  constexpr int MASS_NUMBER_40AR = 40;
  constexpr int PROTON = 2212;
  constexpr int MU_MINUS = 13;

  constexpr double TARGET_MASS = 37.215526; // 40Ar, GeV
  constexpr double NEUTRON_MASS = 0.93956541; // GeV
  constexpr double PROTON_MASS = 0.93827208; // GeV
  constexpr double MUON_MASS = 0.10565837; // GeV
  constexpr double BINDING_ENERGY = 0.02478; // 40Ar, GeV

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

  // Loads a matrix from a text file following the format used
  // in the data release for this MicroBooNE measurement
  TMatrixD load_matrix( const std::string& input_file_name ) {

    // Get the table of matrix element values
    std::ifstream matrix_table_file( input_file_name );

    // Peek at the file contents to decide whether we're working with
    // a matrix or a column vector
    std::string dummy;
    matrix_table_file >> dummy >> dummy >> dummy;
    bool is_matrix = ( dummy == "numYbins" );

    // Return to the beginning of the file for parsing
    matrix_table_file.seekg( 0 );

    // Get the matrix or vector dimensions from the header line(s)
    int num_x_bins, num_y_bins;

    matrix_table_file >> dummy >> num_x_bins;
    if ( is_matrix ) {
      matrix_table_file >> dummy >> num_y_bins;

      // Skip the next header line which contains the data column names
      std::getline( matrix_table_file, dummy );
    }
    else {
      num_y_bins = 1;
    }

    // Create a TMatrixD with the correct dimensions
    TMatrixD matrix( num_x_bins, num_y_bins );

    // Parse its contents from the remaining lines
    std::string line;
    while ( std::getline(matrix_table_file, line) ) {
      int bin1, bin2;
      double element;

      std::stringstream temp_ss( line );
      temp_ss >> bin1;
      if ( is_matrix ) {
        temp_ss >> bin2;
      }
      else {
        bin2 = 0;
      }
      temp_ss >> element;

      if ( bin1 < num_x_bins && bin2 < num_y_bins ) {
        matrix( bin1, bin2 ) = element;
      }
    }

    return matrix;
  }

  // Helper function that converts a TMatrixD into the TMatrixDSym*
  // needed to initialize some class members inherited from Measurement1D.
  // Assumes that the input is a square matrix (TODO: add error handling).
  TMatrixDSym* to_symmetric_matrix( const TMatrixD& mat ) {
    int num_rows = mat.GetNrows();
    TMatrixDSym* sym = new TMatrixDSym( num_rows );
    for ( int a = 0; a < num_rows; ++a ) {
      for ( int b = 0; b < num_rows; ++b ) {
        sym->operator()( a, b ) = mat( a, b );
      }
    }
    return sym;
  }

  // Helper function that creates a TH1D from a TMatrixD. Currently
  // assumes that the input matrix has a single column (TODO: Add
  // error handling)
  TH1D* to_histogram( const TMatrixD& vec ) {
    int num_rows = vec.GetNrows();
    auto* hist = new TH1D( "vec_hist", "", num_rows, 0., num_rows );
    for ( int a = 0; a < num_rows; ++a ) {
      double value = vec( a, 0 );
      hist->SetBinContent( a + 1, value ); // ROOT bin indices are one-based
    }
    return hist;
  }

}

MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu
  ::MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu( nuiskey samplekey )
{
  fSettings = LoadSampleSettings( samplekey );
  std::string name = fSettings.GetS( "name" );

  // The main histograms use the bin number on the x-axis
  fSettings.SetXTitle( "bin number" );

  // They also report total cross sections in each bin (not differential
  // cross sections divided by the bin width)
  fSettings.SetYTitle("#sigma (cm^{2}/^{40}Ar)");

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "Signal: CC1MuNp\n"
                        "Contact: microboone_info@fnal.gov\n"
                        "Reference: Phys. Rev. D 112, 112004 (2025)\n"
                        "DOI: https://doi.org/10.1103/8v2y-l89l\n";

  fSettings.SetDescription( descrip );
  fSettings.SetTitle( name );
  fSettings.SetAllowedTypes( "FULL", "FIX/FULL" );
  fSettings.SetEnuRange( 0.0, 6.8 );
  fSettings.DefineAllowedTargets( "Ar" );
  fSettings.DefineAllowedSpecies( "numu" );
  FinaliseSampleSettings();

  // Scale factor for the flux-averaged total cross section
  // (10^{-38} cm^2 / Ar) in each bin
  fScaleFactor = GetEventHistogram()->Integral( "width" )
    * MASS_NUMBER_40AR / fNEvents / TotalIntegratedFlux();

  // Get bin definitions
  this->LoadBinDefinitions();

  std::string file_name_AC( FitPar::GetDataBase()
    + "/MicroBooNE/CC1MuNp/2025/mat_table_add_smear.txt" );

  // Load the additional smearing matrix
  TMatrixD A_C = load_matrix( file_name_AC );
  fAddSmear = std::make_shared< TMatrixD >( A_C );

  // Load the measured data points
  std::string file_name_data( FitPar::GetDataBase()
    + "/MicroBooNE/CC1MuNp/2025/vec_table_unfolded_signal.txt" );
  TMatrixD temp_data_mat = load_matrix( file_name_data );
  fDataHist = to_histogram( temp_data_mat );

  fDataHist->SetNameTitle( (fSettings.GetName() + "_data").c_str(),
    fSettings.GetFullTitles().c_str() );

  // Also retrieve the total covariance matrix for the measurement
  std::string cov_file_name( FitPar::GetDataBase()
    + "/MicroBooNE/CC1MuNp/2025/mat_table_cov_total.txt" );
  auto temp_cov_matrix = load_matrix( cov_file_name );
  fFullCovar = to_symmetric_matrix( temp_cov_matrix );

  // Now invert the covariance matrix and store the result
  temp_cov_matrix.Invert();
  covar = to_symmetric_matrix( temp_cov_matrix );

  //fDecomp = StatUtils::GetDecomp( fFullCovar );
  TDecompChol chol( *fFullCovar );
  chol.Decompose();
  fDecomp = new TMatrixDSym( fFullCovar->GetNrows(),
    chol.GetU().GetMatrixArray(), "" );

  // Push the diagonals of fFullCovar onto the data histogram
  StatUtils::SetDataErrorFromCov( fDataHist, fFullCovar, 1.0, false );

  // Setup fMCHist from data
  fMCHist = dynamic_cast< TH1D* >( fDataHist->Clone() );
  fMCHist->SetNameTitle( (fSettings.GetName() + "_MC").c_str(),
   fSettings.GetFullTitles().c_str() );
  fMCHist->Reset();

  fMCStat = dynamic_cast< TH1D* >( fMCHist->Clone() );
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

bool MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::isMesonOrAntimeson( int pdg_code ) {
  // Ignore differences between mesons and antimesons for this test. Mesons
  // will have positive PDG codes, while antimesons will have negative ones.
  int abs_pdg = std::abs( pdg_code );

  // Meson PDG codes have no more than seven digits. Seven-digit
  // codes beginning with "99" are reserved for generator-specific
  // particles
  if ( abs_pdg >= 9900000 ) return false;

  // Mesons have a value of zero for $n_{q1}$, the thousands digit
  int thousands_digit = ( abs_pdg / 1000 ) % 10;
  if ( thousands_digit != 0 ) return false;

  // They also have a nonzero value for $n_{q2}$, the hundreds digit
  int hundreds_digit = ( abs_pdg / 100 ) % 10;
  if ( hundreds_digit == 0 ) return false;

  // Reserved codes for Standard Model parton distribution functions
  if ( abs_pdg >= 901 && abs_pdg <= 930 ) return false;

  // Reggeon and pomeron
  if ( abs_pdg == 110 || abs_pdg == 990 ) return false;

  // Reserved codes for GEANT tracking purposes
  if ( abs_pdg == 998 || abs_pdg == 999 ) return false;

  // Reserved code for generator-specific pseudoparticles
  if ( abs_pdg == 100 ) return false;

  // If we've passed all of the tests above, then the particle is a meson
  return true;
}

bool MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::isSignal( FitEvent* event ) {
  // PDG codes of interest
  const int MUON = 13;
  const int MUON_NEUTRINO = 14;
  const int PROTON = 2212;

  // Require the event to be a numu CC inclusive interaction
  if ( !SignalDef::isCCINC(event, MUON_NEUTRINO, EnuMin, EnuMax) ) return false;

  // Require at least one proton in the final state
  if ( event->NumFSProton() < 1 ) return false;

  // Impose kinematic limits in the signal definition
  double p_mu = event->GetHMFSParticle( MUON )->fP.Vect().Mag(); // MeV
  double p_lead_p = event->GetHMFSParticle( PROTON )->fP.Vect().Mag(); // MeV

  // The muon momentum must be at least 100 MeV/c and at most 1.2 GeV/c
  if ( p_mu < 100. || p_mu > 1200. ) return false;

  // The leading proton momentum must lie on the interval [250, 1000] MeV/c
  if ( p_lead_p < 250. || p_lead_p > 1000. ) return false;

  // Veto events with final-state mesons or antimesons of any kind. Do this by
  // looping over every particle in the event.
  size_t num_particles = event->NParticles();
  for ( size_t p = 0u; p < num_particles; ++p ) {
    // Skip the check for any particle that is not part of the final state
    int state = event->GetParticleState( p );
    if ( state != kFinalState ) continue;
    // Otherwise, decide whether it is a meson or antimeson. If it is, then
    // veto the event.
    int pdg = event->GetParticlePDG( p );
    bool is_meson = isMesonOrAntimeson( pdg );
    if ( is_meson ) return false;
  }

  // If we've made it here, then the current event has passed all of the
  // requirements in the signal definition
  return true;
}

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::FillEventVariables( FitEvent* event ) {

  // Clear out the vector of passing bins, which may have already been filled
  // for the previous event
  fPassingBins.clear();

  if ( event->NumFSParticle(MU_MINUS) == 0 ) return;
  if ( event->NumFSParticle(PROTON) == 0 ) return;

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

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::LoadBinDefinitions() {
  std::string binning_file_name( FitPar::GetDataBase()
    + "/MicroBooNE/CC1MuNp/2025/bin_defs.txt" );

  std::ifstream bin_file( binning_file_name );
  std::string dummy_str;
  int bin_type, dummy_int;
  size_t num_true_bins;

  // TODO: add I/O error handling here
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

      // Interpret the variable for this cut and set up a function
      // object that will calculate it from the input FitEvent
      std::function< double(FitEvent*) > getter;

      if ( var_name == "mc_p3_lead_p.Mag()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( PROTON )->fP.Vect().Mag() / 1e3; // GeV
        };
      }
      else if ( var_name == "mc_p3_lead_p.CosTheta()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( PROTON )->fP.Vect().CosTheta();
        };
      }
      else if ( var_name == "mc_p3_mu.Mag()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().Mag() / 1e3; // GeV
        };
      }
      else if ( var_name == "mc_p3_mu.CosTheta()" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return ev->GetHMFSParticle( MU_MINUS )->fP.Vect().CosTheta();
        };
      }
      else if ( var_name == "mc_delta_pT" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return compute_stvs( ev, "delta_pT" );
        };
      }
      else if ( var_name == "mc_delta_pTx" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return compute_stvs( ev, "delta_pTx" );
        };
      }
      else if ( var_name == "mc_delta_pTy" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return compute_stvs( ev, "delta_pTy" );
        };
      }
      else if ( var_name == "mc_delta_alphaT" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return compute_stvs( ev, "delta_alphaT" ) * 180. / M_PI;
        };
      }
      else if ( var_name == "mc_pn" ) {
        getter = [=]( FitEvent* ev ) -> double {
          return compute_stvs( ev, "pn" );
        };
      }
      else if ( var_name == "mc_theta_mu_p" ) {
        getter = [=]( FitEvent* ev ) -> double {
          const TVector3& p3mu = ev->GetHMFSParticle( MU_MINUS )->fP.Vect();
          const TVector3& p3p = ev->GetHMFSParticle( PROTON )->fP.Vect();
          double theta_mup = std::acos( p3mu.Dot(p3p) / p3mu.Mag()
            / p3p.Mag() ) * 180. / M_PI;
          return theta_mup;
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

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::FillHistograms() {

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

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::ConvertEventRates() {
  // Do the standard conversion
  Measurement1D::ConvertEventRates();

  // Clone the binning from the MC histogram
  fMCHistWithAC.reset(
    dynamic_cast< TH1D* >( fMCHist->Clone() )
  );
  fMCHistWithAC->SetNameTitle( (fSettings.GetName() + "_MC_with_AC").c_str(),
   fSettings.GetFullTitles().c_str() );
  fMCHistWithAC->Reset();
  fMCHistWithAC->SetDirectory( nullptr );

  // Build a column vector using the predicted cross sections
  int num_bins = fMCHist->GetNbinsX();
  TMatrixD pred( num_bins, 1 );
  for ( int b = 0; b < num_bins; ++b ) {
    pred( b, 0 ) = fMCHist->GetBinContent( b + 1 );
  }

  // Apply the additional smearing matrix A_C to create a new prediction
  TMatrixD new_pred( *fAddSmear, TMatrixD::kMult, pred );

  // Store it in our clone of the original MC prediction histogram
  for ( int b = 0; b < num_bins; ++b ) {
    double xsec = new_pred( b, 0 );
    fMCHistWithAC->SetBinContent( b + 1, xsec );
  }

  // Prepare the slice histograms now that everything else is ready
  this->PrepareSlices();
}

double MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::GetLikelihood() {

  if ( fNoData || !fDataHist ) return 0.;

  // Apply Masking to MC if Required.
  if ( fIsMask and fMaskHist ) {
    NUIS_ERR(FTL, "Bin masks not yet supported by"
      " the MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu sample" );
    //PlotUtils::MaskBins(fMCHist, fMaskHist);
  }

  // Likelihood Calculation
  double stat = 0.;
  if ( fIsChi2 ) {
    if ( fIsNS ) {
      NUIS_ERR(FTL, "Norm-shape covariance not yet supported by"
        " the MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu sample" );
    }
    stat = StatUtils::GetChi2FromCov( fDataHist, fMCHistWithAC.get(),
      covar, NULL, 1.0, 1.0, fIsWriting ? fResidualHist : NULL );
  }

  fLikelihood = stat;

  return stat;
}

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::Write( std::string drawOpt ) {
  // Write the standard information
  Measurement1D::Write( drawOpt );

  // To start extra chi-squared calculations, create a mask that excludes
  // everything. Note that bins are kept if the mask entry is zero, and
  // discarded otherwise.
  int num_bins = fDataHist->GetNbinsX();
  TH1I mask_all( "mask_all", "mask all", num_bins, 0., num_bins );
  mask_all.SetDirectory( nullptr );
  for ( int b = 1; b <= num_bins; ++b ) mask_all.SetBinContent( b, 1 );

  // Compute chi-squared scores for each block of bins separately using
  // masks. Store the results in TNamed objects in the output
  // file.
  std::shared_ptr< TH1I > block_mask;
  for ( const auto& [ block_idx, bin_vec ] : fBlockHandler->fBlockBins ) {
    // Clone the exclude-everything mask and zero out bins that belong
    // to the current block (thus including them)
    block_mask.reset(
      dynamic_cast< TH1I* >( mask_all.Clone("block_mask") )
    );

    // Global bin indices are zero-based, while the ROOT histogram bins
    // are one-based, so we correct for this here
    for ( int b : bin_vec ) block_mask->SetBinContent( b + 1, 0 );

    // Compute the chi-squared statistic for the current block using the mask
    double chi2 = StatUtils::GetChi2FromCov( fDataHist, fMCHistWithAC.get(),
      covar, block_mask.get(), 1.0, 1.0, nullptr );

    // Create a TNamed object to store the result. We use a TNamed so that we
    // can store the chi^2 value and number of bins together for convenient
    // viewing
    std::string param_name = fSettings.GetName() + "_Block"
      + std::to_string( block_idx ) + "_Chi2";
    std::string chi2_per_bin_str = std::to_string( chi2 )
      + " / " + std::to_string( bin_vec.size() );
    TNamed chi2_param( param_name.c_str(), chi2_per_bin_str.c_str() );

    chi2_param.Write();
  }

  // Now we follow a similar procedure to compute and store chi-squared values
  // for each individual slice histogram in multi-slice blocks
  std::shared_ptr< TH1I > slice_mask;
  int num_slices = fBlockHandler->fHists.size();
  for ( int s = 0u; s < num_slices; ++s ) {

    // Before doing anything else, write the histograms themselves for the
    // current slice to the output file
    fBlockHandler->fHists.at( s )->Write(); // data slice histogram
    fMCHist_Slices.at( s )->Write(); // total MC slice histogram
    for ( int m : fMCHist_Modes->fmodes ) {
      // MC slice histogram for mode m
      fMCModeHists_Slices.at( m ).at( s )->Write();
    }

    // Find the block and slice number within the block for the current slice
    // histogram
    int block_idx = -1; // dummy value
    int slice_idx = -1; // dummy value
    for ( const auto& [ bl, hist_idx_vec ] : fBlockHandler->fBlockHists ) {
      for ( size_t h = 0u; h < hist_idx_vec.size(); ++h ) {
        int h_idx = hist_idx_vec.at( h );
        if ( h_idx == s ) {
          block_idx = bl;
          slice_idx = h;
        }
      }
    }

    // NOTE: This continue statement has been removed since some 1D blocks
    // include underflow or overflow bins that don't show up in their
    // slice histogram. In some cases, the user may want to see both
    // values, so it is easiest just to output all the information.
    //
    // OLD: If this is a 1D block, then there is only a single slice, and we
    // have already computed a suitable chi^2 score in the block-by-block
    // results above. We can therefore skip to the next slice.
    //if ( fBlockHandler->fBlockHists.size() <= 1u ) continue;

    // Clone the exclude-everything mask and zero out bins that belong
    // to the current slice (thus including them)
    slice_mask.reset(
      dynamic_cast< TH1I* >( mask_all.Clone("slice_mask") )
    );

    int num_unmasked_bins = 0;
    for ( const auto& [ mk, mv ] : fBlockHandler->fBinMap ) {
      // Unmask only bins that belong to the current slice histogram
      int hist_index = mv.histIdx;
      if ( hist_index != s ) continue;
      // Do the unmasking, taking into account the one-based indexing
      // of the ROOT histograms and the zero-based bin indices from the table
      // of block definitions
      int global_bin = mk;
      slice_mask->SetBinContent( global_bin + 1, 0 );
      ++num_unmasked_bins;
    }

    // Compute the chi-squared statistic for the current block using the mask
    double chi2 = StatUtils::GetChi2FromCov( fDataHist, fMCHistWithAC.get(),
      covar, slice_mask.get(), 1.0, 1.0, nullptr );

    // Create a TNamed object to store the result. We use a TNamed so that we
    // can store the chi^2 value and number of bins together for convenient
    // viewing
    std::string param_name = fSettings.GetName() + "_Block"
      + std::to_string( block_idx ) + "_Slice"
      + std::to_string( slice_idx ) + "_Chi2";
    std::string chi2_per_bin_str = std::to_string( chi2 )
      + " / " + std::to_string( num_unmasked_bins );
    TNamed chi2_param( param_name.c_str(), chi2_per_bin_str.c_str() );

    chi2_param.Write();
  }
}

void MicroBooNE_BNB_NumuCC0PiNp_2025_XSec_nu::PrepareSlices() {
  // Load the definitions of the blocks of bins
  std::string block_file_name( FitPar::GetDataBase()
    + "/MicroBooNE/CC1MuNp/2025/bin_blocks.txt" );

  fBlockHandler = std::make_shared< MicroBooNEBlockHandler >(
    fSettings.GetName(), block_file_name );

  // Clone the data histograms owned by the block handler to create
  // corresponding MC histograms
  for ( auto& hist : fBlockHandler->fHists ) {
    // Attach the MC suffix to the clone's name, then attach the data suffix
    // to the original
    auto mc_hist_name = hist->GetName() + std::string( "_MC" );
    hist->SetName( (hist->GetName() + std::string( "_data" )).c_str() );
    auto* temp_mc_hist = dynamic_cast< TH1D* >(
      hist->Clone( mc_hist_name.c_str() )
    );
    temp_mc_hist->SetDirectory( nullptr );
    temp_mc_hist->Reset();

    temp_mc_hist->SetLineColor( kRed );
    temp_mc_hist->SetLineStyle( 1 );
    temp_mc_hist->SetLineWidth( 1 );

    temp_mc_hist->SetFillColor( 0 );
    temp_mc_hist->SetFillStyle( 1001 );

    fMCHist_Slices.emplace_back( temp_mc_hist );

    // Make MC slice histograms for individual scattering modes
    for ( int m : fMCHist_Modes->fmodes ) {
      // Convert the interaction mode integer into a string label
      auto mode = static_cast< InputHandler::InteractionModes >( m );
      std::ostringstream oss;
      oss << '_' << mode;
      auto mode_hist_name = temp_mc_hist->GetName() + oss.str();
      // Set up the slice histogram for this mode
      auto* temp_mode_hist = dynamic_cast< TH1D* >(
        temp_mc_hist->Clone( mode_hist_name.c_str() )
      );
      temp_mode_hist->SetDirectory( nullptr );
      temp_mode_hist->Reset();
      fMCModeHists_Slices[ m ].emplace_back( temp_mode_hist );
    }

  }

  // Now that all slice histograms have been created, populate them with
  // corresponding entries from the full measurement histogram (that is
  // organized in terms of global bin number).
  // NOTE: Bins with one or more infinite edges are automatically skipped
  // by the block handler, so we don't have to worry about bins that have
  // no match in the slices
  for ( const auto& [mk, mv] : fBlockHandler->fBinMap ) {
    // The ROOT histogram used to store the global bin contents has one-based
    // indices, but the input table used to define the blocks is zero-based
    int global_bin = mk + 1;

    // Zero-based index of the slice histogram to which this global bin belongs
    int hist_idx = mv.histIdx; // position in the vector of slices

    // One-based bin index to be populated in the target histogram
    int local_bin = mv.rootBin;

    // Product of finite bin widths for this bin (used to convert to a
    // differential cross section)
    double widths = mv.width;

    // Retrieve the values of interest from the global histograms, dividing
    // by the bin width(s) to obtain a differential xsec each time
    double data_val = fDataHist->GetBinContent( global_bin ) / widths;
    double data_err = fDataHist->GetBinError( global_bin ) / widths;
    double mc_val = fMCHist->GetBinContent( global_bin ) / widths;
    std::map< int, double > mode_vals;
    for ( int m : fMCHist_Modes->fmodes ) {
      int mode_idx = fMCHist_Modes->ConvertModeToIndex( m );
      auto* mode_hist = fMCHist_Modes->GetHist( mode_idx );
      double value = mode_hist->GetBinContent( global_bin );
      mode_vals[ m ] = value / widths;
    }

    // Set the bin contents in the target slice histograms
    auto& data_hist = fBlockHandler->fHists.at( hist_idx );
    data_hist->SetBinContent( local_bin, data_val );
    data_hist->SetBinError( local_bin, data_err );

    auto& mc_hist = fMCHist_Slices.at( hist_idx );
    mc_hist->SetBinContent( local_bin, mc_val );

    for ( const auto& [ mode, val ] : mode_vals ) {
      auto& mode_hist = fMCModeHists_Slices.at( mode ).at( hist_idx );
      mode_hist->SetBinContent( local_bin, val );
    }

  }
}
