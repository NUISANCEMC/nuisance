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

/**
 * Radecky et al. Phys Rev D, 3rd series, volume 25, number 5, 1 March 1982, p 1161-1173
*/

#include "ANL_CC1ppip_XSec_1DEnu_nu.h"

// The constructor
ANL_CC1ppip_XSec_1DEnu_nu::ANL_CC1ppip_XSec_1DEnu_nu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile) : wTrueCut(2.0), UseCorrectedData(true) {

  fName = "ANL_CC1ppip_XSec_1DEnu_nu";
  fPlotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 6.0;
  fIsDiag = true; // refers to covariance matrix; this measurement has none so only use errors, not covariance
  fNormError = 0.20; // normalisation error on ANL BNL flux
  fDefaultTypes = "FIX/DIAG";
  fAllowedTypes = "FIX,FREE,SHAPE/DIAG/UNCORR/CORR/W14/W16/NOW";

  // User can specify "UNCORR" for uncorrected data
  // Default is to use correction
  if (type.find("UNCORR") != std::string::npos) {
    UseCorrectedData = false;
  } else {
    UseCorrectedData = true;
  }

  // User can specify "W14" for W < 1.4 GeV cut
  //                  "W16" for W < 1.6 GeV cut
  //                  The default is no W cut
  if (type.find("W14") != std::string::npos) {
    wTrueCut = 1.4;
  } else if (type.find("W16") != std::string::npos) {
    wTrueCut = 1.6;
  } else {
    wTrueCut = 10.0;
  }

  if (UseCorrectedData && wTrueCut == 1.6) {
    ERR(FTL) << "Can not run ANL CC1pi+1p W < 1.6 GeV with CORRECTION, because the data DOES NOT EXIST" << std::endl;
    ERR(FTL) << "Correction exists for W < 1.4 GeV and no W cut data ONLY" << std::endl;
    ERR(FTL) << "Reverting to using uncorrected data!" << std::endl;
    UseCorrectedData = false;
  }
  // Get rid of the slashes in the type
  if (!type.empty() && type != "DEFAULT") {
    std::string temp_type = type;
    std::replace(temp_type.begin(), temp_type.end(), '/', '_');
    fName += "_"+temp_type;
  }

  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  // Now read in different data depending on what the user has specified
  std::string DataLocation = GeneralUtils::GetTopLevelDir()+"/data/ANL/CC1pip_on_p/";

  // If we're using corrected data
  if (UseCorrectedData) {
    if (wTrueCut == 1.4) {
      DataLocation += "anl82corr-numu-p-to-mu-p-piplus-lowW_edges.txt";
    } else {
      DataLocation += "anl82corr-numu-p-to-mu-p-piplus-noW_edges.txt";
    }
  // If we're using raw uncorrected data
  } else {

    if (wTrueCut == 1.4) {
      DataLocation += "anl82-numu-cc1ppip-14Wcut.txt";
    } else if (wTrueCut == 1.6) {
      DataLocation += "anl82-numu-cc1ppip-16Wcut.txt";
    } else if (wTrueCut == 10.0) {
      DataLocation += "anl82-numu-cc1ppip-noWcut.txt";
    }
  }

  SetDataValues(DataLocation);
  SetupDefaultHist();

  fFullCovar = StatUtils::MakeDiagonalCovarMatrix(fDataHist);
  covar     = StatUtils::GetInvert(fFullCovar);

  fScaleFactor = GetEventHistogram()->Integral("width")*double(1E-38)/double(fNEvents)*(16./8.);
};



void ANL_CC1ppip_XSec_1DEnu_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(2212) == 0 || event->NumFSParticle(211) == 0 || event->NumFSParticle(13) == 0) {
    return;
  }

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Pp   = event->GetHMFSParticle(2212)->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(211)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::MpPi(Pp, Ppip);
  double Enu     = -1.0;

  // ANL has a W cuts at 1.4, 1.6 and no w cut
  // This is set by user, or defaults to 2.0
  if (hadMass/1000. < wTrueCut) {
    Enu = Pnu.E()/1.E3;
  }

  fXVar = Enu;

  return;
}


bool ANL_CC1ppip_XSec_1DEnu_nu::isSignal(FitEvent *event) {
  return SignalDef::isCC1pi3Prong(event, 14, 211, 2212, EnuMin, EnuMax);
}

