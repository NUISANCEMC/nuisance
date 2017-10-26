#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "GeneralUtils.h"

/// All functions used to parse input arguments.
namespace ParserUtils {

/// Checks a list of arguments for '-?' flags that have not been removed
void CheckBadArguments(std::vector<std::string> args);

/// Parses the arguments looked for the 'opt' flag and saves it into val
void ParseArgument(std::vector<std::string>& args, std::string opt,
                   int& val, bool required = false, bool duplicates = true);

/// Parses the arguments looked for the 'opt' flag and saves it into val
void ParseArgument(std::vector<std::string>& args, std::string opt,
                   std::string& val, bool required = false, bool duplicates = true);

/// Parses the arguments looked for the 'opt' flag and saves it into val
void ParseSplitArgument(std::vector<std::string>& args, std::string opt,
                        std::string& val, bool required = false, bool duplicates = true);

/// Parses the arguments looked for the 'opt' flag and saves all proceeding values into val (UNSTABLE)
void ParseSplitArgument(std::vector<std::string>& args, std::string opt,
                        std::vector<std::string>& val, bool required = false, bool duplicates = true);

// /// Take everything left in the string
// void ParseRemainingXML(std::vector<std::string>& args, std::vector<std::string>& cmds);

/// Parse arguments looking for '+/-opt' and add/subtract from counter when it occurs
void ParseCounter(std::vector<std::string>& args, std::string opt, int& count);

/// In the case where duplicates can be given, parses all cases into the val vector
void ParseArgument(std::vector<std::string>& args, std::string opt,
                   std::vector<std::string>& val, bool required = false, bool duplicates = true);
};

#endif
