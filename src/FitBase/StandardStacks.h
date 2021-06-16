#ifndef STANDARD_STACKS_H
#define STANDARD_STACKS_H

#include "BaseFitEvt.h"
#include "FitEvent.h"
#include "StackBase.h"

/// Single stack class, for consistent handling of TH1* and StackBase*
/// histograms.
class FakeStack : public StackBase {
public:
  /// Sets Template to exact pointer to hist without cloning.
  FakeStack(TH1 *hist);

  /// Sets NULL template and saves TF1 object instead
  FakeStack(TF1 *f);

  /// Sets NULL template and saves TGraph object instead
  FakeStack(TGraph *gr);

  /// Unlinks pointer to original histogram
  ~FakeStack();

  /// Fills the normal fTemplate histogram
  void Fill(double x, double y = 1.0, double z = 1.0, double weight = 1.0);

  /// Scales the normal fTemplate histogram
  void Scale(double norm, std::string opt);

  /// Resets the normal fTemplate histogram
  void Reset();

  /// Writes the normal fTemplate histogram
  void Write();

  std::string fFakeType;
  TGraph *fTGraphObject;
  TF1 *fTF1Object;
};

/// True Mode stack, an array of neut true interaction channels
class TrueModeStack : public StackBase {
public:
  /// Main constructor listing true mode categories.
  TrueModeStack(std::string name, std::string title, TH1 *hist);

  /// List to convert Modes to Index.
  /// Should be kept in sync with constructor.
  static int ConvertModeToIndex(int mode);

  /// Fill fromgiven mode integer
  void Fill(int mode, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);

  void SetBinContent(int mode, int binx, int biny, int binz, double content);
  void SetBinError(int mode, int binx, int biny, int binz, double error);

  /// Extracts Mode from FitEvent and fills
  void Fill(FitEvent *evt, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);

  /// Extracts Mode from BaseFitEvt
  void Fill(BaseFitEvt *evt, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);
};

/// True Mode NuNuBar stack, array of true channels split by nu/nubar
class NuNuBarTrueModeStack : public StackBase {
public:
  /// Main constructor listing true mode categories.
  NuNuBarTrueModeStack(std::string name, std::string title, TH1 *hist);

  /// List to convert Modes to Index.
  /// Should be kept in sync with constructor.
  int ConvertModeToIndex(int mode);

  /// Fill fromgiven mode integer
  void Fill(int species, int mode, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);
};

/// Species stack to look at contributions from multiple beam leptons
class BeamSpeciesStack : public StackBase {
public:
  /// main constructor listing beam categories
  BeamSpeciesStack(std::string name, std::string title, TH1 *hist);

  /// Fills stack using neutrino species
  void Fill(int species, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);

  /// List converts PDG Beam to index.
  /// Should be kept in sync with constructor.
  int ConvertSpeciesToIndex(int species);
};

/// Species stack to look at contributions from multiple beam leptons
class TargetTypeStack : public StackBase {
public:
  /// main constructor listing beam categories
  TargetTypeStack(std::string name, std::string title, TH1 *hist);

  /// Fills stack using target pdg
  void Fill(int pdg, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);

  /// List converts PDG Beam to index.
  /// Should be kept in sync with constructor.
  int ConvertPDGToIndex(int pdg);

  /// Specie empty histograms to not be included in final stack
  bool IncludeInStack(TH1 *hist);
};

/// CC Topology Stack, categories defined by final state particle counts.
class CCTopologyStack : public StackBase {
public:
  /// main constructor listing beam categories
  CCTopologyStack(std::string name, std::string title, TH1 *hist);

  /// Fills stack using FitEvent
  void Fill(FitEvent *evt, double x, double y = 1.0, double z = 1.0,
            double weight = 1.0);

  /// Extracts index from evt particle counts
  int GetIndexFromEventParticles(FitEvent *evt);
};

#endif
