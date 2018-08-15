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

#include "TH2D.h"
#include "TMatrixD.h"
#include "TRandom3.h"
#include "TVectorD.h"

namespace SmearceptanceUtils {

double Smear1DProp(TH2D *, double TrueProp, TRandom3 *rand = NULL);

TVectorD SVDInverseSolve(TVectorD *inp, TMatrixD *mapping);
TVectorD SVDInverseSolve(TH1D *inp, TMatrixD *mapping);
TVectorD SVDInverseSolve(TH1D *inp, TH2D *mapping);
TH2D *SVDGetInverse(TH2D *mapping, int NToTruncate=0);
void GetSVDDecomp(TH2D *mapping, TVectorD &Sig, TMatrixD &U, TMatrixD &V);

TVectorD GetVector(TH1D *inp);
TVectorD GetErrorVector(TH1D *inp);
TMatrixD GetMatrix(TH2D *inp);
TH1D *GetTH1FromVector(TVectorD const &inp, TH1D *templ = NULL);
TH2D *GetTH2FromMatrix(TMatrixD const &inp, TH2D *templ = NULL);

TVectorD ThrowVectFromHist(TH1D *inp, TRandom3 *rnjesus, bool allowNeg);
void PushTH1ThroughMatrixWithErrors(TH1D *inp, TH1D *oup, TMatrixD &response,
                                    size_t NToys, bool allowNeg);

TH2D *SwapXYTH2D(TH2D *templ);
}
