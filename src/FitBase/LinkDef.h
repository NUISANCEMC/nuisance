#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ namespace FitBase;
#pragma link C++ function FitBase::GetRWConvFunction;
#pragma link C++ function FitBase::GetRWUnits;
#pragma link C++ function FitBase::RWSigmaToFrac;
#pragma link C++ function FitBase::RWSigmaToAbs;
#pragma link C++ function FitBase::RWAbsToSigma;
#pragma link C++ function FitBase::RWFracToSigma;

#pragma link C++ class FitParticle+;
#pragma link C++ class vector<FitParticle>+;
#pragma link C++ class BaseFitEvt+;
#pragma link C++ class FitEvent+;

#pragma link C++ class FitWeight+;

#pragma link C++ class FitSplineHead+;
#pragma link C++ class FitSpline+;

#pragma link C++ class InputHandler+;

#pragma link C++ class MeasurementBase+;
#pragma link C++ class Measurement1D+;
#pragma link C++ class Measurement2D+;
#pragma link C++ class JointMeas1D+;

#endif
