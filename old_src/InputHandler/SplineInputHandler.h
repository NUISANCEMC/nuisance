#ifndef SPLINE_INPUTHANDLER_H
#define SPLINE_INPUTHANDLER_H

#include "InputHandler.h"
#include "FitEvent.h"
#include "PlotUtils.h"

/// Spline InputHandler. Almost functionally identical to FitEventInputHandler
/// with an extension to handle the spline co-efficients.
class SplineInputHandler : public InputHandlerBase {
public:

	/// Standard constructor with name and file input
	SplineInputHandler(std::string const& handle, std::string const& rawinputs);
	virtual ~SplineInputHandler();

	/// Returns NUISANCE FitEvent with Spline Coefficients included.
	/// If lightweight, only spline coefficients are updated.
	FitEvent* GetNuisanceEvent(const UInt_t entry, const bool lightweight = false);

		/// Create a TTree Cache to speed up file read
	void CreateCache();

	/// Remove TTree Cache to save memory
	void RemoveCache();

	/// Return extra input weighting
	double GetInputWeight(int entry);

	/// Prints Event/Spline Information
	void Print();

	TChain* fFitEventTree;   ///< Main Fit Event Tree
	TTree* fSplTree;	     ///< Main Spline Coefficient Tree
	SplineReader* fSplRead;  ///< Spline Reader Object used to interpret splines
	float fSplineCoeff[1000];///< Coefficients. Currently a hardcoded limit of 1000.

	/// Starting RW Input Weights corresponding to nominal spline weight
	std::vector<float> fStartingWeights;
};
#endif
