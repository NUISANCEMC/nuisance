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

//********************************************************************
MicroBooNE_CC1MuNp_XSec_2D_nu::MicroBooNE_CC1MuNp_XSec_2D_nu(nuiskey samplekey) {
//********************************************************************
  fSettings = LoadSampleSettings(samplekey);
  std::string name = fSettings.GetS("name");

  // The main histograms use the bin number on the x-axis
  fSettings.SetXTitle( "bin number" );

  if ( !name.compare("MicroBooNE_CC1MuNp_XSec_2D_PpCosp_nu") ) {
    fDist = kPpCosp;
    fSettings.SetYTitle( "d^{2}#sigma/dp_{p}dcos#theta_{p}"
      " (cm^{2}/GeV/^{40}Ar)" );
  }
  else if ( !name.compare("MicroBooNE_CC1MuNp_XSec_2D_PmuCosmu_nu") ) {
    fDist = kPmuCosmu;
    fSettings.SetYTitle( "d^{2}#sigma/dp_{#mu}dcos#theta_{#mu}"
      " (cm^{2}/GeV/^{40}Ar)" );
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

  // ScaleFactor for the flux-averaged total cross section (cm^2 / ^{40}Ar).
  // The bins are allowed to have non-uniform widths, so we account for
  // that separately bin-by-bin in ConvertEventRates()
  fScaleFactor = GetEventHistogram()->Integral( "width" )
    * 1e-38 * MASS_NUMBER_40AR / fNEvents / TotalIntegratedFlux();

  // Get bin definitions
  this->LoadBinDefinitions();

  // TODO: replace with real data!
  int num_bins = fBinToDefinitionMap.size();
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
};


void MicroBooNE_CC1MuNp_XSec_2D_nu::FillEventVariables( FitEvent* event ) {

  const int PROTON = 2212;
  const int MU_MINUS = 13;

  int pdg;
  double p, cos;
  if ( fDist == kPpCosp ) pdg = PROTON;
  else if ( fDist == kPmuCosmu ) pdg = MU_MINUS;
  else assert( false );

  if ( event->NumFSParticle(pdg) == 0 ) return;
  p = event->GetHMFSParticle(pdg)->fP.Vect().Mag() / 1000;
  cos = event->GetHMFSParticle(pdg)->fP.Vect().CosTheta();

  // TODO: Consider revising. Makes a critical assumption that exactly
  // one bin will ever be filled per event.
  for ( const auto& bin_pair : fBinToDefinitionMap ) {
    int bin_idx = bin_pair.first;
    const BinDef& def = bin_pair.second;
    if ( def.InBin(p, cos) ) {
      fXVar = bin_idx;
      return;
    }
  }
  fXVar = -1.;
  //assert( false );
}


void MicroBooNE_CC1MuNp_XSec_2D_nu::ConvertEventRates() {
  // Do the standard conversion first
  Measurement1D::ConvertEventRates();

  // Divide by the physical bin widths to get a flux-averaged differential
  // cross section. For simplicity, do this by creating a temporary histogram
  // first that contains entries of ( 1 / width ) in each bin.
  // TODO: consider using a std::unique_ptr here instead for auto-deletion
  TH1D* width_hist = dynamic_cast< TH1D* >( fDataHist->Clone("width_hist") );
  width_hist->Reset();
  width_hist->SetDirectory( nullptr );
  for ( const auto& bin_pair : fBinToDefinitionMap ) {
    // ROOT TH1D objects always include an underflow bin with index zero.
    // This introduces an offset that we correct for here by adding one
    // to the original zero-based bin index.
    int root_bin_idx = bin_pair.first + 1;
    const BinDef& def = bin_pair.second;
    double width = ( def.fXMax - def.fXMin ) * ( def.fYMax - def.fYMin );
    width_hist->SetBinContent( root_bin_idx, 1.0 / width );
  }

  // Divide all relevant histograms by the physical bin width. Adapted
  // from similar code in Measurement1D::ScaleEvents()
  fMCHist->Multiply( width_hist );
  fMCFine->Multiply( width_hist );

  if (fMCHist_Modes) fMCHist_Modes->Multiply( width_hist );
  if (fMCFine_Modes) fMCFine_Modes->Multiply( width_hist );

  delete width_hist;
}

void MicroBooNE_CC1MuNp_XSec_2D_nu::LoadBinDefinitions() {
  std::string binning_file_name = FitPar::GetDataBase();
  if ( fDist == kPpCosp ) {
    binning_file_name += "/MicroBooNE/mybins2Dproton.txt";
  }
  else if ( fDist == kPmuCosmu ) {
    binning_file_name += "/MicroBooNE/mybins2Dmuon.txt";
  }
  std::ifstream bin_file( binning_file_name );
  int bin_idx = 0;
  double xmin, xmax, ymin, ymax;
  // TODO: add I/O error handling here
  while ( bin_file >> xmin >> xmax >> ymin >> ymax ) {
    fBinToDefinitionMap[ bin_idx ] = BinDef( xmin, xmax, ymin, ymax );
    ++bin_idx;
  }
}


void MicroBooNE_CC1MuNp_XSec_2D_nu::MakeSlices() {
  // Populate the "slice edge map" with the lower bounds of each
  // momentum bin and both edges of each angular bin
  for ( const auto& bin_pair : fBinToDefinitionMap ) {
    const BinDef& def = bin_pair.second;
    // This command auto-creates an empty std::set if needed
    auto& y_edge_set = fSliceEdgeMap[ def.fXMin ];
    y_edge_set.insert( def.fYMin );
    y_edge_set.insert( def.fYMax );
  }

  // We're ready now. Make the slices.
  int slice_count = 0;
  double old_xmin = DBL_MAX;
  TH1D* current_slice_hist = nullptr;
  for ( const auto& bin_pair : fBinToDefinitionMap ) {
    // We add one here since ROOT TH1 bins have one-based indices
    int root_bin_idx = bin_pair.first + 1;
    const BinDef& def = bin_pair.second;

    if ( def.fXMin != old_xmin ) {
      old_xmin = def.fXMin;

      // Build a temporary vector of bin edges that we can use to initialize
      // the histogram for the current slice. Note that std::set will
      // automatically sort the edges in ascending order. We need a vector,
      // though, so that we can access the underlying C-style array.
      std::vector< double > y_edge_vec;
      const std::set<double>& y_edge_set = fSliceEdgeMap.at( def.fXMin );
      for ( const auto& edge : y_edge_set ) y_edge_vec.push_back( edge );

      int num_y_bins = y_edge_vec.size() - 1;

      current_slice_hist = new TH1D( "slice", "slice",
        num_y_bins, y_edge_vec.data() );
      current_slice_hist->SetDirectory( nullptr );

      std::stringstream temp_ss;
      temp_ss << fSettings.GetS("name") << "_MC_Slice" << slice_count;

      current_slice_hist->SetName( temp_ss.str().c_str() );

      std::string particle_subscript;
      if ( fDist == kPpCosp ) particle_subscript = "p";
      else if ( fDist == kPmuCosmu ) particle_subscript = "#mu";
      else assert( false );

      temp_ss << ", p_{" << particle_subscript << "} [" << def.fXMin
        << "," << def.fXMax << "] GeV";
      current_slice_hist->SetTitle( temp_ss.str().c_str() );

      current_slice_hist->GetXaxis()->SetTitle(
        ("cos#theta_{" + particle_subscript + '}').c_str() );
      current_slice_hist->GetYaxis()->SetTitle( fSettings.GetYTitle().c_str() );

      fMCHist_Slices.push_back( current_slice_hist );
      ++slice_count;
    }

    double xsec = fMCHist->GetBinContent( root_bin_idx );
    double error = fMCHist->GetBinError( root_bin_idx );

    int slice_root_bin_idx = current_slice_hist->FindBin( def.fYMin );
    current_slice_hist->SetBinContent( slice_root_bin_idx, xsec );
    current_slice_hist->SetBinError( slice_root_bin_idx, error );
  }
}

void MicroBooNE_CC1MuNp_XSec_2D_nu::Write( std::string drawopt ) {
  this->Measurement1D::Write( drawopt );
  // Also create the slice MC histograms
  // TODO: revisit, add data slices and possibly put this somewhere else
  this->MakeSlices();
  for ( auto* mc_slice_hist : fMCHist_Slices ) mc_slice_hist->Write();
}
