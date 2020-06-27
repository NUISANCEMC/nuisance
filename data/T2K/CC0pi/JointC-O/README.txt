This folder contains the data release for the paper "Combined measurement of the muon neutrino charged-current cross section on Oxygen and Carbon without pion in the final state at T2K" (https://journals.aps.org/prd/abstract/10.1103/PhysRevD.101.112004).

1. Binning.txt contains the 2D binning: 1st column is the lowest edge of the muon costheta bin, 2nd column is the highest edge of the muon costheta bin, 3rd columns is the lowest edge of the muon momentum bin and the 4th column is the highest edge of the highest edge of the muon momentum bin.

2. cc0pi_xsec_O-C-ratio_noreg.txt contains the unregularized extracted double differential cross sections. From left to right: bin number, oxygen cross section, absolute uncertainty on the oxygen cross section, carbon cross section, absolute uncertainty on the carbon cross section, O/C cross section ratio and its absolute uncertainty. Units are 10^-39 cm^2/(GeV nucleons).

3. cc0pi_xsec_O-C-ratio_reg.txt contains the regularized extracted double differential cross sections. From left to right: bin number, oxygen cross section, absolute uncertainty on the oxygen cross section, carbon cross section, absolute uncertainty on the carbon cross section, O/C cross section ratio and its absolute uncertainty. Units are 10^-39 cm^2/(GeV nucleons). 

4. covmatrix_noreg.root contains the absolute covariance matrices (TMatrixTSym<double>) for the unregularized results.
- covmatrixOeCbin is the full covariance matrices where first 29 bins are for oxygen and last 29 are for carbon, so in total 58 bins
- covmatrixOCratio is the covariance matrix for the O/C ratio, so 29 bins
- covmatrixObin is the covariance matrix for oxygen only (i.e. the first 29x29bins submatrix of the covmatrixOeCbinnorm)
- covmatrixCbin is the corvariance matrix for carbon only (i.e. the last 29x29bins submatrix of the covmatrixOeCbinnorm)
- corrmatrixOCbin is the full correlation matrix where first 29 bins are for oxygen and last 29 are for carbon, so in total 58 bins

5. covmatrix_reg.root contains the absolute covariance matrices (TMatrixTSym<double>) for the regularized results.
- covmatrixOeCbin is the full covariance matrices where first 29 bins are for oxygen and last 29 are for carbon, so in total 58 bins
- covmatrixOCratio is the covariance matrix for the O/C ratio, so 29 bins
- covmatrixObin is the covariance matrix for oxygen only (i.e. the first 29x29bins submatrix of the covmatrixOeCbinnorm)
- covmatrixCbin is the corvariance matrix for carbon only (i.e. the last 29x29bins submatrix of the covmatrixOeCbinnorm)
- corrmatrixOCbin is the full correlation matrix where first 29 bins are for oxygen and last 29 are for carbon, so in total 58 bins

6. linear_unreg_results_C_nuisance.root contains histograms for the unregularized cross section on carbon:
- LinResult: is a one-dimensional histogram where every bin is filled with the double differential cross section on carbon. Units are 10^-39 cm^2/(GeV nucleons). 
- datapoly is a TH2Poly histograms containing the 2D carbon results. y axis represents bins in momentum and x axis is costheta
- result_cthmu are the projected results on the costheta axis, not divided by the bin width
- dataslices_0 to 5: these histograms represent the double differential cross section in each costheta bin

7. linear_reg_results_C_nuisance.root same as before, but for regularized results 

8. linear_unreg_results_O_nuisance.root contains histograms for the unregularized cross section on oxygen:
- LinResult: is a one-dimensional histogram where every bin is filled with the double differential cross section on carbon. Units are 10^-39 cm^2/(GeV nucleons). 
- datapoly is a TH2Poly histograms containing the 2D carbon results. y axis represents bins in momentum and x axis is costheta
- result_cthmu are the projected results on the costheta axis, not divided by the bin width
- dataslices_0 to 5: these histograms represent the double differential cross section in each costheta bin

9. linear_reg_results_O_nuisance.root same as before, but for regularized results 

10. The flux used for this analysis and the model comparison can be found here: https://t2k-experiment.org/result_category/flux/ : under "T2K flux prediction 2016", chose the root file t2kflux_2016_plus250kA.root and histogram enu_nd280_numu
