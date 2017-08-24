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

#include "SmearceptanceUtils.h"

#include "TDecompSVD.h"

#include "FitLogger.h"

namespace SmearceptanceUtils {

double Smear1DProp(TH2D *mapping, double TrueProp, TRandom3 *rnjesus) {
  bool myrand = false;
  if (!rnjesus) {
    rnjesus = new TRandom3();
    myrand = true;
  }

  if (myrand) {
    delete rnjesus;
  }
  THROW("NIMPLEMENTED");
  return 0;
}

TVectorD SVDInverseSolve(TVectorD *inp, TMatrixD *mapping) {
  TDecompSVD svd(*mapping);
  bool ok;

  TVectorD c_svd = svd.Solve(*inp, ok);
  if (!ok) {
    THROW("Failed to solve SVD matrix equation.");
  }
  return c_svd;
}

TVectorD SVDInverseSolve(TH1D *inp, TMatrixD *mapping) {
  TVectorD inp_v = GetVector(inp);
  return SVDInverseSolve(&inp_v, mapping);
}

TVectorD SVDInverseSolve(TH1D *inp, TH2D *mapping) {
  TMatrixD mat(mapping->GetXaxis()->GetNbins(),
               mapping->GetYaxis()->GetNbins());

  for (Int_t xb_it = 0; xb_it < mapping->GetXaxis()->GetNbins(); ++xb_it) {
    for (Int_t yb_it = 0; yb_it < mapping->GetYaxis()->GetNbins(); ++yb_it) {
      mat[xb_it][yb_it] = mapping->GetBinContent(xb_it + 1, yb_it + 1);
    }
  }
  return SVDInverseSolve(inp, &mat);
}

TH2D *SVDGetInverse(TH2D *mapping, int NToTruncate) {
  TMatrixD mat = GetMatrix(mapping);

  if (mat.GetNcols() > mat.GetNrows()) {
    THROW("Trying to invert a " << mat.GetNrows() << "x" << mat.GetNcols()
                                << " matrix.");
  }

  TH2D *inverse = SwapXYTH2D(mapping);
  inverse->SetName("inverse");
  inverse->Reset();

  TDecompSVD svd(mat);
  svd.Decompose();

  if (NToTruncate) {
    TVectorD Sig(svd.GetSig());
    TMatrixD U(svd.GetU());
    TMatrixD V(svd.GetV());
    if (svd.GetV().TestBit(TMatrixD::kTransposed)) {
      THROW("ARGHH");
    }
    TMatrixD V_T = V.Transpose(V);

    TMatrixD Sig_TruncM(U.GetNrows(), V.GetNrows());
    for (Int_t i = 0; i < U.GetNrows(); ++i) {
      for (Int_t j = 0; j < V.GetNrows(); ++j) {
        Sig_TruncM[i][j] =
            ((i != j) || (i >= (Sig.GetNrows() - NToTruncate))) ? 0 : Sig[i];
      }
    }

    TMatrixD Trunc = U * Sig_TruncM * V_T;

    svd.~TDecompSVD();
    new (&svd) TDecompSVD(Trunc);
  }

  TMatrixD inv = svd.Invert();
  if (fabs(inv[mapping->GetXaxis()->GetNbins() / 2]
              [mapping->GetXaxis()->GetNbins() / 2] -
           mat[mapping->GetXaxis()->GetNbins() / 2]
              [mapping->GetXaxis()->GetNbins() / 2]) <
      std::numeric_limits<double>::epsilon()) {
    THROW("Failed to SVD invert matrix.");
  }

  for (Int_t xb_it = 0; xb_it < inverse->GetXaxis()->GetNbins(); ++xb_it) {
    for (Int_t yb_it = 0; yb_it < inverse->GetYaxis()->GetNbins(); ++yb_it) {
      inverse->SetBinContent(xb_it + 1, yb_it + 1, inv[yb_it][xb_it]);
    }
  }

  return inverse;
}

void GetSVDDecomp(TH2D *mapping, TVectorD &Sig, TMatrixD &U, TMatrixD &V) {
  TMatrixD mat = GetMatrix(mapping);

  TDecompSVD svd(mat);

  U.ResizeTo(svd.GetU());
  U = svd.GetU();

  V.ResizeTo(svd.GetV());
  V = svd.GetU();

  Sig.ResizeTo(svd.GetSig());
  Sig = svd.GetSig();
}

TVectorD GetVector(TH1D *inp) {
  TVectorD vec(inp->GetXaxis()->GetNbins());

  for (Int_t xb_it = 0; xb_it < inp->GetXaxis()->GetNbins(); ++xb_it) {
    vec[xb_it] = inp->GetBinContent(xb_it + 1);
  }
  return vec;
}
TVectorD GetErrorVector(TH1D *inp) {
  TVectorD vec(inp->GetXaxis()->GetNbins());

  for (Int_t xb_it = 0; xb_it < inp->GetXaxis()->GetNbins(); ++xb_it) {
    vec[xb_it] = inp->GetBinError(xb_it + 1);
  }
  return vec;
}
TMatrixD GetMatrix(TH2D *inp) {
  TMatrixD mat(inp->GetYaxis()->GetNbins(), inp->GetXaxis()->GetNbins());

  for (Int_t xb_it = 0; xb_it < inp->GetXaxis()->GetNbins(); ++xb_it) {
    for (Int_t yb_it = 0; yb_it < inp->GetYaxis()->GetNbins(); ++yb_it) {
      mat[yb_it][xb_it] = inp->GetBinContent(xb_it + 1, yb_it + 1);
    }
  }
  return mat;
}

TH1D *GetTH1FromVector(TVectorD const &inp, TH1D *templ) {
  TH1D *hist;
  if (templ) {
    hist = static_cast<TH1D *>(templ->Clone());
    hist->Reset();
    hist->SetName("vectHist");
  } else {
    hist = new TH1D("vectHist", "", inp.GetNrows(), 0, inp.GetNrows());
  }

  for (Int_t xb_it = 0; xb_it < inp.GetNrows(); ++xb_it) {
    hist->SetBinContent(xb_it + 1, inp[xb_it]);
  }

  return hist;
}

TH2D *GetTH2FromMatrix(TMatrixD const &inp, TH2D *templ) {
  TH2D *hist;
  if (templ) {
    hist = static_cast<TH2D *>(templ->Clone());
    hist->Reset();
    hist->SetName("matHist");
  } else {
    hist = new TH2D("matHist", "", inp.GetNcols(), 0, inp.GetNcols(),
                    inp.GetNrows(), 0, inp.GetNrows());
  }
  for (Int_t xb_it = 0; xb_it < inp.GetNcols(); ++xb_it) {
    for (Int_t yb_it = 0; yb_it < inp.GetNrows(); ++yb_it) {
      hist->SetBinContent(xb_it + 1, yb_it + 1, inp[yb_it][xb_it]);
    }
  }
  return hist;
}

TVectorD ThrowVectFromHist(TH1D *inp, TRandom3 *rnjesus, bool allowNeg) {
  TVectorD vec(inp->GetXaxis()->GetNbins());

  for (Int_t xb_it = 0; xb_it < inp->GetXaxis()->GetNbins(); ++xb_it) {
    size_t attempt = 0;
    do {
      if (attempt > 1000) {
        THROW("Looks like we aren't getting anywhere with this bin: "
              << inp->GetBinContent(xb_it + 1) << " +- "
              << inp->GetBinError(xb_it + 1));
      }
      vec[xb_it] = inp->GetBinContent(xb_it + 1) +
                   inp->GetBinError(xb_it + 1) * rnjesus->Gaus();
      attempt++;
    } while ((!allowNeg) && (vec[xb_it] < 0));
  }
  return vec;
}

void PushTH1ThroughMatrixWithErrors(TH1D *inp, TH1D *oup, TMatrixD &response,
                                    size_t NToys, bool allowNeg) {
  TRandom3 rnjesus;

  oup->Reset();

  TVectorD Mean(oup->GetXaxis()->GetNbins());
  TVectorD RMS(oup->GetXaxis()->GetNbins());
  std::vector<TVectorD> Toys;
  Toys.reserve(NToys);
  double NToysFact = 1.0 / double(NToys);

  for (size_t t_it = 0; t_it < NToys; ++t_it) {
    TVectorD Toy = ThrowVectFromHist(inp, &rnjesus, allowNeg);
    TVectorD UnfoldToy = response * Toy;

    for (Int_t bi_it = 0; bi_it < oup->GetXaxis()->GetNbins(); ++bi_it) {
      Mean[bi_it] += UnfoldToy[bi_it] * NToysFact;
    }
    Toys.push_back(UnfoldToy);
  }

  for (size_t t_it = 0; t_it < NToys; ++t_it) {
    for (Int_t bi_it = 0; bi_it < oup->GetXaxis()->GetNbins(); ++bi_it) {
      RMS[bi_it] += (Mean[bi_it] - Toys[t_it][bi_it]) *
                    (Mean[bi_it] - Toys[t_it][bi_it]) * NToysFact;
    }
  }

  for (Int_t bi_it = 0; bi_it < oup->GetXaxis()->GetNbins(); ++bi_it) {
    oup->SetBinContent(bi_it + 1, Mean[bi_it]);
    oup->SetBinError(bi_it + 1, sqrt(RMS[bi_it]));
  }
}

TH2D *SwapXYTH2D(TH2D *templ) {
  TH2D *Swapped = new TH2D(
      (std::string(templ->GetName()) + "_c").c_str(), "",
      templ->GetYaxis()->GetNbins(), templ->GetYaxis()->GetXbins()->GetArray(),
      templ->GetXaxis()->GetNbins(), templ->GetXaxis()->GetXbins()->GetArray());
  Swapped->Reset();

  std::string title = ";";
  title += templ->GetYaxis()->GetTitle();
  title += ";";
  title += templ->GetXaxis()->GetTitle();
  Swapped->SetTitle(title.c_str());

  for (Int_t x_it = 0; x_it < templ->GetXaxis()->GetNbins() + 2; ++x_it) {
    for (Int_t y_it = 0; y_it < templ->GetYaxis()->GetNbins() + 2; ++y_it) {
      Swapped->SetBinContent(y_it, x_it, templ->GetBinContent(x_it, y_it));
      Swapped->SetBinError(y_it, x_it, templ->GetBinError(x_it, y_it));
    }
  }
  return Swapped;
}
}
