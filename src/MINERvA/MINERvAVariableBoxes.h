#ifndef MINERvA_VARIABLES_BOX_H
#define MINERvA_VARIABLES_BOX_H

#include "MeasurementVariableBox.h"
#include "MeasurementVariableBox1D.h"
#include "MeasurementVariableBox2D.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

/// Custom box used to also save All Pion Tpi for each event.
class NTpiVariableBox1D : public MeasurementVariableBox1D {
public:
	inline NTpiVariableBox1D() { Reset(); };
	inline void Reset() {  fTpiVect.clear(); } 
	std::vector<double> fTpiVect;
};

/// Custom box used to also save All Pion Tpi for each event.
class NthpiVariableBox1D : public MeasurementVariableBox1D {
 public:
  inline NthpiVariableBox1D() { Reset(); };
  inline void Reset() {  fthpiVect.clear(); }
  std::vector<double> fthpiVect;
};


#endif
