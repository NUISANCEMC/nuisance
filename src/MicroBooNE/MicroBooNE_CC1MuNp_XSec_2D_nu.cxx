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

#include "MicroBooNE_CC1MuNp_XSec_2D_nu.h"
#include "MicroBooNE_SignalDef.h"
#include "TH2D.h"

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

}

MicroBooNE_CC1MuNp_XSec_2D_nu
  ::MicroBooNE_CC1MuNp_XSec_2D_nu( nuiskey samplekey )
{
  fSettings = LoadSampleSettings( samplekey );
  std::string name = fSettings.GetS( "name" );

  // The main histograms use the bin number on the x-axis
  fSettings.SetXTitle( "bin number" );

  if ( !name.compare("MicroBooNE_CC1MuNp_XSec_2D_PpCosp_nu") ) {
    fSettings.SetYTitle( "#sigma (cm^{2}/^{40}Ar)" );
  }
  else {
    assert( false );
  }

  // Sample overview ---------------------------------------------------
  std::string descrip = name + " sample.\n" \
                        "Target: Ar\n" \
                        "Flux: BNB FHC numu\n" \
                        "Signal: CC1MuNp\n";

  fSettings.SetDescription( descrip );
  fSettings.SetTitle( name );
  fSettings.SetAllowedTypes( "FULL,DIAG/FREE,SHAPE,FIX/SYSTCOV/STATCOV",
    "FIX/FULL" );
  fSettings.SetEnuRange( 0.0, 6.8 );
  fSettings.DefineAllowedTargets( "Ar" );
  fSettings.DefineAllowedSpecies( "numu" );
  FinaliseSampleSettings();

  // Scale factor for the flux-averaged total cross section (cm^2 / ^{40}Ar) in
  // each bin
  fScaleFactor = GetEventHistogram()->Integral( "width" )
    * 1e-38 * MASS_NUMBER_40AR / fNEvents / TotalIntegratedFlux();

  // Get bin definitions
  this->LoadBinDefinitions();

  // TODO: replace with real data!
  int num_bins = fBinDefinitions.size();
  fDataHist = new TH1D( "dummy", "dummy", num_bins, 0., num_bins );
  for ( int b = 1; b <= num_bins; ++b ) {
    fDataHist->SetBinContent( b, 1e-38 );
    fDataHist->SetBinError( b, 3e-39 );
  }

  //fDataHist = PlotUtils::GetTH1DFromRootFile(datafile, histname);
  fDataHist->SetNameTitle( (fSettings.GetName() + "_data").c_str(),
    fSettings.GetFullTitles().c_str() );

  this->SetCovarFromDiagonal();
  //SetCovarFromRootFile(inputFile, "CovarianceMatrix_" + objSuffix);

  // Set up the "MC fine" histogram ahead of calling FinaliseMeasurement().
  // This prevents auto-creation of this histogram. Since we're using a
  // 1D histogram with bin number along the x-axis, it doesn't make sense
  // to subdivide the bins. Rather than doing that automatically, I just
  // copy the original data histogram binning here. Thus, MCFine ends up
  // using the same bins as regular MC.
  fMCFine = dynamic_cast< TH1D* >( fDataHist->Clone("mcfine") );
  fMCFine->SetNameTitle( (fSettings.GetName() + "_MC_FINE").c_str(),
    fSettings.GetFullTitles().c_str() );
  fMCFine->Reset();

  // Having created MCFine in advance, we can now proceed to the usual
  // steps in finalizing the measurement
  this->FinaliseMeasurement();
}


bool MicroBooNE_CC1MuNp_XSec_2D_nu::isSignal( FitEvent* event ) {
  return SignalDef::MicroBooNE::isCC1MuNpFor2DAnalysis( event, EnuMin, EnuMax );
}

void MicroBooNE_CC1MuNp_XSec_2D_nu::FillEventVariables( FitEvent* event ) {

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

void MicroBooNE_CC1MuNp_XSec_2D_nu::LoadBinDefinitions() {
  std::string binning_file_name( FitPar::GetDataBase()
    + "/MicroBooNE/myconfig_uB2D.txt" );

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

void MicroBooNE_CC1MuNp_XSec_2D_nu::FillHistograms() {

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
