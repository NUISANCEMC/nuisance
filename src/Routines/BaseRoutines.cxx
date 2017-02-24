#include "BaseRoutines.h"


void BaseRoutines::ParseArgument(std::vector<std::string> args, std::string opt, int& val){
	for (size_t i = 0; i < args.size()-1; i++){
		if (!(args[i]).compare(opt.c_str())){
			val = GeneralUtils::StrToInt(args[i+1]);

			// Remove from vector
			args.erase(args.begin() + i);
			args.erase(args.begin() + i);
		}
	}
}

void BaseRoutines::ParseArgument(std::vector<std::string> args, std::string opt, std::string& val){
	for (size_t i = 0; i < args.size()-1; i++){
		if (!(args[i]).compare(opt.c_str())){
			val = (args[i+1]);

			// Remove from vector
			args.erase(args.begin() + i);
			args.erase(args.begin() + i);
		}
	}
}


void BaseRoutines::ParseRemainingXML(std::vector<std::string> args, std::vector<std::string>& cmds){

	// Make one big string again and parse it by ''
	std::string temp = "";
	for (size_t i = 0; i < args.size(); i++){
		temp += args[i];
	}

	// Printout
	std::cout << "Remaining XML = " << temp << std::endl;

	return;
}

