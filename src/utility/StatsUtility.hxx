#ifndef UTILITY_STATSUTILITY_HXX_SEEN
#define UTILITY_STATSUTILITY_HXX_SEEN

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unimplemented_covariance_usage);
NEW_NUIS_EXCEPT(Mismatched_NBins);

template <typename HT>
typename std::enable_if<TH_traits<HT>::NDims == 1, double>::type
GetChi2(HT const *a, HT const *b, TH2D const *Covariance = nullptr) {
  if (Covariance) {
    throw unimplemented_covariance_usage()
        << "[ERROR]: Using a covariance in the Chi2 evaluation is not yet "
           "implemented.";
  }

  if (a->GetXaxis()->GetNbins() != b->GetXaxis()->GetNbins()) {
    Mismatched_NBins() << "[ERROR]: Attempted to evaluate Chi2 between two "
                          "histograms with differing bin contents: NBins(a) = "
                       << a->GetXaxis()->GetNbins()
                       << ", NBins(b) = " << b->GetXaxis()->GetNbins();
  }

  double chi2 = 0;
  for (Int_t bin_it_i = 0; bin_it_i < a->GetXaxis()->GetNbins(); ++bin_it_i) {
    for (Int_t bin_it_j = 0; bin_it_j < a->GetXaxis()->GetNbins(); ++bin_it_j) {
      double err =
          1.0 / (b->GetBinError(bin_it_i + 1) * b->GetBinError(bin_it_j + 1));

      double contrib =
          (a->GetBinContent(bin_it_i + 1) - b->GetBinContent(bin_it_i + 1)) *
          (err) *
          (a->GetBinContent(bin_it_j + 1) - b->GetBinContent(bin_it_j + 1));

      chi2 += contrib;
    }
  }
  return chi2;
}
} // namespace utility
} // namespace nuis

#endif
