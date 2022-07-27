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
  const int MASS_NUMBER_40AR = 40;
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

  const int PROTON = 2212;
  const int MU_MINUS = 13;

  if ( event->NumFSParticle(MU_MINUS) == 0 ) return;
  if ( event->NumFSParticle(PROTON) == 0 ) return;

  const TVector3& p3mu = event->GetHMFSParticle( MU_MINUS )->fP.Vect();
  const TVector3& p3p = event->GetHMFSParticle( PROTON )->fP.Vect();

  // Loop over each of the bin definitions. Keep track of the bins that
  // pass all cuts (and should thus be filled in this event)
  const std::string delimiter( "&&" );
  for ( size_t b = 0u; b < fBinDefinitions.size(); ++b ) {

    const auto& bin_def = fBinDefinitions.at( b );

    // Start out assuming that the current bin will pass
    bool bin_ok = true;

    // Skip the text before the first "&&" (here we assume that it is a simple
    // bool for the signal definition)
    std::string all_cuts = bin_def.substr(
      bin_def.find(delimiter) + delimiter.length() );

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

      // Interpret the variable for this cut and load the
      // appropriate numerical value for the event
      double var = 0.;
      if ( var_name == "mc_p3_lead_p.Mag()" ) {
        var = p3p.Mag() / 1e3; // GeV
      }
      else if ( var_name == "mc_p3_lead_p.CosTheta()" ) {
        var = p3p.CosTheta();
      }
      else if ( var_name == "mc_p3_mu.Mag()" ) {
        var = p3mu.Mag() / 1e3; // GeV
      }
      else if ( var_name == "mc_p3_mu.CosTheta()" ) {
        var = p3mu.CosTheta();
      }
      // Just skip the background true bin definitions
      else if ( var_name == "ategory" ) {
        bin_ok = false;
        break;
      }
      else NUIS_ABORT( "Unrecognized cut variable " + var_name );

      // Test the cut based on the appropriate comparison operator
      if ( comp_op == ">=" ) {
        if ( var < cut_val ) {
          bin_ok = false;
          break;
        }
      }
      else if ( comp_op == "<" ) {
        if ( var >= cut_val ) {
          bin_ok = false;
          break;
        }
      }
      else NUIS_ABORT( "Unrecognized comparison operator " + comp_op );

    } while ( pos != std::string::npos );

    if ( bin_ok ) fPassingBins.push_back( b );

  } // loop over bins

}

void MicroBooNE_CC1MuNp_XSec_2D_nu::LoadBinDefinitions() {
  std::string binning_file_name( FitPar::GetDataBase()
    + "/MicroBooNE/myconfig_uB2D.txt" );

  std::ifstream bin_file( binning_file_name );
  std::string dummy_str;
  int dummy_int;
  size_t num_true_bins;

  // TODO: add I/O error handling here
  // Skip the output TDirectoryFile name and the input TTree name
  bin_file >> dummy_str >> dummy_str >> num_true_bins;

  for ( size_t tb = 0u; tb < num_true_bins; ++tb ) {

    // Skip the true bin type and block index
    bin_file >> dummy_int >> dummy_int;

    // Use two calls to std::getline using a double quote delimiter
    // in order to get the contents of the next double-quoted string
    std::string temp_line;
    std::getline( bin_file, temp_line, '\"' );
    std::getline( bin_file, temp_line, '\"' );

    fBinDefinitions.push_back( temp_line );
  }

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
