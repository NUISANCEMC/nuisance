// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace FitBase;
#pragma link C++ function FitBase::GetRWConvFunction;
#pragma link C++ function FitBase::GetRWUnits;
#pragma link C++ function FitBase::RWSigmaToFrac;
#pragma link C++ function FitBase::RWSigmaToAbs;
#pragma link C++ function FitBase::RWAbsToSigma;
#pragma link C++ function FitBase::RWFracToSigma;

#pragma link C++ class FitParticle+;
#pragma link C++ class FitEventBase+;
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
