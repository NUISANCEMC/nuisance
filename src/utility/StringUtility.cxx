#include "exception/exception.hxx"

#include "utility/StringUtility.hxx"

#include "string_parsers/from_string.hxx"
#include "string_parsers/utility.hxx"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace nuis {
namespace utility {

NEW_NUIS_EXCEPT(unexpected_empty_string);

std::string EnsureTrailingSlash(std::string str) {
  if (!str.size()) {
    throw unexpected_empty_string();
  }
  if (str.back() != '/') {
    return str + '/';
  }
  return str;
}

std::string parseCode(std::regex_constants::error_type etype) {
  switch (etype) {
  case std::regex_constants::error_collate:
    return "error_collate: invalid collating element request";
  case std::regex_constants::error_ctype:
    return "error_ctype: invalid character class";
  case std::regex_constants::error_escape:
    return "error_escape: invalid escape character or trailing escape";
  case std::regex_constants::error_backref:
    return "error_backref: invalid back reference";
  case std::regex_constants::error_brack:
    return "error_brack: mismatched bracket([ or ])";
  case std::regex_constants::error_paren:
    return "error_paren: mismatched parentheses(( or ))";
  case std::regex_constants::error_brace:
    return "error_brace: mismatched brace({ or })";
  case std::regex_constants::error_badbrace:
    return "error_badbrace: invalid range inside a { }";
  case std::regex_constants::error_range:
    return "erro_range: invalid character range(e.g., [z-a])";
  case std::regex_constants::error_space:
    return "error_space: insufficient memory to handle this regular expression";
  case std::regex_constants::error_badrepeat:
    return "error_badrepeat: a repetition character (*, ?, +, or {) was not "
           "preceded by a valid regular expression";
  case std::regex_constants::error_complexity:
    return "error_complexity: the requested match is too complex";
  case std::regex_constants::error_stack:
    return "error_stack: insufficient memory to evaluate a match";
  default:
    return "";
  }
}

std::string DeGlobPattern(std::string const &pattern) {
  std::stringstream ss("");
  size_t next_asterisk = pattern.find_first_of('*');
  size_t next_to_add = 0;
  bool modified = false;
  while (next_asterisk != std::string::npos) {
    if ((pattern[next_asterisk - 1] !=
         ']') && // Try to allow valid uses of an asterisk without a preceding.
        (pattern[next_asterisk - 1] != '.')) {
      modified = true;
      // Add a .
      ss << pattern.substr(next_to_add, next_asterisk - next_to_add) << ".*";
      next_to_add = next_asterisk + 1;
      if (next_to_add >= pattern.size()) {
        next_to_add = std::string::npos;
      }
    }
    next_asterisk = pattern.find_first_of('*', next_asterisk + 1);
  }

  if (next_to_add != std::string::npos) {
    ss << pattern.substr(next_to_add);
  }

  if (modified) {
    std::cout << "[INFO]: DeGlobified input pattern: " << pattern
              << " to std::regex_friendly: " << ss.str() << std::endl;
  }

  return ss.str();
}

// #define DEBUG_INDENT_APPLY_WIDTH

std::string indent_apply_width(std::string input, size_t indent, size_t width) {
  std::stringstream output_stream;
  std::string indent_string(indent, ' ');

  if (width) {
    width--;
  }

  if (width <= indent) {
    return input;
  }

#ifdef DEBUG_INDENT_APPLY_WIDTH
  std::cout << "[IDEBUG]: Indenting and forcing width on: "
            << std::quoted(input) << ", width = " << width
            << ", indent = " << indent << std::endl;
#endif

  while (input.length()) {
#ifdef DEBUG_INDENT_APPLY_WIDTH
    std::cout << "[DEBUG]\tRemaining string: " << std::quoted(input)
              << ", width = " << width << ", indent = " << indent << std::endl;
#endif
    if (input.length() < width - indent) {
#ifdef DEBUG_INDENT_APPLY_WIDTH
      std::cout << "[DEBUG]\tRemaining string is less than allowed width "
                << std::quoted(input) << std::endl;
#endif
      output_stream << indent_string << input << std::endl;
      break;
    }
    size_t last_ws = input.find_last_of(" ", width - indent);
    size_t first_nl = input.find_first_of("\n");
#ifdef DEBUG_INDENT_APPLY_WIDTH
    std::cout << "[DEBUG]: last_ws = " << last_ws << ", first_nl = " << first_nl
              << std::endl;
#endif
    size_t next_split = std::min(last_ws, first_nl);

#ifdef DEBUG_INDENT_APPLY_WIDTH
    std::cout << "[DEBUG]: Found split at " << next_split << std::endl;
#endif
    if (next_split == std::string::npos) {
      next_split = std::min(width - indent, input.length());
    }
    std::string next_line = input.substr(0, next_split);
    fhicl::string_parsers::trim(next_line);
#ifdef DEBUG_INDENT_APPLY_WIDTH
    std::cout << "[DEBUG]: Streaming " << std::quoted(next_line) << std::endl;
#endif
    output_stream << indent_string << next_line << std::endl;
    input = input.substr(next_split + 1);
  }

  return output_stream.str();
}

std::vector<std::string> split(std::string const &str,
                               std::string const &delim) {
  return fhicl::string_parsers::ParseToVect<std::string>(str, delim, false,
                                                         true);
}

std::vector<std::string> split(std::string const &str,
                               std::vector<std::string> const &delims) {
  return fhicl::string_parsers::ParseToVect<std::string>(str, delims, false,
                                                         true);
}

std::string str_replace(std::string const &inp, std::string const &from,
                        std::string const &to) {
  std::stringstream ss("");

  size_t nextOccurence = 0;
  size_t prevOccurence = 0;
  bool AtEnd = false;
  while (!AtEnd) {
    nextOccurence = inp.find(from, prevOccurence);
    if (nextOccurence == std::string::npos) {
      if (prevOccurence == inp.length()) {
        break;
      }
      AtEnd = true;
    }
    if ((nextOccurence != prevOccurence) || (nextOccurence == 0)) {
      ss << inp.substr(prevOccurence, (nextOccurence - prevOccurence));
      if (!AtEnd) {
        ss << to;
      }
    }
    prevOccurence = nextOccurence + from.size();
  }
  return ss.str();
}

} // namespace utility
} // namespace nuis
