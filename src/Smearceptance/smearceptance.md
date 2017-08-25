# NUISANCE Smearceptance

1. Introduction
2. Quickstart: Applying thresholds
3. Included smearcepters
4. Writing your own

## 1. Introduction
A generalised 'Fast MC' interface for NUISANCE. Smearers, accepters, and 
smearcepters can be written and configured to mock up the effects of using a 
'realistic' detection techniques on any input event format that NUISANCE can 
read. A simple example of this would be a threshold accepter, which lets the 
user define charged particle tracking  thresholds for different types of final 
state particles. These thresholds would be applied so that any downstream 
analysis can only see particles above threshold.

The eventual aim of such a module would be to expand the use of NUISANCE as a 
data-release platform so that analysers could add their 
reconstruction-level/uncorrected results, along with the information to 
forward-fold, or 'smearcept' theory inputs for comparison and model tuning. It 
is not currently clear that this is feasible, but it is an interesting 
possibility.

The current use cases include generating fake data for different types of 
detector technology, running simple mock experiments as a first gauge of 
potential sensitivities, and simple model overlays for uncorrected data from 
both nu-A and e-A scattering experiments.

The interface is designed to be abstract, and very simple. With any real data 
and analyses, the devil is often in the detail and it is likely that tailored 
Smearcepters would have to be written for each analysis. However, a number of 
examples and simple smearcepters already exist and are useful for applying 
thresholds, multi-dimensional efficiency curves, and simple detector smearing 
effects.

## 2. Quickstart: Applying thresholds

Steps to acceptance:
* Write a smearcepter card file, `WaterCherenkovThresholds.xml`:
```xml
<nuisance>
  <smearcepters>
      <ThresholdAccepter Name="WCThresh">
        <RecoThreshold PDG="13" RecoThresholdMomentum_MeV="160" />
        <RecoThreshold PDG="2212" RecoThresholdMomentum_MeV="1400" />
      </ThresholdAccepter>
  </smearcepters>
</nuisance>
```
* Produce a smeared event summary tree: 
`$ nuissmear -i InputVector.root -c WaterCherenkovThresholds.xml -t WCThresh -o WaterCherenkov_summary.root`
* Draw plots from the event tree: 
`[root] FlatTree_VARS->Draw("EISLep_true:EISLep_LepHad_rec >> (100,0,2000,100,0,2000)","Weight*(flagCCINC_rec==1)","COLZ")`

The branchs in the event summary tree are described in detail the class 
documentation of `src/MCStudies/Smearceptance_Tester.cxx`

## 3. Included smearcepters

### General

Smearcepters are defined in a NUISANCE xml configuration file within a tag of 
the `<nuisance>` root element named `<smearcepters>`. Any number can be defined, 
and each should be distinctly named with a `Name="<name>"` attribute. 

An example XML tag is shown below that highlights some of the notation used in 
this section.
```xml
<tag A[B|C]="<name>" (D="<value>") E="[F|G]" />
```
* `AB` and `AC` are both valid attribute names, at least one of which is a 
required attribute.
* `D` is an optional attribute.
* `<name>` is an example attribute value.
* The attribute `E` must take either the value `F` or `G`.

### ThresholdAccepter

Applies tracking and visible energy thresholds to an input vector.

#### Full example
```xml
<nuisance>
  <smearcepters>
      <ThresholdAccepter Name="ForwardMuonHighHadronThresholdDetector">
        <RecoThreshold PDG="13" RecoThresholdMomentum_MeV="300" />
        <RecoThreshold PDG="13" RecoThresholdCosTheta_Min="0.5" />
        <RecoThreshold PDG="2212" RecoThresholdMomentum_MeV="500" />
        <RecoThreshold PDG="2212" RecoThresholdAbsCosTheta_Min="0.5" />
        <RecoThreshold PDG="221,-211" RecoThresholdKE_MeV="120" />
        
        <VisThreshold PDG="2212" VisThresholdKE_MeV="10" Contrib="K" />
        <VisThreshold PDG="111" VisThresholdKE_MeV="0" Contrib="T" />
        <VisThreshold PDG="221,-211" VisThresholdKE_MeV="30" Contrib="K" Fraction="0.9" />
      </ThresholdAccepter>
  </smearcepters>
</nuisance>
```

#### Details
A tracking threshold can be placed on a particles production zenith angle by 
the element:
* `<RecoThreshold PDG="<211,-211,2212>" RecoThresholdCosTheta_[Min,Max]="<value>" />`
* `<RecoThreshold PDG="<X,Y,Z>" RecoThresholdAbsCosTheta_[Min,Max]="<value>" />`

Tracking and visible energy threshold values can be applied by the following 
elements:
* Particle momentum: Use `<[Reco|Vis]Threshold PDG="<X,Y,Z>"  [Reco|Vis]ThresholdMomentum_MeV="<value>" (Contrib="[T|K]" Fraction="<value>") />`
* Particle kinetic energy: Use `<[Reco|Vis]Threshold PDG="<X,Y,Z>"  [Reco|Vis]ThresholdKE_MeV="<value>" (Contrib="[T|K]" Fraction="<value>") />` 

When defining visible energy thresholds, the additional attributes 
`Contrib="[<T>,K]"` and `Fraction="<value>"` can be specified to detail whether 
the energy deposit contribution comes from the particle total or kinetic energy 
only and what fraction of that deposit is visible. The deposited energy defaults 
to the particle total energy and the fraction defaults to 1.

If a particle kinematics do not exceed a momentum or kinetic energy threshold 
for tracking they are tested against the visible energy deposit threshold. If a 
particle is rejected due to an angular threshold, it is not allowed to deposit 
energy.

Multiple thresholds can target the same input PDG, however it is undefined 
behavior to use more than one energy/momentum tracking threshold, or use both 
`Abs` and non-`Abs` zenith angle cuts.

*N.B.* If a PDG code is encountered for which there are no defined thresholds, 
it is not accepted.

### GaussianSmearer

Documentation to follow. Sorry.

### EfficiencyApplicator

Applies tracking efficiencies from one, two, or three dimensional efficiencies 
passed in as input.

#### Full example
```xml
      <EfficiencyApplicator Name="SimpleLArLike">
        <EfficiencyCurve PDG="13" InputFile="uBooNEEffs.root" HistName="Tracking/Muon/p_costheta" NDims="2" XAxis="kMomentum" YAxis="kCosTheta" Interpolate="false" XAxisScaleToMeV="1E3" />
        <EfficiencyCurve PDG="211,-211" InputFile="uBooNEEffs.root" HistName="Tracking/Pion/p_costheta" NDims="2" XAxis="kMomentum" YAxis="kCosTheta" Interpolate="false" XAxisScaleToMeV="1E3" />
        <EfficiencyCurve PDG="2212" InputFile="uBooNEEffs.root" HistName="Tracking/Proton/p_costheta" NDims="2" XAxis="kMomentum" YAxis="kCosTheta" Interpolate="false" XAxisScaleToMeV="1E3" />
        
        <VisThreshold PDG="2212" VisThresholdKE_MeV="10" Contrib="K" />
        <VisThreshold PDG="211,-211" VisThresholdKE_MeV="30" Contrib="K" />
        <VisThreshold PDG="111" VisThresholdKE_MeV="0" Contrib="T" />
      </EfficiencyApplicator>
```
#### Details

Each `<EfficiencyCurve>` tag requires at least:
* `PDG="<211,-211>"`: The particle IDs to apply this efficiency to.
* `InputFile="<file.root>"`: The location of the ROOT file containing the 
efficiency histogram.
* `HistName="<histname>"`: The name (may include directories) of the `TH1` or 
`TEfficiency` that describes the efficiency.
* `NDims="[1|2|3]"`: The number of kinematic axes for the efficiency curve.
* `XAxis="[kMomentum|kKE|kCosTheta|kTheta|kPhi]"`: The kinematic variable 
spanned by the first dimension of the efficiency histogram.

optional attributes:
* `YAxis="[kMomentum|kKE|kCosTheta|kTheta|kPhi]"`: The kinematic variable 
spanned by the second dimension of the efficiency histogram.
* `ZAxis="[kMomentum|kKE|kCosTheta|kTheta|kPhi]"`: The kinematic variable 
spanned by the third dimension of the efficiency histogram.
* `Interpolate="[true|false]"`: Whether to use TH1:Interpolate or to take the 
bin-averaged efficiency.
* `[X|Y|Z]AxisScaleToInternal="<value>"`: A scale factor to translate the axes 
to the internal NUISANCE units (MeV and radians). *e.g.* Use 
`XAxisScaleToInternal="1E3"` if the input histogram uses kinetic energy in units 
of GeV.

An `EfficiencyApplicator` Can also contain `<RecoThreshold>` and 
`<VisThreshold>` tags, as described in `ThresholdAccepter`. Particles that are 
rejected due to inefficiency get passed to a configured `ThresholdAccepter` 
instance. *N.B.* it is unlikely that you want to have an `<EfficiencyCurve>` tag 
and a `<RecoThreshold>` tag for the same PDG. However, it is possible that 
particles that failed to be tracked, left some visible energy. 

### TrackedMomentumMatrixSmearer

Documentation to follow. Sorry.

### MetaSimpleSmearcepter

Documentation to follow. Sorry.
