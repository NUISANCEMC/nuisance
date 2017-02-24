#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H
#include "FitParameters.h"
#include "GeneralUtils.h"
#include <vector>
namespace ParserUtils {

void CheckBadArguments(std::vector<std::string> args);

void ParseArgument(std::vector<std::string>& args, std::string opt, 
	int& val, bool required = false, bool duplicates=true);

void ParseArgument(std::vector<std::string>& args, std::string opt, 
	std::string& val, bool required = false, bool duplicates=true);

void ParseSplitArgument(std::vector<std::string>& args, std::string opt, std::string& val, bool required = false, bool duplicates=true);
void ParseSplitArgument(std::vector<std::string>& args, std::string opt, std::vector<std::string>& val, bool required=false, bool duplicates=true);
void ParseRemainingXML(std::vector<std::string>& args, std::vector<std::string>& cmds);
void ParseCounter(std::vector<std::string>& args, std::string opt, int& count);
void ParseArgument(std::vector<std::string>& args, std::string opt, std::vector<std::string>& val, bool required = false, bool duplicates=true);
};



#endif