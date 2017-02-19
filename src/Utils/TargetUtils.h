#ifndef TARGET_UTILS_H
#define TARGET_UTILS_H

#include "FitLogger.h"
#include "GeneralUtils.h"

namespace TargetUtils {

	std::vector<int> ParseTargetsToIntVect(std::string targets);

	int GetTargetPDGFromString(std::string target);
	int GetTargetPDGFromZA(int Z, int A);
	int GetTargetZFromPDG(int PDG);
	int GetTargetAFromPDG(int PDG);
}


#endif