#include "ParserUtils.h"

void ParserUtils::CheckBadArguments(std::vector<std::string> args) {
	// Any argument that has not already been erased is considered bad.
	for (size_t i = 0; i < args.size(); i++) {
		if (args[i][0] == '-') {
		 QTHROW("Unknown command line option given : \'" << args[i] << "\'");
		}
	}
}


void ParserUtils::ParseArgument(std::vector<std::string>& args, std::string opt,
                                int& val, bool required, bool duplicates) {

	int indexfound = -1;
	for (size_t i = 0; i < args.size() - 1; i++) {
		if (args.empty()) break;

		if (!(args[i]).compare(opt.c_str())) {

			// Found it
			if (indexfound != -1 and !duplicates) {
			 QTHROW("Duplicate arguments given with : " << opt << std::endl
				      << "1 -> " << opt << " " << val << std::endl
				      << "2 -> " << opt << " " << args[i + 1]);
			}

			// Get Value
			val = GeneralUtils::StrToInt(args[i + 1]);
			indexfound = i;
		}
	}

	// if not found
	if (indexfound == -1 and required) {
	 QTHROW("No flag " << opt << " found in commands."
		      << "This is required!");
	}

	// Remove from vector
	if (indexfound != -1) {
		args.erase(args.begin() + indexfound);
		args.erase(args.begin() + indexfound);
	}
}

void ParserUtils::ParseArgument(std::vector<std::string>& args, std::string opt,
                                std::string& val, bool required, bool duplicates) {

	int indexfound = -1;
	for (size_t i = 0; i < args.size() - 1; i++) {
		if (args.empty()) break;

		if (!(args[i]).compare(opt.c_str())) {

			// Found it
			if (indexfound != -1 and !duplicates) {
			 QTHROW("Duplicate arguments given with : " << opt << std::endl
				      << "1 -> " << opt << " " << val << std::endl
				      << "2 -> " << opt << " " << args[i + 1]);
			}

			// Get Value
			val = (args[i + 1]);
			indexfound = i;
		}
	}

	// if not found
	if (indexfound == -1 and required) {
	 QTHROW("No flag " << opt << " found in commands."
		      << "This is required!");
	}

	// Remove from vector
	if (indexfound != -1) {
		args.erase(args.begin() + indexfound);
		args.erase(args.begin() + indexfound);
	}
}

void ParserUtils::ParseArgument(std::vector<std::string>& args, std::string opt,
                                std::vector<std::string>& val, bool required, bool duplicates) {

	while (std::find(args.begin(), args.end(), opt) != args.end()) {
		std::string temp = "";
		ParseArgument(args, opt, temp, required);
		val.push_back(temp);
	}

	if (required and val.empty()) {
	 QTHROW("No flag " << opt << " found in commands."
		      << "This is required!");
	}
}



void ParserUtils::ParseSplitArgument(std::vector<std::string>& args, std::string opt,
                                     std::string& val, bool required, bool duplicates) {

	int indexfound = -1;
	std::string splitline = "";
	int linecount = 0;

	for (size_t i = 0; i < args.size(); i++) {
		if (args.empty()) break;


		// Start saving
		if (!(args[i]).compare(opt.c_str())) {
			indexfound = i;
			splitline = "";
			continue;
		}

		if (indexfound != -1) {
			if (args[i][0] == '-') {
				break;
			}

			splitline += args[i] + " ";
			linecount++;
		}
	}

	// Remove from vector
	if (indexfound != -1) {
		for (int i = indexfound; i <= indexfound + linecount; i++) {
			args.erase(args.begin() + i);
		}
		val = splitline;
	}
}


void ParserUtils::ParseSplitArgument(std::vector<std::string>& args, std::string opt,
                                     std::vector<std::string>& val, bool required, bool duplicates) {

	//std::cout << "Starting split argument" << std::endl;
	while (std::find(args.begin(), args.end(), opt) != args.end()) {
		std::string temp = "";
		ParseArgument(args, opt, temp, required, duplicates);

		val.push_back(temp);
	}

	if (required and val.empty()) {
	 QTHROW( "No flag " << opt << " found in commands."
		       << "This is required!");
		throw;
	}


}



// void ParserUtils::ParseRemainingXML(std::vector<std::string>& args, std::vector<std::string>& cmds) {

// 	// Make one big string again and parse it by ''
// 	std::string temp = "";
// 	for (size_t i = 0; i < args.size(); i++) {
// 		temp += args[i] + " ";
// 	}

// 	return;
// }

void ParserUtils::ParseCounter(std::vector<std::string>& args, std::string opt, int& count) {

	std::vector<int> indexlist;

	for (size_t i = 0; i < args.size(); i++) {
		if (!(args[i]).compare( "+" + opt)) {
			count++;
			indexlist.push_back(i);
		} else if (!(args[i]).compare( "-" + opt)) {
			count--;
			indexlist.push_back(i);
		}
	}

	for (size_t i = 0; i < indexlist.size(); i++) {
		args.erase(args.begin() + indexlist[i]);
	}
}


