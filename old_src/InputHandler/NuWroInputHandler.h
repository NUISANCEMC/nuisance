#ifndef NuWroINPUTHANDLER_H
#define NuWroINPUTHANDLER_H
#ifdef __NUWRO_ENABLED__
#include "GeneratorUtils.h"
#include "InputHandler.h"
#include "PlotUtils.h"

/// NuWro Generator Container to save extra particle status codes.
class NuWroGeneratorInfo : public GeneratorInfoBase {
 public:
  NuWroGeneratorInfo(){};
  virtual ~NuWroGeneratorInfo();

  /// Assigns information to branches
  void AddBranchesToTree(TTree* tn);

  /// Setup reading information from branches
  void SetBranchesFromTree(TTree* tn);

  /// Allocate any dynamic arrays for a new particle stack size
  void AllocateParticleStack(int stacksize);

  /// Clear any dynamic arrays
  void DeallocateParticleStack();

  /// Read extra genie information from the event
  void FillGeneratorInfo(event* e);

  /// Reset extra information to default/empty values
  void Reset();

  int kMaxParticles;        ///< Number of particles in stack
  int* fNuWroParticlePDGs;  ///< NuWro Particle PDGs (example)
};

/// Main NuWro Input Reader. Requires events have flux and xsec TH1Ds saved into
/// them.
class NuWroInputHandler : public InputHandlerBase {
#ifdef __USE_NUWRO_SRW_EVENTS__
  params fNuwroParams;
  std::vector<BaseFitEvt> rwEvs;
#endif

 public:
  /// Constructor. Can handle single and joint inputs.
  NuWroInputHandler(std::string const& handle, std::string const& rawinputs);
  ~NuWroInputHandler();

  /// Create a TTree Cache to speed up file read
  void CreateCache();

  /// Remove TTree Cache to save memory
  void RemoveCache();

  /// Returns filled NUISANCEEvent for given entry.
  FitEvent* GetNuisanceEvent(const UInt_t entry,
                             const bool lightweight = false);

#ifdef __USE_NUWRO_SRW_EVENTS__
  // Returns filled BaseFitEvent for a given entry;
  BaseFitEvt* GetBaseEvent(const UInt_t entry) {
    if (rwEvs.size() <= entry) {
      THROW("Tried to get cached BaseFitEv[" << entry << "], but only have "
                                             << rwEvs.size()
                                             << " in the cache.");
    }
    return &rwEvs[entry];
  }
#endif

  /// Fills fNUISANCEEvent from fNuWroEvent
  void CalcNUISANCEKinematics();

  /// (LEGACY) Automatically creates nuwro flux/event histograms that
  /// nuisance needs to normalise events.
  void ProcessNuWroInputFlux(const std::string file);

  /// Calculates a True Interaction code for NuWro events
  int ConvertNuwroMode(event* e);

  /// Adds a new particle to NUISANCE stack for given NuWro particle
  void AddNuWroParticle(FitEvent* evt, particle& p, int state);

  event* fNuWroEvent;  ///< Pointer to NuWro Format Events

  /// Print Event Information
  void Print();

  TChain* fNuWroTree;  ///< TTree for reading NuWro event vectors
  bool fSaveExtra;     ///< Save Extra NuWro info into Nuisance Event
  NuWroGeneratorInfo* fNuWroInfo;  ///< Extra NuWro Generator Info
};
/*! @} */
#endif
#endif
