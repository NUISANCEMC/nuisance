# Oscillation studies in NUISANCE

This simple module allows the construction of simple NOvA-style unfold/propagate/refold/compare style neutrino oscillation studies in NUSIANCE. It is implemented as a standard NUSIANCE sample, although with signficantly more complicated markup than usual.

## Full example

```xml
<nuisance>
   <OscParam detection_zenith_deg="95.8" />

   <parameter type="osc_parameter" name="dm23" nominal="2.54E-3" step="0.025E-3" low="2E-3" high="3.4E-3" state="FREE" />
   <parameter type="osc_parameter" name="sinsq_theta23" nominal="0.534" step="0.0025" low="0.35" high="0.7" state="FREE" />

   <parameter type="osc_parameter" name="dcp" nominal="0" step="0.0625" low="-3.14" high="3.14" state="FREE" />
   <parameter type="osc_parameter" name="sinsq_theta13" nominal="21.4E-3" step="1.25E-3" low="10E-3" high="50E-3" state="FREE" />

   <parameter type="osc_parameter" name="sinsq_theta12" nominal="0.297" state="FIX" />
   <parameter type="osc_parameter" name="dm12" nominal="7.37E-5" state="FIX" />

   <sample name="Smear_SVDUnfold_Propagation_Osc" input="HISTO:NDObs_numu.root[ELepRec_rate|ELepHadVis_Smear_ev]" NuPDG="14" TruncateStart="0" TruncateUpTo="10" DetectorVolume="36" DetectorDensity="1.395E3" SetErrorsFromRate="1" ScalePOT="10">

      <NDObs ObsInput="NDObs_nue.root[ELepRec_rate|ELepHadVis_Smear_ev]" FitRegion_max="6.99" NuPDG="12" TruncateStart="0" TruncateUpTo="10" />

      <FDObs ObsInput="FDObs_numu.root[ELepRec_rate],NDObs_numu.root[ELepHadVis_Smear_ev]" FitRegion_max="6.99" OscillateToPDG="14" DetectorVolume="1" DetectorDensity="40E6" >
         <FDNDRatio FromPDG="14" DivergenceFactor="1.50926829E-7" />
      </FDObs>

      <FDObs ObsInput="FDObs_nue.root[ELepRec_rate],FDObs_nue_intrinsic.root[ELepRec_rate],NDObs_nue.root[ELepHadVis_Smear_ev]" FitRegion_max="6.99" OscillateToPDG="12" DetectorVolume="1" DetectorDensity="40E6" >
         <FDNDRatio FromPDG="12" DivergenceFactor="1.26826492E-7" />
         <FDNDRatio FromPDG="14" DivergenceFactor="1.50926829E-7" />
      </FDObs>
   </sample>
</nuisance>
```

## Details

### NUISANCE Parameters

Explained in detail elsewhere, but briefly repeated here for completeness.
A reweightable parameter is specified by an element like:
```xml
   <parameter type="osc_parameter" name="dm23" nominal="2.54E-3" step="0.025E-3" low="2E-3" high="3.4E-3" state="FREE" />
```
where the attributes are hopefully self explanatory. The state should be `FREE`
or `FIXED` depending on whether in a minimisation or chi2 scan the parameter
should be allowed to vary. The valid names for oscillation parameters currently
implemented through Prob3++ are:
* `sinsq_theta12`
* `sinsq_theta13`
* `sinsq_theta23`
* `dm12`
* `dm23`
* `dcp`

In general, all parameters should be specified if any are, as the default values
may not be sensible.

The baseline is calculated from the assumption of a beam passing through the
earth to be detected on the surface.

### Analysis structure

The oscillation analysis inputs are near detector observations, far detector
observations and neutrino energy migration matricies that encode how detector
effects cause interactions of some true neutrino energy to be reconstructed
with some other neutrino energy. As oscillations happen as a function of
neutrino energy, being able to corectly identify the energy spectrum of
neutrinos before and after oscillation is the most key aspect of an oscillation
analysis.

The obseved near detector reconstructed energy spectrum is unfolded through the
near detector migration matrix to give the data-motivated, estimated true energy
spectrum of interactions at the near detector. This is then oscillated according
to the current PMNS parameters and corrected for far/near beam divergence and
detector mass. The true energy spectrum at the far detector is then smeared with
the far detector migration matrix, which may or may not be the same as the near
detector one, to give the predicted far detector reconstructed energy spectrum.
A test statistic is evaluated between the predicted and observed far detector
spectrum for the current set of oscillation, and potentially nuisance,
parameters.

In general the unfolding can be used to estimate the true energy spectrum of
the observed interactions, the true energy spectrum of the interactions that
occured (including efficiency), or the true energy spectrum of the neutrino
flux (also including cross-section). As the far over near correction can
currently only be a single number to account for beam divergence, it is advised
that the supplied migration matrices should account for the migration from true
interactions to true energy spectra (i.e. include the efficiency
correction/application at the near and far detectors). Currently, the nue and
numu cross sections are assumed identical.

**N.B.:** The near and far detector samples are assigned a neutrino PDG code, *i.e.* they must represent a CCInclusive sample of a single neutrino flavor. There is no support for interaction model-fitting style (T2K) analyses.

### The Sample element

```xml
<sample name="Smear_SVDUnfold_Propagation_Osc" input="HISTO:NDObs_numu.root[ELepRec_rate|ELepHadVis_Smear_ev]" NuPDG="14" TruncateStart="0" TruncateUpTo="10" DetectorVolume="36" DetectorDensity="1.395E3" SetErrorsFromRate="1" ScalePOT="10">
...
</sample>
```

Every oscillation analysis needs at least one near and one far detector sample.
To keep the structure of NUISANCE sample elements, the first near
detector sample is specified slightly differently to subsequent samples.

The sample element specifies the near detector mass through the attributes `DetectorVolume` and `DetectorDensity`, the separation is purely for your bookkeeping, they must multiply to give a mass in the same units as the far detector masses that will be specified. This is purely used for near-to-far extrapolation, the input histograms are expected to be well-normalised event rate predictions in the relevant near and far detector configurations.

The `ScalePOT` attribute is applied to all input histograms as a re-normalisation factor, this allows event rate predictions to be recalculated by a simple scale factor at runtime.

The `SetErrorsFromRate` attribute specifies whether the input histogram errors should be respected or they should be rescaled to the poisson error on the fully normalised event rate (included the `ScalePOT` attributed) in each bin.

**N.B.:** Only a single near detector can be specified, it would be fairly trivial to add multiple near detector setups, but that is currently not implemented. However, multiple near detector CCInc samples may be declared.

### ND sample descriptors

```xml
<NDObs ObsInput="NDObs_nue.root[ELepRec_rate|ELepHadVis_Smear_ev]" NuPDG="12" TruncateStart="0" TruncateUpTo="10" />
```

The input histograms for each ND sample should be given in the order: [observed rate in ERec], [ERec:ETrue migration matrix]. These can come from a single ROOT file, as shown above, or from two separate ROOT files, specified like: `ObsInput="FILE1.root[ObsRateERecHistName],FILE2.root[ERecETrueMigrationMatrixTH2DName]"`. The first ND sample, which is specified in the sample element, as opposed to in a child element named `NDObs`, must be prefixed with `HISTO:` to satisfy NUISANCE's requirement for input types.

The `NuPDG` attribute specifies the neutrinp PDG code which is observed sample corresponds to, most likely one of `-12,12,-14,14`, depending on the analysis setup.

The `TruncateStart` and `TruncateUpTo` attributes specify the number of singular values that may be set to 0 in the SVD decoposition to ensure that the unfolded spectrum at the near detector is >= 0 within the union of specified `FitRegion`s in the far detector samples (see the next section). If there are still negative values after the maximum allowed regularisation has been attempted, the analysis will throw an exception.

### FD sample descriptors

```xml
      <FDObs ObsInput="FDObs_nue.root[ELepRec_rate],FDObs_nue_intrinsic.root[ELepRec_rate],NDObs_nue.root[ELepHadVis_Smear_ev]" FitRegion_max="6.99" OscillateToPDG="12" DetectorVolume="1" DetectorDensity="40E6" >
         <FDNDRatio FromPDG="12" DivergenceFactor="1.26826492E-7" />
         <FDNDRatio FromPDG="14" DivergenceFactor="1.50926829E-7" />
      </FDObs>
```

Each far detector sample has the same inputs as each near detector sample, an observed histogram (correctly normalised) and a migration matrix. For the far detector samples, the migration matrix is not inverted, but used to forward-fold the true spectrum prediction at the far detector.

The two elements `FitRegion_min` and `FitRegion_max` are used to specify the range of the observation histogram that is included in the Chi2 calculation. These ranges are also used to inform the regularisation of the SVD inversion used ont he near detector samples.

Each far detector sample receives contributions from every near detector sample, these may well be 0, but the extrapolation is performed independently for each far detector sample. Therefore, each far detector sample element must set up its detector mass, even if they are all the same. In the example shown above the far detector is set up to have a 40 kilotonne fiducial mass.

The `OscillateToPDG` attribtue functions similarly to the `NuPDG` attribute for the near detector samples and specifies what species of neutrino the sample characterises, muon neutrino, electron anti-neutrino, etc... This is used to calculate the correction oscillation probability from each of the near detector samples. *e.g.* For a electron neutrino far detector sample, the muon neutrino to electron neutrino appearance probability is applied during the propagation to a muon neutrino near detector sample, and the electron neutrino survival probability is applied to an intrinsic electron neutrino near detector sample.

To inform the correct propagation of each near detector sample, a child `FDNDRatio` tag must be supplied. This specifies the beam divergence factor for each near detector sample --- linked via its `NuPDG` --- this is applied as a simple scale factor during near-to-far propagation. It would be possible to allow this a functional form to account for the relative difference in shape of the unoscillated flux at the near and far detector, but this is not yet implemented. Any near detector samples which do not have a corresponding `FDNDRatio` tag are given a divergence factor of `0` and effectively do not contribute to the far detector prediction. *e.g.* In the example at the top of this note, the far detector muon neutrino disapeerance sample recieves no contribution from the intrinsic electron neutrino sample, this is trivial to enable, but makes a completely insignificant difference to the far detector prediction.

## Input generation details

The input histograms are simplest to prepare through the `nuissmear` application, which by default will output both that are required for each input.

The observed ERec distribution for each sample should be self-explanatory. The response matrix should be supplied as a TH2D, with the true energy on the X axis and the reconstructed energy on the Y axis. The generation shape should be divided out of this. There is a small choice to be made here, either the efficiency correction can be included as part of the unfolding, this would involve dividing every bin by the total number of interaction generated in that bin of true neutrino energy -- events that are not reconstructed and thus do not make it into the histogram are accounted for by the sum of each Y slice of bins not totalling unity. Alternatively, you could re-normalised each Y slice so that the sum of bins totals to unity and this would mean that the unfolding only accounted for the migration and not the detector/selection efficiency. This implicitly would include the assumption into any susequent analysis that the detectors are functionally identical. It would also be possible to apply the efficiency ratio during the propagation, but this can only be a flat scale calculated from the ratio of detector masses and the beam divergence factor, it is not currently possible.
