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

#include "T2K_SignalDef.h"

#include "T2K_CC0pinp_STV_XSec_1Ddpt_nu.h"

//********************************************************************
T2K_CC0pinp_STV_XSec_1Ddpt_nu::T2K_CC0pinp_STV_XSec_1Ddpt_nu(
    std::string inputfile, FitWeight *rw, std::string type,
    std::string fakeDataFile)
//********************************************************************
{
  fName = "T2K_CC0pinp_STV_XSec_1Ddpt_nu";
  fDefaultTypes = "FIX/DIAG/CHI2";
  fPlotTitles =
      "; #delta#it{p}_{T} (GeV c^{-1}); #frac{d#sigma}{d#delta#it{p}_{T}} "
      "(cm^{2} neutron^{-1} GeV^{-1} c)";
  EnuMin = 0;
  EnuMax = 50;
  fIsDiag = true;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  SetDataValues(GeneralUtils::GetTopLevelDir() +
                "/data/T2K/T2K_CC0pinp_STV_XSec_1Ddpt_nu.dat");

  fDataHist->Scale(1E-38);
  fDataTrue->Scale(1E-38);

  SetupDefaultHist();

  fScaleFactor = fEventHist->Integral("width") * double(1E-38) *
                (13.0 / 6.0 /*Data is /neutron */) /
                (double(fNEvents) * TotalIntegratedFlux("width"));
};

void T2K_CC0pinp_STV_XSec_1Ddpt_nu::FillEventVariables(FitEvent *event) {
  fXVar = FitUtils::Get_STV_dpt(event, true) / 1000.0;
  return;
};

//********************************************************************
bool T2K_CC0pinp_STV_XSec_1Ddpt_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isT2K_CC0pi_STV(event, EnuMin, EnuMax);
}
