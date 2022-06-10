#ifndef GIBUUNATIVEINPUTHANDLER_H
#define GIBUUNATIVEINPUTHANDLER_H
#ifdef GiBUU_ENABLED
/*!
 *  \addtogroup InputHandler
 *  @{
 */
#include "InputHandler.h"
#include "PlotUtils.h"

struct GiBUUEventReader {
public:
  
  // Event weight
  double weight;

  // The full particle stack
  TBranch *b_pdg = 0;
  TBranch *b_Px = 0;
  TBranch *b_Py = 0;
  TBranch *b_Pz = 0;
  TBranch *b_E = 0;

  // These are only if you build with the right options
  TBranch *b_x = 0;
  TBranch *b_y = 0;
  TBranch *b_z = 0;

  std::vector<int> *pdg = 0;
  std::vector<double> *Px = 0;
  std::vector<double> *Py = 0;
  std::vector<double> *Pz = 0;
  std::vector<double> *E = 0;

  std::vector<double> *x = 0;
  std::vector<double> *y = 0;
  std::vector<double> *z = 0;

  int mode;
  int process_ID;
  int flavor_ID;
  int num_ensembles;
  int num_runs;
  int nucleus_A;
  int nucleus_Z;
  
  double lepIn_E;
  double lepIn_Px;
  double lepIn_Py;
  double lepIn_Pz;
  
  double lepOut_E;
  double lepOut_Px;
  double lepOut_Py;
  double lepOut_Pz;

  double nuc_E;
  double nuc_Px;
  double nuc_Py;
  double nuc_Pz;
  int nuc_chrg;

  bool SetBranchAddresses(TChain* tn);
};

int GetGiBUUNuPDG(int flavor_ID, int process_ID);

int GetGiBUULepPDG(int flavor_ID, int process_ID);

int CheckGiBUUParticleStatus(double E, int pdg, double dist, int targetA);

int ConvertModeGiBUUtoNEUT(int gibuu_mode, int process_ID, int struck_nucleon_pdg, int first_part_pdg);


/// GiBUU Handler to read in GiBUU's ROOT format
class GiBUUNativeInputHandler : public InputHandlerBase {
public:

	/// Standard constructor given name and inputs
	GiBUUNativeInputHandler(std::string const& handle, std::string const& rawinputs);

	/// Actually need to clean up some GiBUU-specific things
	~GiBUUNativeInputHandler();

	/// Returns NUISANCE Format Event from the GiBUU stack
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

	/// Override to handle the fact that GiBUU already averages over nevents.
	void SetupJointInputs();

	/// Special info is required for GiBUU...
	void RegisterJointInput(std::string input, int process_ID, int flavor_ID, 
				int nnucleons, int n, int nrequested, TH1D *f, TH1D *e);

	/// Fill NUISANCE Particle Stack
	void CalcNUISANCEKinematics();

	/// Print event information
	void Print();

private:
	GiBUUEventReader* fGiReader;
	TChain* fGiBUUTree;           ///< GiBUU Event Tree

	// For GiBUU specific normalization
	std::vector<int> jointrequested;
	std::vector<int> jointtype;
	std::vector<int> jointnnucl;
};
#endif
#endif
