// Copyright 2018 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#pragma once

#include "utility/ROOTUtility.hxx"

#include "exception/exception.hxx"

#include "string_parsers/from_string.hxx"

#include "fhiclcpp/ParameterSet.h"

#include "TAxis.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TH2Poly.h"

#include <iomanip>
#include <memory>
#include <string>
#include <vector>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unimplemented_GetHistogram_method);
NEW_NUIS_EXCEPT(invalid_histogram_descriptor);
NEW_NUIS_EXCEPT(invalid_histogram_name);
NEW_NUIS_EXCEPT(failed_to_clone);

bool IsFlowBin(TAxis const &ax, Int_t bin_it);

bool IsInHistogramRange(TAxis const &ax, double v);

template <typename HT> struct HType_traits {};
template <> struct HType_traits<TH1> {
  using type = TH1;
  static size_t const NDim = 1;
  using NumericT = double;
  static std::string name() { return "TH1"; }
};
template <> struct HType_traits<TH1D> {
  using type = TH1D;
  static size_t const NDim = 1;
  using NumericT = double;
  static std::string name() { return "TH1D"; }
};
template <> struct HType_traits<TH1F> {
  using type = TH1F;
  static size_t const NDim = 1;
  using NumericT = float;
  static std::string name() { return "TH1F"; }
};
template <> struct HType_traits<TH2> {
  using type = TH2;
  static size_t const NDim = 2;
  using NumericT = double;
  static std::string name() { return "TH2"; }
};
template <> struct HType_traits<TH2D> {
  using type = TH2D;
  static size_t const NDim = 2;
  using NumericT = double;
  static std::string name() { return "TH2D"; }
};
template <> struct HType_traits<TH2F> {
  using type = TH2F;
  static size_t const NDim = 2;
  using NumericT = float;
  static std::string name() { return "TH2F"; }
};
template <> struct HType_traits<TH2Poly> {
  using type = TH2Poly;
  static size_t const NDim = 0;
  using NumericT = double;
  static std::string name() { return "TH2Poly"; }
};

template <size_t, typename T = double> struct HType_Helper {};
template <> struct HType_Helper<1, void> {
  using type = TH1;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};
template <> struct HType_Helper<1, double> {
  using type = TH1D;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};
template <> struct HType_Helper<1, float> {
  using type = TH1F;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};
template <> struct HType_Helper<2, void> {
  using type = TH2;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};
template <> struct HType_Helper<2, double> {
  using type = TH2D;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};
template <> struct HType_Helper<2, float> {
  using type = TH2F;
  static size_t const NDim = HType_traits<type>::NDim;
  using NumericT = HType_traits<type>::NumericT;
  static std::string name() { return HType_traits<type>::name(); }
};

template <typename HT, typename Enable = void> struct TH_Helper {};

template <typename HT>
struct TH_Helper<HT,
                 typename std::enable_if<HType_traits<HT>::NDim == 1>::type> {
  static size_t const NDim = HType_traits<HT>::NDim;
  using NumericT = typename HType_traits<HT>::NumericT;
  static std::string name() { return HType_traits<HT>::name(); }

  static Int_t NbinsIncludeFlow(HT const &h) {
    return h.GetXaxis()->GetNbins() + 2;
  }
  static Int_t Nbins(HT const &h) { return h.GetXaxis()->GetNbins(); }

  static bool IsFlowBin(HT const &h, Int_t bin_it) {
    return nuis::utility::IsFlowBin(*h.GetXaxis(), bin_it);
  }

  static void Fill(HT &h, std::array<NumericT, NDim> const &v, double w = 1) {
    h.Fill(v[0], w);
  }

  static void Scale(HT &h, NumericT SF, char const *opt = "") {
    h.Scale(SF, opt);
  }

  static double Integral(HT const &h, char const *opt = "") {
    return h.Integral(opt);
  }

  static Int_t NbinsIncludeFlow(std::unique_ptr<HT> const &h) {
    return NbinsIncludeFlow(*h);
  }
  static Int_t Nbins(std::unique_ptr<HT> const &h) { return Nbins(*h); }

  static bool IsFlowBin(std::unique_ptr<HT> const &h, Int_t bin_it) {
    return IsFlowBin(*h, bin_it);
  }

  static void Fill(std::unique_ptr<HT> &h, std::array<NumericT, NDim> const &v,
                   double w = 1) {
    Fill(*h, v, w);
  }

  static void Scale(std::unique_ptr<HT> &h, NumericT SF, char const *opt = "") {
    Scale(*h, SF, opt);
  }

  static double Integral(std::unique_ptr<HT> const &h, char const *opt = "") {
    return Integral(*h, opt);
  }
};

template <typename HT>
struct TH_Helper<HT,
                 typename std::enable_if<HType_traits<HT>::NDim == 2>::type> {
  static size_t const NDim = HType_traits<HT>::NDim;
  using NumericT = typename HType_traits<HT>::NumericT;
  static std::string name() { return HType_traits<HT>::name(); }

  // TH2 ***************************************************************
  static Int_t NbinsIncludeFlow(HT const &h) {
    return (h.GetXaxis()->GetNbins() + 2) * (h.GetYaxis()->GetNbins() + 2);
  }
  static Int_t Nbins(HT const &h) {
    return (h.GetXaxis()->GetNbins()) * (h.GetYaxis()->GetNbins());
  }

  static bool IsFlowBin(HT const &h, Int_t xbin_it, Int_t ybin_it) {
    return nuis::utility::IsFlowBin(*h.GetXaxis(), xbin_it) ||
           nuis::utility::IsFlowBin(*h.GetYaxis(), ybin_it);
  }

  static void Fill(HT &h, std::array<NumericT, NDim> const &v, double w = 1) {
    h.Fill(v[0], v[1], w);
  }

  static void Scale(HT &h, NumericT SF, char const *opt = "") {
    h.Scale(SF, opt);
  }

  static double Integral(HT const &h, char const *opt = "") {
    return h.Integral(opt);
  }

  static Int_t NbinsIncludeFlow(std::unique_ptr<HT> const &h) {
    return NbinsIncludeFlow(*h);
  }
  static Int_t Nbins(std::unique_ptr<HT> const &h) { return Nbins(*h); }

  static bool IsFlowBin(std::unique_ptr<HT> const &h, Int_t xbin_it,
                        Int_t ybin_it) {
    return IsFlowBin(*h, xbin_it, ybin_it);
  }

  static void Fill(std::unique_ptr<HT> &h, std::array<NumericT, NDim> const &v,
                   double w = 1) {
    Fill(*h, v, w);
  }

  static void Scale(std::unique_ptr<HT> &h, NumericT SF, char const *opt = "") {
    Scale(*h, SF, opt);
  }

  static double Integral(std::unique_ptr<HT> const &h, char const *opt = "") {
    return Integral(*h, opt);
  }
};

template <typename HT>
struct TH_Helper<
    HT, typename std::enable_if<std::is_same<HT, TH2Poly>::value>::type> {
  static size_t const NDim = 2;
  using NumericT = typename HType_traits<HT>::NumericT;
  static std::string name() { return HType_traits<HT>::name(); }

  // TH2Poly ***************************************************************
  static Int_t NbinsIncludeFlow(HT const &h) { return h.GetNumberOfBins() + 9; }
  static Int_t Nbins(HT const &h) { return h.GetNumberOfBins(); }

  static bool IsFlowBin(HT const &h, Int_t bin_it) { return (bin_it < 0); }

  static void Fill(HT &h, std::array<NumericT, NDim> const &v, double w = 1) {
    h.Fill(v[0], v[1], w);
  }

  static void Scale(HT &h, NumericT SF, char const *opt = "") {

    bool width = (std::string(opt).find("width") != std::string::npos);
    size_t nbins = Nbins(h);
    for (size_t bin_it = 0; bin_it < nbins; ++bin_it) {
      double bin_area = 1;

      if (width) {
        TH2PolyBin *poly_bin =
            dynamic_cast<TH2PolyBin *>(h.GetBins()->At(bin_it));

        bin_area = poly_bin->GetArea();
      }

      h.SetBinContent(bin_it + 1,
                      h.GetBinContent(bin_it + 1) * (SF / bin_area));
      h.SetBinError(bin_it + 1, h.GetBinError(bin_it + 1) * (SF / bin_area));
    }
  }

  static double Integral(HT &h, char const *opt = "") {
    bool width = (std::string(opt).find("width") != std::string::npos);
    size_t nbins = Nbins(h);
    double integral = 0;
    for (size_t bin_it = 0; bin_it < nbins; ++bin_it) {
      double bin_area = 1;

      if (width) {
        TH2PolyBin *poly_bin =
            dynamic_cast<TH2PolyBin *>(h.GetBins()->At(bin_it));

        bin_area = poly_bin->GetArea();
      }

      integral += h.GetBinContent(bin_it + 1) * bin_area;
    }
    return integral;
  }

  static Int_t NbinsIncludeFlow(std::unique_ptr<HT> const &h) {
    return NbinsIncludeFlow(*h);
  }
  static Int_t Nbins(std::unique_ptr<HT> const &h) { return Nbins(*h); }

  static bool IsFlowBin(std::unique_ptr<HT> const &h, Int_t bin_it) {
    return IsFlowBin(*h, bin_it);
  }

  static void Fill(std::unique_ptr<HT> &h, std::array<NumericT, NDim> const &v,
                   double w = 1) {
    Fill(*h, v, w);
  }

  static void Scale(std::unique_ptr<HT> &h, NumericT SF, char const *opt = "") {
    Scale(*h, SF, opt);
  }

  static double Integral(std::unique_ptr<HT> &h, char const *opt = "") {
    return Integral(*h, opt);
  }
};

template <typename HT>
void Clear(typename std::enable_if<HType_traits<HT>::NDim != 0, HT>::type &h) {
  for (Int_t bin_it = 0; bin_it < TH_Helper<HT>::NbinsIncludeFlow(h);
       ++bin_it) {
    h.SetBinContent(bin_it, 0);
    h.SetBinError(bin_it, 0);
  }
}

template <typename HT>
void Clear(
    typename std::enable_if<std::is_same<HT, TH2Poly>::value, HT>::type &h) {
  h.ClearBinContents();
}

template <typename HT>
inline std::unique_ptr<HT> Clone(HT const &source, bool clear = false,
                                 std::string const &clone_name = "") {
  std::unique_ptr<HT> target(dynamic_cast<HT *>(
      source.Clone(clone_name.size() ? clone_name.c_str() : "")));
  if (!target) {
    throw failed_to_clone()
        << "[ERROR]: Failed to clone a " << TH_Helper<HT>::name() << ".";
  }
  target->SetDirectory(nullptr);

  if (clear) {
    Clear<HT>(*target);
  }

  return target;
}
template <typename HT>
inline std::unique_ptr<HT> Clone(std::unique_ptr<HT> const &source,
                                 bool clear = false,
                                 std::string const &clone_name = "") {
  return Clone(*source, clear, clone_name);
}

template <typename HT>
inline std::unique_ptr<HT>
GetHistogramFromROOTFile(TFile_ptr &f, std::string const &hname,
                         bool ThrowIfMissing = true) {

  TObject *obj = f->Get(hname.c_str());
  if(std::string(obj->ClassName()) == "TList"){
    obj = static_cast<TList *>(obj)->At(0);
  }

  HT *h = dynamic_cast<HT *>(obj);
  if (!h) {
    if (ThrowIfMissing) {
      throw invalid_histogram_name()
          << "[ERROR]: Failed to get " << TH_Helper<HT>::name() << " named "
          << std::quoted(hname) << " from input file "
          << std::quoted(f->GetName());
    } else {
      return nullptr;
    }
  }
  std::unique_ptr<HT> clone = Clone<HT>(*h);
  return clone;
}

template <typename HT>
inline std::unique_ptr<HT>
GetHistogramFromROOTFile(std::string const &fname, std::string const &hname,
                         bool ThrowIfMissing = true) {
  TFile_ptr temp = CheckOpenTFile(fname, "READ");
  return GetHistogramFromROOTFile<HT>(temp, hname, ThrowIfMissing);
}

template <typename HT>
std::unique_ptr<HT> GetHistogram(std::string const &input_descriptor) {
  std::vector<std::string> split_descriptor =
      fhicl::string_parsers::ParseToVect<std::string>(input_descriptor, ";",
                                                      true, true);

  if (split_descriptor.size() == 1) { // assume text
    throw unimplemented_GetHistogram_method();
  } else if (split_descriptor.size() == 2) {
    return GetHistogramFromROOTFile<HT>(split_descriptor[0],
                                        split_descriptor[1]);
  } else {
    throw invalid_histogram_descriptor()
        << "[ERROR]: Failed to parse histogram descriptor: "
        << std::quoted(input_descriptor)
        << " as an input histogram (Text/ROOT).";
  }
}

struct PolyBinSpecifier {
  double X, Y;
  bool UseXAxis;
};

constexpr PolyBinSpecifier XPolyBinSpec(double X, double Y) {
  return PolyBinSpecifier{X + std::numeric_limits<double>::epsilon() * 1E2,
                          Y + std::numeric_limits<double>::epsilon() * 1E2,
                          true};
}

constexpr PolyBinSpecifier YPolyBinSpec(double X, double Y) {
  return PolyBinSpecifier{X + std::numeric_limits<double>::epsilon() * 1E2,
                          Y + std::numeric_limits<double>::epsilon() * 1E2,
                          false};
}

std::vector<std::unique_ptr<TH1>> GetTH2PolySlices(
    std::unique_ptr<TH2Poly> &hinp,
    std::vector<std::vector<PolyBinSpecifier>> const &BinsSpecifiers);

template <typename HT>
inline typename std::enable_if<HType_traits<HT>::NDim == 1,
                               std::unique_ptr<TH1>>::type
BuildHistFromFHiCL(fhicl::ParameterSet const &ps) {
  std::array<double, 3> xaxis =
      ps.get<std::array<double, 3>>("xaxis_descriptor");
  std::unique_ptr<TH1> rtn = std::make_unique<TH1D>(
      ps.get<std::string>("name", "").c_str(),
      ps.get<std::string>("title", "").c_str(), xaxis[0], xaxis[1], xaxis[2]);
  rtn->SetDirectory(nullptr);
  return rtn;
}

template <typename HT>
inline typename std::enable_if<HType_traits<HT>::NDim == 2,
                               std::unique_ptr<TH2>>::type
BuildHistFromFHiCL(fhicl::ParameterSet const &ps) {
  std::array<double, 3> xaxis =
      ps.get<std::array<double, 3>>("xaxis_descriptor");
  std::array<double, 3> yaxis =
      ps.get<std::array<double, 3>>("yaxis_descriptor");
  std::unique_ptr<TH2> rtn =
      std::make_unique<TH2D>(ps.get<std::string>("name", "").c_str(),
                             ps.get<std::string>("title", "").c_str(), xaxis[0],
                             xaxis[1], xaxis[2], yaxis[0], yaxis[1], yaxis[2]);
  rtn->SetDirectory(nullptr);
  return rtn;
}

static bool const kYSlice = true;
static bool const kXSlice = false;
void SliceNorm(std::unique_ptr<TH2> &hist, bool AlongY = kYSlice,
               char const *opt = "");

} // namespace utility
} // namespace nuis
