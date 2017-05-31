#ifndef CUSTOM_VARIABLES_BOX_H
#define CUSTOM_VARIABLES_BOX_H

#include "MeasurementVariableBox.h"
#include "MeasurementVariableBox1D.h"
#include "MeasurementVariableBox2D.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

/// Custom box used to also save Q2 for each event.
class Q2VariableBox1D : public MeasurementVariableBox1D {
public:
	inline Q2VariableBox1D() { Reset(); };
	inline void Reset() {	fQ2 = -999.9; }
	double fQ2;
};

#endif
