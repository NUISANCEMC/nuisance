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

#include "TH1D.h"
#include "StatUtils.h"

//*******************************************************************
Double_t StatUtils::GetChi2FromDiag(TH1D* data, TH1D* mc, TH1I* mask) {
//*******************************************************************

  Double_t Chi2 = 0.0;
  TH1D* calc_data = (TH1D*)data->Clone();
  TH1D* calc_mc   = (TH1D*)mc->Clone();

  // Add MC Error to data if required
  if (FitPar::Config().GetParB("statutils.addmcerror")) {
    for (int i = 0; i < calc_data->GetNbinsX(); i++) {

      double dterr = calc_data->GetBinError(i + 1);
      double mcerr = calc_mc->GetBinError(i + 1);

      if (dterr > 0.0) {
        calc_data->SetBinError(i + 1, sqrt(dterr * dterr + mcerr * mcerr));
      }
    }
  }

  // Apply masking if required
  if (mask) {
    calc_data = ApplyHistogramMasking(data, mask);
    calc_mc   = ApplyHistogramMasking(mc, mask);
  }

  // Iterate over bins in X
  for (int i = 0; i < calc_data->GetNbinsX(); i++) {

    // Ignore bins with zero data or zero bin error
    if (calc_data->GetBinError(i + 1) <= 0.0 ||
        calc_data->GetBinContent(i + 1) == 0.0) continue;

    // Take mc data difference
    double diff = calc_data->GetBinContent(i + 1) - calc_mc->GetBinContent(i + 1);
    double err = calc_data->GetBinError(i + 1);
    Chi2 += (diff * diff) / (err * err);

  }

  // cleanup
  delete calc_data;
  delete calc_mc;

  return Chi2;
};

//*******************************************************************
Double_t StatUtils::GetChi2FromDiag(TH2D* data, TH2D* mc,
                                    TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)  map = GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate 1D chi2 from 1D Plots
  Double_t Chi2 = StatUtils:: GetChi2FromDiag(data_1D, mc_1D,  mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return Chi2;
};

//*******************************************************************
Double_t StatUtils::GetChi2FromCov(TH1D* data, TH1D* mc,
                                   TMatrixDSym* invcov, TH1I* mask) {
//*******************************************************************

  Double_t Chi2 = 0.0;
  TMatrixDSym* calc_cov = (TMatrixDSym*) invcov->Clone();
  TH1D* calc_data = (TH1D*) data->Clone();
  TH1D* calc_mc   = (TH1D*) mc->Clone();

  // If a mask if applied we need to apply it before the matrix is inverted
  if (mask) {
    calc_cov  = ApplyInvertedMatrixMasking(invcov, mask);
    calc_data = ApplyHistogramMasking(data, mask);
    calc_mc   = ApplyHistogramMasking(mc, mask);
  }

  // Add MC Error to data if required
  if (FitPar::Config().GetParB("statutils.addmcerror")) {

    // Make temp cov
    TMatrixDSym* newcov = StatUtils::GetInvert(calc_cov);

    // Add MC err to diag
    for (int i = 0; i < calc_data->GetNbinsX(); i++) {

      double mcerr = calc_mc->GetBinError(i + 1) * 1E38;
      double oldval = (*newcov)(i, i);

      (*newcov)(i, i) = oldval + mcerr * mcerr;
    }

    // Reset the calc_cov to new invert
    delete calc_cov;
    calc_cov = GetInvert(newcov);

    // Delete the tempcov
    delete newcov;
  }

  // iterate over bins in X (i,j)
  for (int i = 0; i < calc_data->GetNbinsX(); i++) {

    for (int j = 0; j < calc_data->GetNbinsX(); j++) {

      if (calc_data->GetBinContent(i + 1) != 0 || calc_mc->GetBinContent(i + 1) != 0) {

        LOG(DEB) << "i j = " << i << " " << j << std::endl;
        LOG(DEB) << "Calc_data mc i = " << calc_data->GetBinContent(i + 1)
                 << " " << calc_mc->GetBinContent(i + 1)
                 << "  Dif = "
                 << ( calc_data->GetBinContent(i + 1) - calc_mc->GetBinContent(i + 1) )
                 << std::endl;

        LOG(DEB) << "Calc_data mc i = " << calc_data->GetBinContent(j + 1)
                 << " " << calc_mc->GetBinContent(j + 1)
                 << "  Dif = "
                 << ( calc_data->GetBinContent(j + 1) - calc_mc->GetBinContent(j + 1) )
                 << std::endl;

        LOG(DEB) << "Covar = " <<    (*calc_cov)(i, j) << std::endl;

        LOG(DEB) << "Cont chi2 = " \
                 << ( ( calc_data->GetBinContent(i + 1) - calc_mc->GetBinContent(i + 1) ) \
                      * (*calc_cov)(i, j) \
                      * 1E76 \
                      *   ( calc_data->GetBinContent(j + 1) - calc_mc->GetBinContent(j + 1) ) )
                 << " " << Chi2 << std::endl;

        Chi2 += ( ( calc_data->GetBinContent(i + 1) - calc_mc->GetBinContent(i + 1) ) * \
                  (*calc_cov)(i, j) * 1E76        * \
                  ( calc_data->GetBinContent(j + 1) - calc_mc->GetBinContent(j + 1) ) );

      } else {

        LOG(DEB) << "Error on bin (i,j) = (" << i << "," << j << ")" << std::endl;
        LOG(DEB) << "data->GetBinContent(i+1) = " << calc_data->GetBinContent(i + 1) << std::endl;
        LOG(DEB) << "mc->GetBinContent(i+1) = " << calc_mc->GetBinContent(i + 1) << std::endl;
        LOG(DEB) << "Adding zero to chi2 instead of dying horrifically " << std::endl;

        Chi2 += 0.;
      }
    }
  }

  // Cleanup
  delete calc_cov;
  delete calc_data;
  delete calc_mc;

  return Chi2;
}


//*******************************************************************
Double_t StatUtils::GetChi2FromCov( TH2D* data, TH2D* mc,
                                    TMatrixDSym* invcov, TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map) {
    map = StatUtils::GenerateMap(data);
  }

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate 1D chi2 from 1D Plots
  Double_t Chi2 = StatUtils::GetChi2FromCov(data_1D, mc_1D,  invcov, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return Chi2;
}

//*******************************************************************
Double_t StatUtils::GetChi2FromSVD( TH1D* data, TH1D* mc,
                                    TMatrixDSym* cov,  TH1I* mask) {
//*******************************************************************

  Double_t Chi2 = 0.0;
  TMatrixDSym* calc_cov = (TMatrixDSym*) cov->Clone();
  TH1D* calc_data = (TH1D*) data->Clone();
  TH1D* calc_mc   = (TH1D*) mc->Clone();

  // If a mask if applied we need to apply it before the matrix is inverted
  if (mask) {
    calc_cov  = StatUtils::ApplyMatrixMasking(cov, mask);
    calc_data = StatUtils::ApplyHistogramMasking(data, mask);
    calc_mc   = StatUtils::ApplyHistogramMasking(mc, mask);
  }

  // Decompose matrix
  TDecompSVD LU = TDecompSVD((*calc_cov));
  LU.Decompose();
  TMatrixDSym* cov_U = new TMatrixDSym(calc_data->GetNbinsX(), LU .GetU().GetMatrixArray(), "");
  TVectorD*    cov_S = new TVectorD( LU.GetSig() );

  // Apply basis rotation before adding up chi2
  Double_t rotated_difference = 0.0;

  for (int i = 0; i < calc_data->GetNbinsX(); i++) {
    rotated_difference = 0.0;

    // Rotate basis of Data - MC
    for (int j = 0; j < calc_data->GetNbinsY(); j++)
      rotated_difference += ( calc_data->GetBinContent(j + 1) - calc_mc  ->GetBinContent(j + 1) ) * (*cov_U)(j, i) ;

    // Divide by rotated error cov_S
    Chi2 += rotated_difference * rotated_difference * 1E76 / (*cov_S)(i);

  }

  // Cleanup
  delete calc_cov;
  delete calc_data;
  delete calc_mc;
  delete cov_U;
  delete cov_S;

  return Chi2;
}


//*******************************************************************
Double_t StatUtils::GetChi2FromSVD( TH2D* data, TH2D* mc,
                                    TMatrixDSym* cov,    TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t Chi2 = StatUtils::GetChi2FromSVD(data_1D, mc_1D, cov, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return Chi2;
}

//*******************************************************************
double StatUtils::GetChi2FromEventRate(TH1D* data, TH1D* mc, TH1I* mask) {
//*******************************************************************

  // If just an event rate, for chi2 just use Poission Likelihood to calculate the chi2 component
  double chi2 = 0.0;
  TH1D* calc_data = (TH1D*)data->Clone();
  TH1D* calc_mc   = (TH1D*)mc->Clone();

  // Apply masking if required
  if (mask) {
    calc_data = ApplyHistogramMasking(data, mask);
    calc_mc   = ApplyHistogramMasking(mc, mask);
  }

  // Iterate over bins in X
  for (int i = 0; i < calc_data->GetNbinsX(); i++) {

    double dt = calc_data->GetBinContent(i + 1);
    double mc = calc_mc->GetBinContent(i + 1);

    if (mc <= 0) continue;

    if (dt <= 0) {
      // Only add difference
      chi2 += 2 * (mc - dt);
    } else {
      // Do the chi2 for Poisson distributions
      chi2 +=  2 * (mc - dt + (dt * log(dt / mc)));
    }

    /*
        LOG(REC)<<"Evt Chi2 cont = "<<i<<" "
          <<mc<<" "<<dt<<" "
          <<2 * (mc - dt + (dt+0.) * log((dt+0.) / (mc+0.)))
          <<" "<<Chi2<<std::endl;
    */
  }

  // cleanup
  delete calc_data;
  delete calc_mc;

  return chi2;
}

//*******************************************************************
Double_t StatUtils::GetChi2FromEventRate(TH2D* data, TH2D* mc, TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t Chi2 = StatUtils::GetChi2FromEventRate(data_1D, mc_1D, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return Chi2;
}




//*******************************************************************
Double_t StatUtils::GetLikelihoodFromDiag(TH1D* data, TH1D* mc, TH1I* mask) {
//*******************************************************************
  // Currently just a placeholder!
  (void) data;
  (void) mc;
  (void) mask;
  return 0.0;
};

//*******************************************************************
Double_t StatUtils::GetLikelihoodFromDiag(TH2D* data, TH2D* mc, TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t MLE = StatUtils::GetLikelihoodFromDiag(data_1D, mc_1D, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return MLE;
};


//*******************************************************************
Double_t StatUtils::GetLikelihoodFromCov( TH1D* data, TH1D* mc, TMatrixDSym* invcov, TH1I* mask) {
//*******************************************************************
  // Currently just a placeholder !
  (void) data;
  (void) mc;
  (void) invcov;
  (void) mask;

  return 0.0;
};

//*******************************************************************
Double_t StatUtils::GetLikelihoodFromCov( TH2D* data, TH2D* mc, TMatrixDSym* invcov, TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t MLE = StatUtils::GetLikelihoodFromCov(data_1D, mc_1D, invcov, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return MLE;
};

//*******************************************************************
Double_t StatUtils::GetLikelihoodFromSVD( TH1D* data, TH1D* mc, TMatrixDSym* cov,    TH1I* mask) {
//*******************************************************************
  // Currently just a placeholder!
  (void) data;
  (void) mc;
  (void) cov;
  (void) mask;

  return 0.0;
};

//*******************************************************************
Double_t StatUtils::GetLikelihoodFromSVD( TH2D* data, TH2D* mc, TMatrixDSym* cov,    TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t MLE = StatUtils::GetLikelihoodFromSVD(data_1D, mc_1D, cov, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return MLE;
};

//*******************************************************************
Double_t StatUtils::GetLikelihoodFromEventRate(TH1D* data, TH1D* mc, TH1I* mask) {
//*******************************************************************
  // Currently just a placeholder!
  (void) data;
  (void) mc;
  (void) mask;

  return 0.0;
};


//*******************************************************************
Double_t StatUtils::GetLikelihoodFromEventRate(TH2D* data, TH2D* mc, TH2I* map, TH2I* mask) {
//*******************************************************************

  // Generate a simple map
  if (!map)
    map = StatUtils::GenerateMap(data);

  // Convert to 1D Histograms
  TH1D* data_1D = MapToTH1D(data, map);
  TH1D* mc_1D   = MapToTH1D(mc,   map);
  TH1I* mask_1D = MapToMask(mask, map);

  // Calculate from 1D
  Double_t MLE = StatUtils::GetChi2FromEventRate(data_1D, mc_1D, mask_1D);

  // CleanUp
  delete data_1D;
  delete mc_1D;
  delete mask_1D;

  return MLE;
};



//*******************************************************************
Int_t StatUtils::GetNDOF(TH1D* hist, TH1I* mask) {
//*******************************************************************

  TH1D* calc_hist = (TH1D*)hist->Clone();

  // If a mask is provided we need to apply it before getting NDOF
  if (mask) {
    calc_hist = StatUtils::ApplyHistogramMasking(hist, mask);
  }

  // NDOF is defined as total number of bins with non-zero errors
  Int_t NDOF = 0;
  for (int i = 0; i < calc_hist->GetNbinsX(); i++) {
    if (calc_hist->GetBinError(i + 1) > 0.0) NDOF++;
  }

  delete calc_hist;

  return NDOF;
};


//*******************************************************************
Int_t StatUtils::GetNDOF(TH2D* hist, TH2I* map, TH2I* mask) {
//*******************************************************************

  Int_t NDOF = 0;
  if (!map) map = StatUtils::GenerateMap(hist);

  for (int i = 0; i < hist->GetNbinsX(); i++) {
    for (int j = 0; j < hist->GetNbinsY(); j++) {

      if (mask->GetBinContent(i + 1, j + 1)) continue;
      if (map->GetBinContent(i + 1, j + 1) <= 0) continue;

      NDOF++;

    }
  }

  return NDOF;
};



//*******************************************************************
TH1D* StatUtils::ThrowHistogram(TH1D* hist, TMatrixDSym* cov, bool throwdiag, TH1I* mask) {
//*******************************************************************

  TH1D* calc_hist = (TH1D*) hist->Clone( (std::string(hist->GetName()) + "_THROW" ).c_str() );
  TMatrixDSym* calc_cov = (TMatrixDSym*) cov->Clone();
  Double_t correl_val = 0.0;

  // If a mask if applied we need to apply it before the matrix is decomposed
  if (mask) {
    calc_cov  = ApplyMatrixMasking(cov, mask);
    calc_hist = ApplyHistogramMasking(calc_hist, mask);
  }

  // If a covariance is provided we need a preset random vector and a decomp
  std::vector<Double_t> rand_val;
  TMatrixDSym* decomp_cov;

  if (cov) {
    for (int i = 0; i < hist->GetNbinsX(); i++) {
      rand_val.push_back(gRandom->Gaus(0.0, 1.0));
    }

    // Decomp the matrix
    decomp_cov = StatUtils::GetDecomp(calc_cov);
  }

  // iterate over bins
  for (int i = 0; i < hist->GetNbinsX(); i++) {

    // By Default the errors on the histogram are thrown uncorrelated to the other errors
    //    if (throwdiag) {
    //      calc_hist->SetBinContent(i + 1, (calc_hist->GetBinContent(i + 1) + \
				       gRandom->Gaus(0.0, 1.0) * calc_hist->GetBinError(i + 1)) );
      //    }

    // If a covariance is provided that is also thrown
    if (cov) {
      correl_val = 0.0;

      for (int j = 0; j < hist->GetNbinsX(); j++) {
        correl_val += rand_val[j] * (*decomp_cov)(j, i) ;
      }      
      calc_hist->SetBinContent(i + 1, (calc_hist->GetBinContent(i + 1) + correl_val * 1E-38));
    }
  }

  delete calc_cov;
  delete decomp_cov;

  // return this new thrown data
  return calc_hist;
};

//*******************************************************************
TH2D* StatUtils::ThrowHistogram(TH2D* hist, TMatrixDSym* cov, TH2I* map, bool throwdiag, TH2I* mask) {
//*******************************************************************

  // PLACEHOLDER!!!!!!!!!
  // Currently no support for throwing 2D Histograms from a covariance
  (void) hist;
  (void) cov;
  (void) map;
  (void) throwdiag;
  (void) mask;

  // /todo
  // Sort maps if required
  // Throw the covariance for a 1D plot
  // Unmap back to 2D Histogram

  return hist;
}






//*******************************************************************
TH1D* StatUtils::ApplyHistogramMasking(TH1D* hist, TH1I* mask) {
//*******************************************************************

  if (!mask) return ( (TH1D*)hist->Clone() );

  // This masking is only sufficient for chi2 calculations, and will have dodgy bin edges.

  // Get New Bin Count
  Int_t NBins = 0;
  for (int i = 0; i < hist->GetNbinsX(); i++) {
    if (mask->GetBinContent(i + 1)) continue;
    NBins++;
  }

  // Make new hist
  std::string newmaskname = std::string(hist->GetName()) + "_MSKD";
  TH1D* calc_hist = new TH1D( newmaskname.c_str(), newmaskname.c_str(), NBins, 0, NBins);

  // fill new hist
  int binindex = 0;
  for (int i = 0; i < hist->GetNbinsX(); i++) {
    if (mask->GetBinContent(i + 1)) {
      LOG(REC) << "Applying mask to bin " << i + 1 << " " << hist->GetName() << std::endl;
      continue;
    }
    calc_hist->SetBinContent(binindex + 1, hist->GetBinContent(i + 1));
    calc_hist->SetBinError(binindex + 1, hist->GetBinError(i + 1));
    binindex++;
  }

  return calc_hist;
};

//*******************************************************************
TH2D* StatUtils::ApplyHistogramMasking(TH2D* hist, TH2I* mask) {
//*******************************************************************

  TH2D* newhist = (TH2D*) hist->Clone();

  if (!mask) return newhist;

  for (int i = 0; i < hist->GetNbinsX(); i++) {
    for (int j = 0; j < hist->GetNbinsY(); j++) {

      if (mask->GetBinContent(i + 1, j + 1) > 0) {
        newhist->SetBinContent(i + 1, j + 1, 0.0);
        newhist->SetBinContent(i + 1, j + 1, 0.0);
      }
    }
  }

  return newhist;
}

//*******************************************************************
TMatrixDSym* StatUtils::ApplyMatrixMasking(TMatrixDSym* mat, TH1I* mask) {
//*******************************************************************

  if (!mask) return (TMatrixDSym*)(mat->Clone());

  // Get New Bin Count
  Int_t NBins = 0;
  for (int i = 0; i < mask->GetNbinsX(); i++) {
    if (mask->GetBinContent(i + 1)) continue;
    NBins++;
  }

  // make new matrix
  TMatrixDSym* calc_mat = new TMatrixDSym(NBins);
  int col, row;

  // Need to mask out bins in the current matrix
  row = 0;
  for (int i = 0; i < mask->GetNbinsX(); i++) {
    col = 0;

    // skip if masked
    if (mask->GetBinContent(i + 1) > 0.5) continue;

    for (int j = 0; j < mask->GetNbinsX(); j++) {

      // skip if masked
      if (mask->GetBinContent(j + 1) > 0.5) continue;

      (*calc_mat)(row, col) = (*mat)(i, j);
      col++;
    }
    row++;
  }

  return calc_mat;
};

//*******************************************************************
TMatrixDSym* StatUtils::ApplyMatrixMasking(TMatrixDSym* mat, TH2D* data, TH2I* mask, TH2I* map) {
//*******************************************************************

  if (!map) map = StatUtils::GenerateMap(data);
  TH1I* mask_1D = StatUtils::MapToMask(mask, map);

  TMatrixDSym* newmat = StatUtils::ApplyMatrixMasking(mat,  mask_1D);

  delete mask_1D;
  return newmat;
}



//*******************************************************************
TMatrixDSym* StatUtils::ApplyInvertedMatrixMasking(TMatrixDSym* mat, TH1I* mask) {
//*******************************************************************

  TMatrixDSym* new_mat = GetInvert(mat);
  TMatrixDSym* masked_mat = ApplyMatrixMasking(new_mat, mask);

  TMatrixDSym* inverted_mat = GetInvert(masked_mat);

  delete masked_mat;
  delete new_mat;

  return inverted_mat;
};



//*******************************************************************
TMatrixDSym* StatUtils::ApplyInvertedMatrixMasking(TMatrixDSym* mat, TH2D* data, TH2I* mask, TH2I* map) {
//*******************************************************************

  if (!map) map = StatUtils::GenerateMap(data);
  TH1I* mask_1D = StatUtils::MapToMask(mask, map);

  TMatrixDSym* newmat = ApplyInvertedMatrixMasking(mat, mask_1D);

  delete mask_1D;
  return newmat;
}




//*******************************************************************
TMatrixDSym* StatUtils::GetInvert(TMatrixDSym* mat) {
//*******************************************************************

  TMatrixDSym* new_mat = (TMatrixDSym*)mat->Clone();

  // Check for diagonal
  bool non_diagonal = false;
  for (int i = 0; i < new_mat->GetNrows(); i++) {
    for (int j = 0; j < new_mat->GetNrows(); j++) {
      if (i == j) continue;

      if ((*new_mat)(i, j) != 0.0) {
        non_diagonal = true;
        break;
      }
    }
  }

  // If diag, just flip the diag
  if (!non_diagonal or new_mat->GetNrows() == 1) {
    for (int i = 0; i < new_mat->GetNrows(); i++) {
      if ((*new_mat)(i, i) != 0.0)
        (*new_mat)(i, i) = 1.0 / (*new_mat)(i, i);
      else
        (*new_mat)(i, i) = 0.0;
    }
    return new_mat;
  }


  // Invert full matrix
  TDecompSVD LU = TDecompSVD((*new_mat));
  new_mat = new TMatrixDSym(new_mat->GetNrows(), LU.Invert().GetMatrixArray(), "");

  return new_mat;
}

//*******************************************************************
TMatrixDSym* StatUtils::GetDecomp(TMatrixDSym* mat) {
//*******************************************************************

  TMatrixDSym* new_mat = (TMatrixDSym*)mat->Clone();
  int nrows = new_mat->GetNrows();

  // Check for diagonal
  bool diagonal = true;
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < nrows; j++) {
      if (i == j) continue;

      if ((*new_mat)(i, j) != 0.0) {
        diagonal = false;
        break;
      }
    }
  }

  // If diag, just flip the diag
  if (diagonal or nrows == 1) {
    for (int i = 0; i < nrows; i++) {
      if ((*new_mat)(i, i) > 0.0)
        (*new_mat)(i, i) = sqrt((*new_mat)(i, i));
      else
        (*new_mat)(i, i) = 0.0;
    }
    return new_mat;
  }

  TDecompChol LU = TDecompChol(*new_mat);
  LU.Decompose();
  delete new_mat;

  TMatrixDSym* dec_mat = new TMatrixDSym(nrows, LU.GetU().GetMatrixArray(), "");

  return dec_mat;
}





//*******************************************************************
void StatUtils::ForceNormIntoCovar(TMatrixDSym* mat, TH1D* hist, double norm) {
//*******************************************************************

  if (!mat) mat = MakeDiagonalCovarMatrix(hist);

  int nbins = mat->GetNrows();
  TMatrixDSym* new_mat = new TMatrixDSym(nbins);

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {

      double valx = hist->GetBinContent(i + 1) * 1E38;
      double valy = hist->GetBinContent(j + 1) * 1E38;

      (*new_mat)(i, j) = (*mat)(i, j) + norm * norm * valx * valy;

    }
  }

  // Swap the two
  delete mat;
  mat = new_mat;

  return;
};

//*******************************************************************
void StatUtils::ForceNormIntoCovar(TMatrixDSym* mat, TH2D* data, double norm, TH2I* map ) {
//*******************************************************************

  if (!map) map = StatUtils::GenerateMap(data);
  TH1D* data_1D = MapToTH1D(data, map);

  StatUtils::ForceNormIntoCovar(mat, data_1D, norm);
  delete data_1D;

  return;
}

//*******************************************************************
TMatrixDSym* StatUtils::MakeDiagonalCovarMatrix(TH1D* data, double scaleF) {
//*******************************************************************

  TMatrixDSym* newmat = new TMatrixDSym(data->GetNbinsX());

  for (int i = 0; i < data->GetNbinsX(); i++) {
    (*newmat)(i, i) = data->GetBinError(i + 1) * data->GetBinError(i + 1) * scaleF * scaleF;
  }

  return newmat;
}



//*******************************************************************
TMatrixDSym* StatUtils::MakeDiagonalCovarMatrix(TH2D* data, TH2I* map, double scaleF) {
//*******************************************************************

  if (!map) map = StatUtils::GenerateMap(data);
  TH1D* data_1D = MapToTH1D(data, map);

  return StatUtils::MakeDiagonalCovarMatrix(data_1D, scaleF);
};


//*******************************************************************
void StatUtils::SetDataErrorFromCov(TH1D* data, TMatrixDSym* cov, double scale) {
//*******************************************************************

  // Check
  if (cov->GetNrows() != data->GetNbinsX()) {
    ERR(WRN) << "Nrows in cov don't match nbins in data for SetDataErrorFromCov" << std::endl;
  }

  // Set bin errors form cov diag
  for (int i = 0; i < data->GetNbinsX(); i++) {
    data->SetBinError(i + 1, sqrt((*cov)(i, i)) * scale );
  }

  return;
}

//*******************************************************************
void StatUtils::SetDataErrorFromCov(TH2D* data, TMatrixDSym* cov, TH2I* map, double scale) {
//*******************************************************************

  // Create map if required
  if (!map) map = StatUtils::GenerateMap(data);
  std::cout << data << " " << cov <<  " " << map <<  " " << scale << std::endl;

  // Set Bin Errors from cov diag
  int count = 0;
  for (int i = 0; i < data->GetNbinsX(); i++) {
    for (int j = 0; j < data->GetNbinsY(); j++) {

      if (data->GetBinContent(i + 1, j + 1) == 0.0) continue;

      count = map->GetBinContent(i + 1, j + 1) - 1;
      data->SetBinError(i + 1, j + 1,  sqrt((*cov)(count, count)) * scale );

    }
  }

  return;
}

//*******************************************************************
TH2I* StatUtils::GenerateMap(TH2D* hist) {
//*******************************************************************

  std::string maptitle = std::string(hist->GetName()) + "_MAP";

  TH2I* map = new TH2I( maptitle.c_str(), maptitle.c_str(),
                        hist->GetNbinsX(), 0, hist->GetNbinsX(),
                        hist->GetNbinsY(), 0, hist->GetNbinsY());

  Int_t index = 1;

  for (int i = 0; i < hist->GetNbinsX(); i++) {
    for (int j = 0; j < hist->GetNbinsY(); j++) {

      if (hist->GetBinContent(i + 1, j + 1) > 0 && hist->GetBinError(i + 1, j + 1) > 0) {

        map->SetBinContent(i + 1, j + 1, index);
        index++;
      } else {
        map->SetBinContent(i + 1, j + 1, 0);
      }
    }
  }

  return map;
}

//*******************************************************************
TH1D* StatUtils::MapToTH1D(TH2D* hist, TH2I* map) {
//*******************************************************************

  if (!hist) return NULL;

  // Get N bins for 1D plot
  Int_t Nbins = map->GetMaximum();
  std::string name1D = std::string(hist->GetName()) + "_1D";

  // Make new 1D Hist
  TH1D* newhist = new TH1D(name1D.c_str(), name1D.c_str(), Nbins, 0, Nbins);

  // map bin contents
  for (int i = 0; i < map->GetNbinsX(); i++) {
    for (int j = 0; j < map->GetNbinsY(); j++) {

      if (map->GetBinContent(i + 1, j + 1) == 0) continue;

      newhist->SetBinContent(map->GetBinContent(i + 1, j + 1), hist->GetBinContent(i + 1, j + 1));
      newhist->SetBinError(map->GetBinContent(i + 1, j + 1),   hist->GetBinError(i + 1, j + 1));
    }
  }

  // return
  return newhist;
}


//*******************************************************************
TH1I* StatUtils::MapToMask(TH2I* hist, TH2I* map) {
//*******************************************************************

  TH1I* newhist = NULL;
  if (!hist) return newhist;

  // Get N bins for 1D plot
  Int_t Nbins = map->GetMaximum();
  std::string name1D = std::string(hist->GetName()) + "_1D";

  // Make new 1D Hist
  newhist = new TH1I(name1D.c_str(), name1D.c_str(), Nbins, 0, Nbins);

  // map bin contents
  for (int i = 0; i < map->GetNbinsX(); i++) {
    for (int j = 0; j < map->GetNbinsY(); j++) {

      if (map->GetBinContent(i + 1, j + 1) == 0) continue;

      newhist->SetBinContent(map->GetBinContent(i + 1, j + 1), hist->GetBinContent(i + 1, j + 1));
    }
  }

  // return
  return newhist;
}


TMatrixDSym* StatUtils::GetCovarFromCorrel(TMatrixDSym* correl, TH1D* data) {

  int nbins = correl->GetNrows();
  TMatrixDSym* covar = new TMatrixDSym(nbins);

  for (int i = 0; i < nbins; i++) {
    for (int j = 0; j < nbins; j++) {
      (*covar)(i, j) = (*correl)(i, j) * data->GetBinError(i + 1) * data->GetBinError(j + 1);
    }
  }

  return covar;
}



//*******************************************************************
TMatrixD* StatUtils::GetMatrixFromTextFile(std::string covfile, int dimx, int dimy) {
//*******************************************************************

  // Determine dim
  if (dimx == -1 and dimy == -1) {
    std::string line;
    std::ifstream covar(covfile.c_str(), ifstream::in);

    int row = 0;
    while (std::getline(covar >> std::ws, line, '\n')) {
      int column = 0;

      std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
      for (std::vector<double>::iterator iter = entries.begin();
           iter != entries.end(); iter++) {
        column++;

        if (column > dimx) dimx = column;
      }
      row++;
      if (row > dimy) dimy = row;
    }
  }

  // Or assume symmetric
  if (dimx != -1 and dimy == -1) {
    dimy = dimx;
  }
  assert(dimy != -1 && " matrix dimy not set.");

  // Make new matrix
  TMatrixD* mat = new TMatrixD(dimx, dimy);
  std::string line;
  std::ifstream covar(covfile.c_str(), ifstream::in);

  int row = 0;
  while (std::getline(covar >> std::ws, line, '\n')) {
    int column = 0;

    std::vector<double> entries = GeneralUtils::ParseToDbl(line, " ");
    for (std::vector<double>::iterator iter = entries.begin();
         iter != entries.end(); iter++) {

      // Check Rows
      //assert(row > mat->GetNrows() && " covar rows doesn't match matrix rows.");
      //assert(column > mat->GetNcols() && " covar cols doesn't match matrix cols.");

      // Fill Matrix
      (*mat)(row, column) = (*iter);
      column++;
    }
    row++;
  }

  return mat;
}

//*******************************************************************
TMatrixD* StatUtils::GetMatrixFromRootFile(std::string covfile, std::string histname) {
//*******************************************************************

  std::string inputfile = covfile + ";" + histname;
  std::vector<std::string> splitfile = GeneralUtils::ParseToStr(inputfile, ";");

  if (splitfile.size() < 2) {
    ERR(FTL) << "No object name given!" << std::endl;
    throw;
  }

  // Get file
  TFile* tempfile = new TFile(splitfile[0].c_str(), "READ");

  // Get Object
  TObject* obj = tempfile->Get(splitfile[1].c_str());
  if (!obj) {
    ERR(FTL) << "Object " << splitfile[1] << " doesn't exist!" << std::endl;
    throw;
  }

  // Try casting
  TMatrixD* mat = dynamic_cast<TMatrixD*>(obj);
  if (mat) {

    TMatrixD* newmat = (TMatrixD*)mat->Clone();

    delete mat;
    tempfile->Close();

    return newmat;
  }

  TMatrixDSym* matsym = dynamic_cast<TMatrixDSym*>(obj);
  if (matsym) {

    TMatrixD* newmat = new TMatrixD(matsym->GetNrows(), matsym->GetNrows());
    for (int i = 0; i < matsym->GetNrows(); i++) {
      for (int j = 0; j < matsym->GetNrows(); j++) {
        (*newmat)(i, j) = (*matsym)(i, j);
      }
    }

    delete matsym;
    tempfile->Close();

    return newmat;
  }

  TH2D* mathist = dynamic_cast<TH2D*>(obj);
  if (mathist) {
    TMatrixD* newmat = new TMatrixD(mathist->GetNbinsX(), mathist->GetNbinsX());
    for (int i = 0; i < mathist->GetNbinsX(); i++) {
      for (int j = 0; j < mathist->GetNbinsX(); j++) {
        (*newmat)(i, j) = mathist->GetBinContent(i + 1, j + 1);
      }
    }

    delete mathist;
    tempfile->Close();

    return newmat;
  }
  
  return NULL;
}

//*******************************************************************
TMatrixDSym* StatUtils::GetCovarFromTextFile(std::string covfile, int dim){
//*******************************************************************

  // Delete TempMat
  TMatrixD* tempmat = GetMatrixFromTextFile(covfile, dim, dim);

  // Make a symmetric covariance
  TMatrixDSym* newmat = new TMatrixDSym(tempmat->GetNrows());
  for (int i = 0; i < tempmat->GetNrows(); i++){
    for (int j = 0; j < tempmat->GetNrows(); j++){
      (*newmat)(i,j) = (*tempmat)(i,j);
    }
  }


  delete tempmat;
  return newmat;
}

//*******************************************************************
TMatrixDSym* StatUtils::GetCovarFromRootFile(std::string covfile, std::string histname){
//*******************************************************************

  TMatrixD* tempmat = GetMatrixFromRootFile(covfile, histname);
  TMatrixDSym* newmat = new TMatrixDSym(tempmat->GetNrows());

  for (int i = 0; i < tempmat->GetNrows(); i++){
    for (int j = 0; j < tempmat->GetNrows(); j++){
      (*newmat)(i,j) = (*tempmat)(i,j);
    }
  }

  delete tempmat;
  return newmat;
}
