#include "BeamUtils.h"

std::vector<int> BeamUtils::ParseSpeciesToIntVect(std::string spc){

	std::vector<std::string> splitspc = GeneralUtils::ParseToStr(spc,",");
	std::vector<int> convspc;

	for (size_t i = 0; i < splitspc.size(); i++){

		std::string type = splitspc[i];

		if      (!type.compare("electron")) convspc.push_back(11);
		else if (!type.compare("positron")) convspc.push_back(-11);
		else if (!type.compare("muon")) convspc.push_back(13);
		else if (!type.compare("antimuon")) convspc.push_back(-13);

		else if (!type.compare("nue")) convspc.push_back(12);
		else if (!type.compare("numu")) convspc.push_back(14);
		else if (!type.compare("antinue")) convspc.push_back(12);
		else if (!type.compare("antinumu")) convspc.push_back(14);
		else{
			convspc.push_back( GeneralUtils::StrToInt(type) );
		}
	}

	return convspc;
}