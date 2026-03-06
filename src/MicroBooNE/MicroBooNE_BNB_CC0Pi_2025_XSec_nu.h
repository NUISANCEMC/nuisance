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
#ifndef MICROBOONE_BNB_MUCC0Pi_2025_XSEC_NU_H_SEEN
#define MICROBOONE_BNB_MUCC0Pi_2025_XSEC_NU_H_SEEN



#include <functional>
#include <memory>
#include <vector>

#include "Measurement1D.h"

#include <iostream>
#include <fstream>
#include "TH2D.h"

class TH2D;

struct MyCut {
  MyCut( std::function< double(FitEvent*) > getter,
    std::function< bool(double) > tester ) : getter_( getter ),
    tester_( tester ) {}

  std::function< double(FitEvent*) > getter_;
  std::function< bool(double) > tester_;

  inline bool evaluate( FitEvent* event ) const {
    return tester_( getter_(event) );
  }
};

class MicroBooNE_BNB_CC0Pi_2025_XSec_nu : public Measurement1D {

public:

  /// Basic Constructor.
  MicroBooNE_BNB_CC0Pi_2025_XSec_nu( nuiskey samplekey );

  /// Virtual Destructor
  inline ~MicroBooNE_BNB_CC0Pi_2025_XSec_nu() {
    fResidualHist = NULL;
    fChi2LessBinHist = NULL;
  };

  /// Apply signal definition
  virtual bool isSignal( FitEvent* nvect );

  /// Fill kinematic distributions
  void FillEventVariables( FitEvent* customEvent );

  virtual void FillHistograms() override;

  ///  Adjust the standard event rate conversion to include
  ///  application of the additional smearing matrix A_C
  void ConvertEventRates() override;

  /// Work around some hard-coded assumptions in Measurement1D::GetLikelihood()
  double GetLikelihood() override;
  

private:

  void LoadBinDefinitions(std::string binning_file_name) ;

  // Each bin is defined as a series of cuts that are applied to a FitEvent to
  // determine whether it belongs
  std::vector< std::vector<MyCut> > fBinDefinitions;

  // Temporary storage for the index of each bin that passed all cuts for any
  // particular event
  std::vector< size_t > fPassingBins;

  // Additional smearing matrix used to transform the input MC predictions
  std::unique_ptr< TMatrixD > fAddSmear;
  
  bool is2D; 

};

////////
/// Extra Function checking the input histogram used when validating
///////
inline bool CheckHist(const TH1D* h, std::ostream& out = std::cerr, bool verbose = true) {
  if (!h) { out << "[FAIL] Histogram pointer is null.\n"; return false; }

  if (!h->InheritsFrom(TH1D::Class())) {
    out << "[FAIL] Object '" << h->GetName() << "' is a " << h->ClassName()
        << ", not a TH1D.\n";
    return false;
  }

  const int nb = h->GetNbinsX();
  if (nb <= 0) { out << "[FAIL] " << h->GetName() << " has no bins.\n"; return false; }

  // Basic stats
  const double entries = h->GetEntries();
  const double integral = h->Integral(0, nb+1); // include under/overflow
  if (verbose) {
    out << "[INFO] " << h->GetName()
        << " | bins=" << nb
        << " | entries=" << entries
        << " | integral(uf/of)=" << integral
        << " | sumw2=" << (h->GetSumw2N() ? "yes" : "no")
        << "\n";
  }

  // Look for NaNs/Infs in bin content or error (inc. under/overflow)
  auto bad = [&](double x){ return !std::isfinite(x); };
  int n_bad_content = 0, n_bad_error = 0, n_neg_error = 0;
  for (int b = 0; b <= nb+1; ++b) {
    double c = h->GetBinContent(b);
    double e = h->GetBinError(b);
    if (bad(c)) ++n_bad_content;
    if (bad(e)) ++n_bad_error;
    if (e < 0)   ++n_neg_error;
  }
  if (n_bad_content || n_bad_error || n_neg_error) {
    out << "[FAIL] " << h->GetName() << " has issues: "
        << "bad_content=" << n_bad_content
        << ", bad_error=" << n_bad_error
        << ", negative_error=" << n_neg_error << "\n";
    return false;
  }

  // Optional: flag suspicious all-zero or empty-entries cases
  if (entries == 0 && integral == 0.0) {
    out << "[WARN] " << h->GetName()
        << " has zero entries and zero integral (is it empty?).\n";
  }

  // Optional: check axis metadata
  if (!h->GetXaxis()->GetTitle() || std::string(h->GetXaxis()->GetTitle()).empty()) {
    if (verbose) out << "[WARN] X-axis title is empty.\n";
  }
  if (!h->GetYaxis()->GetTitle() || std::string(h->GetYaxis()->GetTitle()).empty()) {
    if (verbose) out << "[WARN] Y-axis title is empty.\n";
  }

  if (verbose) out << "[OK] " << h->GetName() << " passed checks.\n";
  return true;
}


#endif
