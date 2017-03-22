#include "SplineUtils.h"


// std::vector<int> SplineUtils::GetSplitDialPositions(FitWeight* rw, std::string names) {
// 	std::vector<std::string> splitnames = GeneralUtils::ParseToStr(names, ";");
// 	std::vector<int> splitpos;

// 	for (size_t i = 0; i < splitnames.size(); i++) {
// 		int pos = fRW->GetDialPos(splitnames[i]);
// 		splitpos.push_back(pos);
// 	}
	
// 	return splitpos;
// }

std::vector< std::vector<double> > SplineUtils::GetSplitDialPoints(std::string points) {

	// Determine Type
	std::vector<std::string> splittype = GeneralUtils::ParseToStr(points, ":");
	std::string deftype = "";
	if (splittype.size() == 1) {
		deftype = "GRID";
	} else if (splittype.size() > 1) {
		deftype = splittype[0];
		splittype.erase(splittype.begin());
	} else {
		throw;
	}

	// Make Grid
	std::vector<std::vector<double> > gridpoints;

	// Possible Point Types
	if (!deftype.compare("GRID")) {

		std::vector<std::string> splitpoints = GeneralUtils::ParseToStr(splittype[0], ";");

		// CBA writing an ND iterator to build this grids. 1D, 2D, and 3D are below..
		if (splitpoints.size() == 1) {
			std::vector<double> tempcont = std::vector<double>(1, 0.0);
			std::vector<double> tempsplit = GeneralUtils::ParseToDbl(splitpoints[0], ",");

			for (size_t i = 0; i < tempsplit.size(); i++) {
				tempcont[0] = tempsplit[i];
				gridpoints.push_back(tempcont);
			}

			// 2D
		} else if (splitpoints.size() == 2) {

			std::vector<double> tempcont = std::vector<double>(2, 0.0);
			std::vector<double> tempsplit1 = GeneralUtils::ParseToDbl(splitpoints[0],",");
			std::vector<double> tempsplit2 = GeneralUtils::ParseToDbl(splitpoints[1],",");

			for (size_t i = 0; i < tempsplit1.size(); i++) {
				for (size_t j = 0; j < tempsplit2.size(); j++) {
					tempcont[0] = tempsplit1[i];
					tempcont[1] = tempsplit2[j];
					gridpoints.push_back(tempcont);
				}
			}
			// 3D
		} else if (splitpoints.size() == 3) {

			std::vector<double> tempcont = std::vector<double>(3, 0.0);
			std::vector<double> tempsplit1 = GeneralUtils::ParseToDbl(splitpoints[0],",");
			std::vector<double> tempsplit2 = GeneralUtils::ParseToDbl(splitpoints[1],",");
			std::vector<double> tempsplit3 = GeneralUtils::ParseToDbl(splitpoints[2],",");

			for (size_t i = 0; i < tempsplit1.size(); i++) {
				for (size_t j = 0; j < tempsplit2.size(); j++) {
					for (size_t k = 0; k < tempsplit3.size(); k++) {
						tempcont[0] = tempsplit1[i];
						tempcont[1] = tempsplit2[j];
						tempcont[2] = tempsplit3[j];
						gridpoints.push_back(tempcont);
					}
				}
			}
		}
	} else if (!deftype.compare("DIAG2D")) {
		std::vector<std::string> splitpoints = GeneralUtils::ParseToStr(splittype[0], ";");
		
		std::vector<double> tempcont = std::vector<double>(2, 0.0);
		std::vector<double> tempsplit1 = GeneralUtils::ParseToDbl(splitpoints[0],",");

		for (size_t i = 0; i < tempsplit1.size(); i++) {
			tempcont[0] = tempsplit1[i];
			tempcont[1] = 0.0;
			gridpoints.push_back(tempcont);
		}

		for (size_t i = 0; i < tempsplit1.size(); i++) {
			tempcont[0] = 0.0;
			tempcont[1] = tempsplit1[i];
			gridpoints.push_back(tempcont);
		}
		for (size_t i = 0; i < tempsplit1.size(); i++) {
			tempcont[0] = tempsplit1[i];
			tempcont[1] = tempsplit1[i];
			gridpoints.push_back(tempcont);
		}

		for (size_t i = 0; i < tempsplit1.size(); i++) {
			tempcont[0] = tempsplit1[i];
			tempcont[1] = tempsplit1[ tempsplit1.size() - 1 - i];
			gridpoints.push_back(tempcont);
		}
	}
	return gridpoints;
}