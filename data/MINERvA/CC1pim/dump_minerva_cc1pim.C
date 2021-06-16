#include <fstream>
#include <string>

#include "TH1D.h"
#include "TH2D.h"

const int NUM_XSEC_HEADER_LINES = 2;
const int NUM_CORR_HEADER_LINES = 1;

void make_histograms( const std::string& variable_name,
  const std::string& xaxis_title, const std::string& yaxis_title,
  const std::string& output_tfile, bool update_tfile = true,
  bool draw = false )
{
  std::cout << "\nProcessing data for " << variable_name << '\n';

  std::ifstream xsec_file( ("xsec_" + variable_name + ".csv").c_str() );
  std::ifstream corr_file( ("correlation_" + variable_name + ".csv").c_str() );

  // First, skip the header lines in the differential cross section CSV file
  std::string line;
  int lines_skipped = 0;
  while ( lines_skipped < NUM_XSEC_HEADER_LINES ) {
    std::getline( xsec_file, line );
    ++lines_skipped;
  }

  // Now read in the data for each 1D bin
  std::vector< double > bin_edges;
  std::vector< double > bin_xsecs;
  std::vector< double > bin_errors;
  double high_edge = 0.;
  while ( std::getline(xsec_file, line) && xsec_file ) {
    std::vector< double > entry_vec;
    std::string entry;
    std::istringstream iss( line );
    while ( std::getline(iss, entry, ',') && iss ) {
      double value;
      std::istringstream iss2( entry );
      iss2 >> value;
      if ( !iss2 ) continue;
      entry_vec.push_back( value );
    }

    if ( entry_vec.empty() ) continue;

    double low_edge = entry_vec.front();
    high_edge = entry_vec.at( 1 );

    // Convert tabulated value (10^{-40} cm^2 / nucleon / x-axis units)
    // to cm^2 / nucleon / x-axis units
    double raw_xsec = entry_vec.at( 2 );
    double xsec = raw_xsec * 1e-40;

    // Fractional errors are given as percentages
    double pct_error = entry_vec.at( 5 );
    double tot_error = xsec * pct_error / 100.;

    bin_edges.push_back( low_edge );
    bin_xsecs.push_back( xsec );
    bin_errors.push_back( tot_error );

    std::cout << "low = " << low_edge << ", high = " << high_edge
      << ", xsec = " << raw_xsec << ", total error (%) = "
      << pct_error << '\n';
  }

  // Also store the high edge of the last bin (the value persists after
  // completion of the loop above)
  bin_edges.push_back( high_edge );

  int num_bins = bin_xsecs.size();
  std::string data_hist_name = variable_name + "_data";
  std::string data_hist_title = variable_name + " data; " + xaxis_title
    + "; " + yaxis_title;

  TH1D* data_hist = new TH1D( data_hist_name.c_str(), data_hist_title.c_str(),
    num_bins, bin_edges.data() );

  // The Enu distribution tabulated in the supplement is actually dsigma/dEnu
  // instead of sigma(Ev) as in the body of the paper. The units given in the
  // supplement are 10^{-40} cm^2 / 0.5 GeV / nucleon, so we need to correct
  // for the bin width here in order to do the comparison in terms of
  // sigma(Ev).
  if ( variable_name == "enu" ) {
    for ( int b = 1; b <= num_bins; ++b ) {
      // ROOT histograms use one-based binning (to allow for underflow), while
      // std::vector indexing is zero-based
      size_t bin_index = b - 1;

      double bin_width = data_hist->GetBinWidth( b );
      // Note that the bins have widths in units of GeV
      double correction_factor = bin_width / 0.5;

      double& xsec = bin_xsecs.at( bin_index );
      double& error = bin_errors.at( bin_index );

      xsec *= correction_factor;
      error *= correction_factor;
    }
  }

  for ( int b = 1; b <= num_bins; ++b ) {
    // ROOT histograms use one-based binning (to allow for underflow), while
    // std::vector indexing is zero-based
    size_t bin_index = b - 1;
    data_hist->SetBinContent( b, bin_xsecs.at(bin_index) );
    data_hist->SetBinError( b, bin_errors.at(bin_index) );
  }

  // Use the data histogram and the correlation matrix to calculate a total
  // covariance matrix

  // Skip the header lines in the correlation matrix CSV file
  lines_skipped = 0;
  while ( lines_skipped < NUM_CORR_HEADER_LINES ) {
    std::getline( corr_file, line );
    ++lines_skipped;
  }

  std::string cov_mat_name = variable_name + "_covariance";
  std::string cov_mat_title = variable_name + " covariance; " + xaxis_title
    + "; " + xaxis_title + "; total covariance";

  // Prepare a TH2D to store the covariance matrix
  TH2D* cov_mat = new TH2D( cov_mat_name.c_str(), cov_mat_title.c_str(),
    num_bins, bin_edges.data(), num_bins, bin_edges.data() );

  // Now read in the correlation coefficients
  int current_x_bin = 1;
  while ( std::getline(corr_file, line) && corr_file ) {
    std::list< double > entry_list;
    std::string entry;
    std::istringstream iss( line );
    while ( std::getline(iss, entry, ',') && iss ) {
      double value;
      std::istringstream iss2( entry );
      iss2 >> value;
      if ( !iss2 ) continue;
      entry_list.push_back( value );
    }

    if ( entry_list.empty() ) continue;

    // The first two entries on each line are bin boundaries. We don't
    // need them, so just remove them from the list before continuing
    entry_list.pop_front();
    entry_list.pop_front();

    double x_err = data_hist->GetBinError( current_x_bin );
    int current_y_bin = 1;
    std::cout << "Correlation matrix:\n";
    for ( const double& correlation : entry_list ) {
      double y_err = data_hist->GetBinError( current_y_bin );
      double covariance = correlation * x_err * y_err;
      covariance /= std::pow( 1e-38, 2 );
      cov_mat->SetBinContent( current_x_bin, current_y_bin, covariance );

      ++current_y_bin;
      std::cout << ' ' << correlation;
    }

    ++current_x_bin;
    std::cout << '\n';
  }

  data_hist->SetStats( false );
  cov_mat->SetStats( false );

  if ( draw ) {
    TCanvas* c1 = new TCanvas;
    data_hist->Draw();

    TCanvas* c2 = new TCanvas;
    cov_mat->Draw( "colz" );
  }

  std::string tfile_option = "update";
  if ( !update_tfile ) tfile_option = "recreate";
  TFile out_tfile( output_tfile.c_str(), tfile_option.c_str() );

  data_hist->Write();
  cov_mat->Write();
}

void dump_minerva_cc1pim() {

  make_histograms( "kinetic", "T_{#pi} (GeV)", "d#sigma/dT_{#pi} ("
    "cm^{2}/GeV/nucleon)", "minerva_cc1pim_data.root", false );

  make_histograms( "mmom", "p_{#mu} (GeV)", "d#sigma/dp_{#mu} ("
    "cm^{2}/GeV/nucleon)", "minerva_cc1pim_data.root" );

  make_histograms( "mutheta", "#theta_{#mu} (degree)",
    "d#sigma/d#theta_{#mu} (cm^{2}/degree/nucleon)",
    "minerva_cc1pim_data.root" );

  make_histograms( "theta", "#theta_{#pi^{-}} (degree)",
    "d#sigma/d#theta_{#pi^{-}} (cm^{2}/degree/nucleon)",
    "minerva_cc1pim_data.root" );

  make_histograms( "q2", "Q^{2} (GeV^{2})", "d#sigma/dQ^{2} ("
    "cm^{2}/GeV^{2}/nucleon)", "minerva_cc1pim_data.root" );

  make_histograms( "enu", "E_{#bar{#nu}} (GeV)", "#sigma(E_{#bar{#nu}}) ("
    "cm^{2}/nucleon)", "minerva_cc1pim_data.root" );

}
