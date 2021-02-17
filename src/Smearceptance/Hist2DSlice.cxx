// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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

#include "Hist2DSlice.h"

Int_t Hist2DSlice::GetNbinsX() { return Bins.size(); }
double Hist2DSlice::GetBinContent_Index(size_t i) { return Bins[i].second; }
double Hist2DSlice::GetBinNumber_Index(size_t i) { return Bins[i].first; }
double Hist2DSlice::GetBinContent_BinNum(Int_t i);
double Hist2DSlice::GetMaximum() { return Max; }

Hist2DSlice::Hist2DSlice(TH2D const *Hist, Int_t YBinNum) {
  std::stringstream ss("");
  ss << Hist->GetName() << "_yslice_" << YBinNum;
  Slice = new TH1D(ss.str().c_str(),"",);
  Slice->SetDirectory(NULL);
  for (Int_t i = 0; i < (Hist->GetXaxis()->GetNbins() + 2); ++i) {
    Int_t MatrixBin = Hist->GetBin(i, YBinNum);
    Float_t cont = Hist->GetBinContent(MatrixBin);
    if (cont <= 0) {
      continue;
    }
    Bins.push_back(std::make_pair(i, cont));
    Max = std::max(Max, Bins.back().second);
  }

  NOrigBinsX = Hist->GetXaxis()->GetNbins();
  return GetNbinsX();
}

Float_t Hist2DSlice::GetBinContent_BinNum(Int_t binnum) {
  for (size_t i = 0; i < Bins.size(); ++i) {
    if (Bins[i].first == binnum) {
      return Bins[i].second;
    }
  }
  return 0;
}
