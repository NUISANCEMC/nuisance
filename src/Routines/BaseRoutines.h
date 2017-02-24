#ifndef BASE_ROUTINES_H
#define BASE_ROUTINES_H
#include "FitParameters.h"
#include "GeneralUtils.h"
#include <vector>
class BaseRoutines {
public:
	BaseRoutines();
	~BaseRoutines();

	void ParseArgument(std::vector<std::string> args, std::string opt, int& val);
	void ParseArgument(std::vector<std::string> args, std::string opt, std::string& val);
	void ParseRemainingXML(std::vector<std::string> args, std::vector<std::string>& cmds);


};



#endif