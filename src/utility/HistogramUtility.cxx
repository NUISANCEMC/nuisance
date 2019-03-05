#include "utility/HistogramUtility.hxx"

#include <algorithm>
#include <cctype>

namespace nuis {
namespace utility {
bool IsFlowBin(TAxis const &ax, Int_t bin_it) {
  return ((bin_it <= 0) || (bin_it >= (ax.GetNbins() + 1)));
}

bool IsInHistogramRange(TAxis const &ax, double v) {
  Int_t bin_it = ax.FindFixBin(v);
  return !IsFlowBin(ax, bin_it);
}

NEW_NUIS_EXCEPT(invalid_TH2Poly);
NEW_NUIS_EXCEPT(invalid_PolyBinSpecifierList);

std::vector<std::unique_ptr<TH1>> GetTH2PolySlices(
    std::unique_ptr<TH2Poly> &hinp,
    std::vector<std::vector<PolyBinSpecifier>> const &BinsSpecifiers) {

  std::vector<std::unique_ptr<TH1>> slices;

  size_t sl_it = 0;
  for (auto &slice_spec : BinsSpecifiers) {
    std::vector<double> Binning;
    std::vector<double> BinContent;
    std::vector<double> BinError;
    bool UseXAxis = false;
    size_t bin_ctr = 0;
    for (auto poly_bin_spec : slice_spec) {
      Int_t bin_it = hinp->FindBin(poly_bin_spec.X, poly_bin_spec.Y);
      if (bin_it < 1) {
        std::cout << "[WARN]: When searching for matching bin: { X: "
                  << poly_bin_spec.X << ", Y: " << poly_bin_spec.Y
                  << "} got flow bin: " << bin_it << std::endl;
        continue;
      }
      TH2PolyBin *poly_bin =
          dynamic_cast<TH2PolyBin *>(hinp->GetBins()->At(bin_it - 1));

      if (!bin_ctr) {
        UseXAxis = poly_bin_spec.UseXAxis;
      } else if (UseXAxis != poly_bin_spec.UseXAxis) {
        throw invalid_PolyBinSpecifierList()
            << "[ERROR]: For slice: " << sl_it
            << " of TH2Poly: " << std::quoted(hinp->GetName())
            << " bin specifier: " << bin_ctr << " was set to use the "
            << (poly_bin_spec.UseXAxis ? "X" : "Y")
            << " axis as the dependent axis of the slice, but previous bins "
               "were set to use the "
            << (poly_bin_spec.UseXAxis ? "X" : "Y") << " axis.";
      }

      if ((poly_bin_spec.X < poly_bin->GetXMin()) ||
          (poly_bin_spec.X >= poly_bin->GetXMax()) ||
          (poly_bin_spec.Y < poly_bin->GetYMin()) ||
          (poly_bin_spec.Y >= poly_bin->GetYMax())) {
        std::cout
            << "[WARN]: Found bin doesn't seem to contain expected point: { X: "
            << poly_bin_spec.X << ", Y: " << poly_bin_spec.Y
            << "}, got bin_it = " << bin_it
            << " which had x_low: " << poly_bin->GetXMin()
            << ", and x_up: " << poly_bin->GetXMax()
            << ", y_low: " << poly_bin->GetYMin()
            << ", y_up: " << poly_bin->GetYMax() << std::endl;
      }

      double low = UseXAxis ? poly_bin->GetXMin() : poly_bin->GetYMin();
      double up = UseXAxis ? poly_bin->GetXMax() : poly_bin->GetYMax();

      if (!Binning.size()) { // Add low edge
        Binning.push_back(low);
      } else if (std::abs(Binning.back() - low) >
                 (std::numeric_limits<double>::epsilon() * 1E2)) {
        BinContent.push_back(0);
        BinError.push_back(0);
        Binning.push_back(low);
      }

      BinContent.push_back(hinp->GetBinContent(bin_it));
      BinError.push_back(hinp->GetBinError(bin_it));
      Binning.push_back(up);
    }
    slices.emplace_back(new TH1D(
        (std::string(hinp->GetName()) + "_slice" + std::to_string(sl_it++))
            .c_str(),
        (std::string(";") +
         (UseXAxis ? hinp->GetXaxis() : hinp->GetYaxis())->GetTitle() + ";" +
         hinp->GetZaxis()->GetTitle())
            .c_str(),
        Binning.size() - 1, Binning.data()));

    for (size_t bin_it = 0; bin_it < BinContent.size(); ++bin_it) {
      slices.back()->SetBinContent(bin_it + 1, BinContent[bin_it]);
      slices.back()->SetBinError(bin_it + 1, BinError[bin_it]);
    }
  }
  return slices;
}

void SliceNorm(std::unique_ptr<TH2> &hist, bool AlongY, char const *opt) {
  std::string opt_str = opt;
  std::transform(opt_str.begin(), opt_str.end(), opt_str.begin(), ::tolower);
  bool width = (opt_str.find("width") != std::string::npos);
  for (int slice_it = 0;
       slice_it < (AlongY ? hist->GetXaxis() : hist->GetYaxis())->GetNbins();
       ++slice_it) {
    int xl = AlongY ? slice_it + 1 : 1;
    int xu = AlongY ? slice_it + 1 : hist->GetXaxis()->GetNbins();

    int yl = AlongY ? 1 : slice_it + 1;
    int yu = AlongY ? hist->GetYaxis()->GetNbins() : slice_it + 1;

    double integral = hist->Integral(xl, xu, yl, yu, opt);

    if (integral <= 0) {
      continue;
    }

    double integ_check= 0;
    for (int bin_it = 0;
         bin_it < (AlongY ? hist->GetYaxis() : hist->GetXaxis())->GetNbins();
         ++bin_it) {
      double s = 1.0 / integral;
      if (width) {
        if (AlongY) {
          s /= (hist->GetXaxis()->GetBinWidth(slice_it + 1) *
                hist->GetYaxis()->GetBinWidth(bin_it + 1));
        } else {
          s /= (hist->GetYaxis()->GetBinWidth(slice_it + 1) *
                hist->GetXaxis()->GetBinWidth(bin_it + 1));
        }
      }
      if (AlongY) {
        hist->SetBinContent(slice_it + 1, bin_it + 1,
                            hist->GetBinContent(slice_it + 1, bin_it + 1) * s);
        hist->SetBinError(slice_it + 1, bin_it + 1,
                            hist->GetBinError(slice_it + 1, bin_it + 1) * s);
      } else {
        hist->SetBinContent(bin_it + 1, slice_it + 1,
                            hist->GetBinContent(bin_it + 1, slice_it + 1) * s);
        hist->SetBinError(bin_it + 1, slice_it + 1,
                            hist->GetBinError(bin_it + 1, slice_it + 1) * s);
      }
    }

  }
}

} // namespace utility
} // namespace nuis
