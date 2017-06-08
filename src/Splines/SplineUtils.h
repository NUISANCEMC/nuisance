#ifndef SPLINE_UTILS_H
#define SPLINE_UTILS_H

//#include "FitWeight.h"
#include "GeneralUtils.h"

namespace SplineUtils {
	//std::vector<int> GetSplitDialPositions(FitWeight* rw, std::string names);
	std::vector< std::vector<double> > GetSplitDialPoints(std::string points);
};

#endif
