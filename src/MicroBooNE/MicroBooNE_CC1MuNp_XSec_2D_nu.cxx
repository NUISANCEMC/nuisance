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

  if ( !name.compare("MicroBooNE_CC1MuNp_XSec_2D_nu") ) {
    //fDist = kPmu;
    //objSuffix = "mumom";
    //fSettings.SetXTitle("cos#theta_{p}-p_{p} (GeV)");
    fSettings.SetXTitle( "bin number" );
    fSettings.SetYTitle( "d^{2}#sigma/dp_{p}dcos#theta_{p}"
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
  fSettings.DefineAllowedTargets("Ar");
  fSettings.DefineAllowedSpecies("numu");
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
  return SignalDef::MicroBooNE::isCC1MuNp( event, EnuMin, EnuMax );
  //bool is_sig = SignalDef::MicroBooNE::isCC1MuNp( event, EnuMin, EnuMax );
  //if ( !is_sig ) return false;

  //const int PROTON = 2212;

  //if ( event->NumFSParticle(PROTON) == 0 ) return false;
  //double pp = event->GetHMFSParticle(PROTON)->fP.Vect().Mag() / 1000;
  //double cosp = event->GetHMFSParticle(PROTON)->fP.Vect().CosTheta();
  //const auto iter = fBinToDefinitionMap.cbegin();
  //const auto& def = iter->second;
  //return def.InBin( pp, cosp );
};


void MicroBooNE_CC1MuNp_XSec_2D_nu::FillEventVariables( FitEvent* event ) {

  const int PROTON = 2212;

  if ( event->NumFSParticle(PROTON) == 0 ) return;
  double pp = event->GetHMFSParticle(PROTON)->fP.Vect().Mag() / 1000;
  double cosp = event->GetHMFSParticle(PROTON)->fP.Vect().CosTheta();

  // TODO: Consider revising. Makes a critical assumption that exactly
  // one bin will ever be filled per event.
  for ( const auto& bin_pair : fBinToDefinitionMap ) {
    int bin_idx = bin_pair.first;
    const BinDef& def = bin_pair.second;
    if ( def.InBin(pp, cosp) ) {
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
  const std::string binning_file_name = FitPar::GetDataBase()
    + "/MicroBooNE/mybins2Dproton.txt";
  std::ifstream bin_file( binning_file_name );
  int bin_idx = 0;
  double xmin, xmax, ymin, ymax;
  // TODO: add I/O error handling here
  while ( bin_file >> xmin >> xmax >> ymin >> ymax ) {
    fBinToDefinitionMap[ bin_idx ] = BinDef( xmin, xmax, ymin, ymax );
    ++bin_idx;
  }
}
