#include "utility/StringUtility.hxx"

#include "string_parsers/utility.hxx"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace nuis {
namespace utility {

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
} // namespace utility
} // namespace nuis
