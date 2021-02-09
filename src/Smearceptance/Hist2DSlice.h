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

#ifndef HIST2DSLICE_HXX_SEEN
#define HIST2DSLICE_HXX_SEEN

class Hist2DSlice {
  TH1D *Slice;

  bool IsSparse;
  std::vector<std::pair<Int_t, double> > SparseBins;
  double Max;
  Int_t NOrigBinsX;

  Int_t GetNbinsX();
  double GetBinContent_Index(size_t i);
  double GetBinNumber_Index(size_t i);
  double GetBinContent_BinNum(Int_t i);
  double GetMaximum();

 public:
  Hist2DSlice(TH2D const *Hist, Int_t YBinNum);
  Int_t ThrowBin(TRandom3 *);
  double ThrowXValue(TRandom3 *);
};

#endif
