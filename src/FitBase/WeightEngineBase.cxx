#include "WeightEngineBase.h"
double WeightEngineBase::GetDialValue(int rwenum) {
	std::map<int, double>::iterator val =
	    fEnumCurValues.find(rwenum);

	if (val != fEnumCurValues.end()) {
		return (*val).second;
	} else {
		ERR(FTL) << "Cannot find enum in " << fName << std::endl;
		throw;
	}
};