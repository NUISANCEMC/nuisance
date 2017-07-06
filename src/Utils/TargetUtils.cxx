#include "TargetUtils.h"

std::vector<int> TargetUtils::ParseTargetsToIntVect(std::string targets) {

	std::vector<std::string> splittgt = GeneralUtils::ParseToStr(targets, ",");
	std::vector<int> convtgt;

	for (size_t i = 0; i < splittgt.size(); i++) {
		std::string type = splittgt[i];
		convtgt.push_back( GetTargetPDGFromString( type ) );
	}

	return convtgt;
}


int TargetUtils::GetTargetPDGFromString(std::string target){

	if      (!target.compare("H")) return 1000010010;
	else if (!target.compare("C")) return 1000060120;
	else if (!target.compare("O")) return 1000080160;
	else {
		int conv = GeneralUtils::StrToInt(target);
		if (abs(conv) > 1) return conv;
	}
	return 0;
}

int TargetUtils::GetTargetPDGFromZA(int Z, int A){
	return 1000000000 + A*10 + Z*10000;
}

int TargetUtils::GetTargetAFromPDG(int PDG){
  return ((PDG%10000))/10;
}

int TargetUtils::GetTargetZFromPDG(int PDG){
  return (PDG%1000000000 - PDG%10000)/10000;
}
