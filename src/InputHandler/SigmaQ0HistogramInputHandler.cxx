#include "SigmaQ0HistogramInputHandler.h"


SigmaQ0HistogramInputHandler::SigmaQ0HistogramInputHandler(std::string const& handle, std::string const& rawinputs) {
	LOG(SAM) << "Creating SigmaQ0HistogramInputHandler : " << handle << std::endl;

	// Run a joint input handling
	fName = handle;

	// Assign to hist event format
	fEventType = kHISTEVENT;


	// Parse our input file
	// SIGMAQ0HIST:path.txt,ENERGY=0.25,THETA=0.34,Q0COL=1,XSCOL=2,SCL=10
	std::vector<std::string> parsedinputs = GeneralUtils::ParseToStr(rawinputs, ",");
	fFilePath = parsedinputs[0];

	// Setup Defaults incase none given
	fEnergy = -1.0;
	fTheta = -1.0;
	fQ0Column = 0;
	fSigmaColumn = 1;
	fBeamPDG = -1; // Assume electron by default
	fDelim = " ";
	fScaling = 1.E38;

	// Now Loop through and parse possible inputs
	for (uint i = 1; i < parsedinputs.size(); i++) {

		std::vector<std::string> parsedspec = GeneralUtils::ParseToStr(parsedinputs[i], "=");
		if (parsedspec.size() < 2) {
			THROW("NO VALUE GIVEN TO SPECIFIER : " << parsedinputs[0]);
		}

		std::string spec = parsedspec[0];
		std::string value = parsedspec[1];

		// Read Energy
		if (!spec.compare("ENERGY") or
		        !spec.compare("E")) {
			fEnergy = GeneralUtils::StrToDbl(value);

			// Read Theta
		} else if (!spec.compare("THETA") or
		           !spec.compare("T")) {
			fTheta = GeneralUtils::StrToDbl(value);

			// Set Q0 Column
		} else if (!spec.compare("Q0") or
		           !spec.compare("Q0COL")) {
			fQ0Column = GeneralUtils::StrToInt(value);

			// Set XSec Column
		} else if (!spec.compare("XSCOL") or
		           !spec.compare("XS") or
		           !spec.compare("X") or
		           !spec.compare("SIG") or
		           !spec.compare("SIGMA")) {
			fSigmaColumn = GeneralUtils::StrToInt(value);

		} else if (!spec.compare("BEAM") or
		           !spec.compare("P") or
		           !spec.compare("PDG")) {
			fBeamPDG = GeneralUtils::StrToInt(value);

		} else if (!spec.compare("SCL") or
		           !spec.compare("SC") or
		           !spec.compare("S")) {
			fScaling = GeneralUtils::StrToDbl(value);

			// Throw
		} else {
			THROW("Unknown argument given to SigmaQ0 InputHandler!" );
		}
	}

	// Check we got everything
	if (fEnergy == -1 or fEnergy < 0) {
		THROW("Energy for SigmaQ0 Handler either invalid or not given! Specify with ',E=ENERGY' in the input spec.");
	}

	// Default to Electron
	if (fBeamPDG == -1) {
		ERR(WRN) << "No Beam PDG Given in SigmaQ0 Handler so assuming electron. " <<
		         "If input is not electron scattering please specify using : PDG=PDGCODE" << std::endl;
	}


	// Convert Theta
	fTheta = fTheta * M_PI / 180.0;
	LOG(FIT) << "Set Theta to = " << fTheta << std::endl;

	// Have to create a dummy flux and event rate histogram :(
	fFluxHist = new TH1D("fluxhist", "fluxhist", 100.0, 0.0, 2.0 * fEnergy);
	fFluxHist->SetBinContent( fFluxHist->FindBin(fEnergy), 1.0 );

	fEventHist = new TH1D("eventhist", "eventhist", 100.0, 0.0, 2.0 * fEnergy);
	fEventHist->SetBinContent( fEventHist->FindBin(fEnergy), 1.0 );

	fEnergy = fEnergy * 1.E3;
	LOG(FIT) << "Set E energy" << std::endl;

	// Now parse the lines in our input file.
	fApplyInterpolation = FitPar::Config().GetParB("InterpolateSigmaQ0Histogram");
	double interpolation_res = FitPar::Config().GetParD("InterpolateSigmaQ0HistogramRes");
	fMaxValue = 0.0;
	fMaxX = 0.0;
	fMinX = 1.E10;

	// Create a TGraph of Points
	TGraph* gr = new TGraph();
	std::vector<std::string> inputlines = GeneralUtils::ParseFileToStr(fFilePath, "\n");
	for (uint i = 0; i < inputlines.size(); i++) {
	  std::vector<double> splitline = GeneralUtils::ParseToDbl(inputlines[i], fDelim.c_str());
	  double q0 = splitline[fQ0Column];
	  double sig = splitline[fSigmaColumn];
	  gr->SetPoint(gr->GetN(), q0, sig);
	  if (sig > fMaxValue) fMaxValue = sig;
	  if (q0 > fMaxX) fMaxX = q0;
	  if (q0 < fMinX) fMinX = q0;
	}

	fInputGraph = new TGraph();
	for (int i = 0; i < gr->GetN(); i++){
	  fInputGraph->SetPoint(fInputGraph->GetN(), gr->GetX()[i], gr->GetY()[i]);
	  fNEvents++;

	  if (fApplyInterpolation){
	    // If not last point create extra events
	    if (i != gr->GetN()-1){
	      double xlow  = gr->GetX()[i];
	      double xhigh = gr->GetX()[i+1];
	      for (int j = 1; j < int(interpolation_res); j++){
		double x = xlow + double(j) * (xhigh - xlow) / interpolation_res;
		fInputGraph->SetPoint(fInputGraph->GetN(),x, gr->Eval(x));
		fNEvents++;
	      }
	    }
	  }
	}

	delete gr;

	  
        fFluxHist->Scale(1.0 / fFluxHist->Integral("width") );
        fEventHist->Scale( fScaling * double(fNEvents) / fEventHist->Integral() );
	if (fApplyInterpolation){
	  fEventHist->Scale(1.0 / interpolation_res);
	}
	
	fUseAcceptReject = FitPar::Config().GetParB("InterpolateSigmaQ0HistogramThrow");
	if (fUseAcceptReject){
	  std::cout << "USING ACCEPT REJECT" << std::endl;
	  fEventHist->Scale( fMaxValue / double(fNEvents) );
	  fNEvents = FitPar::Config().GetParI("InterpolateSigmaQ0HistogramNTHROWS");
	  std::cout << "NEvents = " << fNEvents << std::endl;
	  sleep(1);
	}

	fNUISANCEEvent = new FitEvent();
	

};

SigmaQ0HistogramInputHandler::~SigmaQ0HistogramInputHandler() {
};

FitEvent* SigmaQ0HistogramInputHandler::GetNuisanceEvent(const UInt_t entry, const bool lightweight) {

	// Catch too large entries
	if (entry >= (UInt_t)fNEvents) return NULL;

	// Evaluate Graph to Create an Event
	if (!fUseAcceptReject){
	  double q0 = fInputGraph->GetX()[entry];
	  double sig = fInputGraph->GetY()[entry];
	  FillNuisanceEvent(q0, sig);
	} else {
	  double q0 = ThrowQ0();
	  FillNuisanceEvent(q0, 1.0);
	}

	// Return event pointer
	return fNUISANCEEvent;
}

double SigmaQ0HistogramInputHandler::ThrowQ0(){

  // Use input graph to Throw Q0.
  int count = 0;
  while (count < 1E7){
    double x = fRandom.Uniform(fMinX,fMaxX);
    if (fRandom.Uniform(0.0,1.0) <= fInputGraph->Eval(x) / fMaxValue){ return x; };
    std::cout << "THROW " << count << " : " << x << std::endl;
  }
  return 0.0;
}
    
void SigmaQ0HistogramInputHandler::FillNuisanceEvent(double q0, double sig) {
  
  // Reset all variables
  
  fNUISANCEEvent->ResetEvent();
  
  // Fill Globals
  fNUISANCEEvent->fMode    = 1; // Assume CCQE for now...
  fNUISANCEEvent->Mode     = 1;

  fNUISANCEEvent->fEventNo = 0;
	fNUISANCEEvent->fTargetA = 0;  // Should the User Specify these?
	fNUISANCEEvent->fTargetZ = 0;  // Should the User Specify these?
	fNUISANCEEvent->fTargetH = 0; // Should the User Specify these?
	fNUISANCEEvent->fBound   = 1; // Should the User Specify these?

	fNUISANCEEvent->InputWeight = sig;

	// Add incoming beam particle along Z with energy E and outgoing with theta
	if (fBeamPDG == 11) {

		// Initial Beam
		fNUISANCEEvent->fParticleState[0] = kInitialState;
		fNUISANCEEvent->fParticlePDG[0] = 11;
		double mass = 0.511;

		// Get Momentum of Electron moving along Z
		fNUISANCEEvent->fParticleMom[0][0] = 0.0;
		fNUISANCEEvent->fParticleMom[0][1] = 0.0;
		LOG(FIT) << "Setting Energy = " << fEnergy << std::endl;
		fNUISANCEEvent->fParticleMom[0][2] = sqrt(fEnergy * fEnergy - mass * mass);
		fNUISANCEEvent->fParticleMom[0][3] = fEnergy;

		// Outgoing particle
		fNUISANCEEvent->fParticleState[1] = kFinalState;
		fNUISANCEEvent->fParticlePDG[1] = 11;

		// Get Momentum of Electron outgoing minus q0
		double oute = fEnergy - q0;
		double outp = sqrt(oute * oute - mass * mass);
		fNUISANCEEvent->fParticleMom[1][0] = 0.0;
		fNUISANCEEvent->fParticleMom[1][1] = sin(fTheta) * outp;
		fNUISANCEEvent->fParticleMom[1][2] = cos(fTheta) * outp;
		fNUISANCEEvent->fParticleMom[1][3] = oute;

	}

	// Update Particles
	fNUISANCEEvent->fNParticles = 2;

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();

	// Check Q0
	//	FitParticle* ein  = fNUISANCEEvent->PartInfo(0);
	//	FitParticle* eout = fNUISANCEEvent->GetHMFSParticle(11);

	//	double newq0    = fabs(ein->fP.E() - eout->fP.E()) / 1000.0;
	//	double E     = ein->fP.E() / 1000.0;
	//	double theta = ein->fP.Vect().Angle(eout->fP.Vect()) * 180. / M_PI;

	//	LOG(FIT) << "Extracted event from line: theirs-" << q0/1.E3 << " ours-" << newq0 << " E-" << fEnergy << " T-" << theta << " X-" << sig << std::endl;

	return;
}

