#include "GENIESplineReader.hxx"

#include "exception/exception.hxx"

#include "utility/StringUtility.hxx"

#include "string_parsers/from_string.hxx"
#include "string_parsers/utility.hxx"

#include <iostream>

#include <cctype>
#include <cstring>

// #define SPLINE_NAME_MATCHER_VERBOSE

namespace nuis {
namespace genietools {

SSAX::SSAX()
    : EventOnAllTags(true), LeavingAlready(false), InterestingTags(),
      buf_size(1), buf_usage(0), buf(NULL), depth(0) {
  buf = new char[buf_size];
};

SSAX::~SSAX() { delete buf; }

void SSAX::OnStartDocument() {}
void SSAX::OnEndDocument() {}
void SSAX::OnStartElement(
    const char *name,
    std::vector<std::pair<const char *, const char *>> &attributes) {
  std::cout << "[SSAX]: OnStartElement(\"" << name << "\", " << std::endl;
  for (size_t a_it = 0; a_it < attributes.size(); ++a_it) {
    std::cout << "\t\"" << attributes[a_it].first << "\" = \""
              << attributes[a_it].second << "\"" << std::endl;
  }
  std::cout << "\t)" << std::endl;
}
void SSAX::OnCharacters(const char *characters) {
  std::cout << "[SSAX]: OnCharacters(\"" << characters << "\")" << std::endl;
}
void SSAX::OnEndElement(const char *name) {
  std::cout << "[SSAX]: OnEndElement(\"" << name << "\")" << std::endl;
}
void SSAX::OnComment(const char *) {}

void SSAX::NotifyInterestingTags(std::vector<std::string> it) {
  InterestingTags = it;
  EventOnAllTags = !InterestingTags.size();
}
void SSAX::NotifyEarlyExit() { LeavingAlready = true; }

void SSAX::ExpandBuf() {
  size_t new_buf_size = (buf_size <= 1E6) ? buf_size * 10 : (buf_size + 1E6);

  std::cout << "[SSAX]: Expanding internal buffer to: " << new_buf_size
            << " bytes." << std::endl;

  char *new_buf = new char[new_buf_size];
  memcpy(new_buf, buf, buf_size);
  delete[] buf;

  buf = new_buf;
  buf_size = new_buf_size;
}

void SSAX::AddToBuf(char character) {
  if (buf_usage == buf_size) {
    ExpandBuf();
  }
  buf[buf_usage++] = character;
}

void SSAX::NullTermBuf() { AddToBuf('\0'); }

bool SSAX::IsNameStartChar(char c) {
  return (isalpha(c) || (c == ':') || (c == '_'));
}

bool SSAX::IsNameChar(char c) {
  return (IsNameStartChar(c) || isdigit(c) || (c == '-') || (c == '.'));
}

void SSAX::IgnoreAllToRightBracket(std::ifstream &ifs) {
  char prev, next;
  while (EOF != (next = ifs.get())) {
    if (next == '>') {
      if (prev == '/') { // Empty element
        depth--;
      }

      return;
    }
    prev = next;
  }
  std::cerr << "[SSAX - ERROR]: encountered EOF before ending '>' at depth: "
            << depth << std::endl;
  throw;
}

char SSAX::IgnoreSpace(std::ifstream &ifs) {
  char next;
  while (EOF != (next = ifs.get())) {
    if (isspace(next)) {
      continue;
    } else {
      return next;
    }
  }
  std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
            << std::endl;
  throw;
}

char SSAX::BufNameToBoundary(std::ifstream &ifs) {
  char next;
  while (EOF != (next = ifs.get())) {
    if (isspace(next) || (next == '>') || (next == '/') || (next == '=')) {
      return next;
    }
    if (IsNameChar(next)) {
      AddToBuf(next);
    } else {
      std::cerr << "[SSAX - ERROR]: encountered bad character " << next
                << std::endl;
      throw;
    }
  }
  std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
            << std::endl;
  throw;
}

char SSAX::BufAttrValueToMatchingQuote(std::ifstream &ifs, char quot) {
  char next;
  while (EOF != (next = ifs.get())) {
    if (next == quot) {
      return next;
    }
    AddToBuf(next);
  }
  std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
            << std::endl;
  throw;
}

std::pair<char const *, char const *>
SSAX::HandleAttribute(std::ifstream &ifs) {
  return std::pair<char const *, char const *>(nullptr, nullptr);
}

void SSAX::HandleOpenTag(std::ifstream &ifs) {
  char next = ifs.get();
  size_t AttrNameStart = 0;
  size_t AttrValueStart = 0;
  attrs.clear();
  bool IsInteresting = true;

  if (next == EOF) {
    std::cerr << "[SSAX - ERROR]: encountered EOF directly after element "
                 "opening at depth: "
              << depth << std::endl;
    throw;
  }

  if (!IsNameStartChar(next)) {
    std::cerr << "[SSAX - ERROR]: Bad element name first character: " << next
              << std::endl;
    throw;
  }
  AddToBuf(next);

  next = BufNameToBoundary(ifs);
  NullTermBuf();
  AttrNameStart = buf_usage;

  if (!EventOnAllTags) {
    IsInteresting = false;
    for (size_t it_it = 0; it_it < InterestingTags.size(); ++it_it) {
      if (InterestingTags[it_it] == buf) {
        IsInteresting = true;
        break;
      }
    }
  }

  if ((next == '>')) { // Handles elements like <bla>
    if (IsInteresting) {
      OnStartElement(buf, attrs);
    }
    depth++;
    return;
  }
  if ((next == '/')) { // Handles elements like <bla/>
    next = ifs.get();
    if (EOF == next) {
      std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
                << std::endl;
      throw;
    }

    if ((next == '>')) {
      if (IsInteresting) {
        OnStartElement(buf, attrs);
      }
      return;
    } else {
      std::cerr << "[SSAX - ERROR]: encountered unexpected character " << next
                << " after /. Expected >. Exiting early at depth: " << depth
                << std::endl;
      throw;
    }
  }

  while (EOF != (next = ifs.get())) {
    if (isspace(next)) {
      next = IgnoreSpace(ifs);
    }

    if (IsNameStartChar(next)) {
      AddToBuf(next);
      next = BufNameToBoundary(ifs);
      if (next != '=') {
        std::cerr << "[SSAX - ERROR]: encountered unexpected character \""
                  << next
                  << "\". Expected \"=\". Exiting early at depth: " << depth
                  << std::endl;
        throw;
      }
      NullTermBuf();
      AttrValueStart = buf_usage;

      next = ifs.get();
      if ((next != '\"') && (next != '\'')) {
        std::cerr << "[SSAX - ERROR]: encountered unexpected character \""
                  << next << "\" (" << int(next) << "). Expected \"\'\"("
                  << int('\'') << ") or \"\"\"(" << int('\"')
                  << "). Exiting early at depth: " << depth << std::endl;
        throw;
      }
      next = BufAttrValueToMatchingQuote(ifs, next);
      NullTermBuf();
      attrs.push_back(
          std::make_pair(&buf[AttrNameStart], &buf[AttrValueStart]));
      AttrNameStart = buf_usage;
      continue;
    }

    if ((next == '>')) { // Handles elements like <bla>
      if (IsInteresting) {
        OnStartElement(buf, attrs);
      }
      depth++;
      return;
    }
    if ((next == '/')) { // Handles elements like <bla/>
      next = ifs.get();
      if (EOF == next) {
        std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
                  << std::endl;
        throw;
      }

      if ((next == '>')) {
        if (IsInteresting) {
          OnStartElement(buf, attrs);
        }
        // No depth ++;
        return;
      } else {
        std::cerr << "[SSAX - ERROR]: encountered unexpected character " << next
                  << " after /. Expected >. Exiting early at depth: " << depth
                  << std::endl;
        throw;
      }
    }

    std::cerr << "[SSAX - ERROR]: encountered unexpected character " << next
              << " after /. Expected >. Exiting early at depth: " << depth
              << std::endl;
    throw;
  }
  std::cerr << "[SSAX - ERROR]: encountered EOF early at depth: " << depth
            << std::endl;
  throw;
}

void SSAX::HandleCloseTag(std::ifstream &ifs) {
  ifs.ignore();
  char next = ifs.get();
  bool IsInteresting = true;

  if (!IsNameStartChar(next)) {
    std::cerr << "[SSAX - ERROR]: Bad element name first character: " << next
              << std::endl;
    throw;
  }
  AddToBuf(next);

  next = BufNameToBoundary(ifs);
  NullTermBuf();

  if (!EventOnAllTags) {
    IsInteresting = false;
    for (size_t it_it = 0; it_it < InterestingTags.size(); ++it_it) {
      if (InterestingTags[it_it] == buf) {
        IsInteresting = true;
        break;
      }
    }
  }

  if (IsInteresting) {
    OnEndElement(buf);
  }
}

void SSAX::HandleCommentTag(std::ifstream &ifs) {
  IgnoreAllToRightBracket(ifs);
}

void SSAX::FlushBufferToCharacterHandler() {
  NullTermBuf();
  OnCharacters(buf);
}
void SSAX::ClearBuf() { buf_usage = 0; }

void SSAX::ParseFile(const char *filename) {
  std::ifstream ifs(filename);

  if (!ifs.good()) {
    std::cerr << "[SSAX]: Failed to open " << filename << " for reading."
              << std::endl;
    throw;
  }
  ifs.clear();
  ifs.seekg(0);

  OnStartDocument();

  char next;
  while (EOF != (next = ifs.get())) {
    if (LeavingAlready) {
      return;
    }
    if (next == '<') {
      FlushBufferToCharacterHandler();
      ClearBuf();
      switch (ifs.peek()) {
      case '!':
      case '?': {
        HandleCommentTag(ifs);
        ClearBuf();
        break;
      }
      case '/': {
        HandleCloseTag(ifs);
        ClearBuf();
        break;
      }
      default: {
        HandleOpenTag(ifs);
        ClearBuf();
      }
      }
      continue;
    }
    AddToBuf(next);
  }
  OnEndDocument();
}

GENIESplineGetter::GENIESplineGetter(std::vector<std::string> sps) : SSAX() {
  SplinePatternStrings = sps;
  for (auto const &sp : SplinePatternStrings) {
    SplinePatterns.emplace_back(nuis::utility::DeGlobPattern(sp));
    Splines.push_back({});
  }

  ReadingDocument = false;
  NSplines = 0;

  InSplineElement = false;
  InEElement = false;
  InxsecElement = false;
}

void GENIESplineGetter::OnStartDocument() {
  ReadingDocument = true;
  std::cout << "[GENIESplineGetter]: Begin document..." << std::endl;
}
void GENIESplineGetter::OnEndDocument() {
  ReadingDocument = false;
  std::cout << "[GENIESplineGetter]: Finished document, read " << NSplines
            << " splines." << std::endl;
}

void GENIESplineGetter::OnStartElement(
    const char *name,
    std::vector<std::pair<const char *, const char *>> &attributes) {
  std::string name_str = std::string(name);

  if (InSplineElement) {
    if (name_str == "E") {
      InEElement = true;
    }
    if (name_str == "xsec") {
      InxsecElement = true;
    }
  }

  if (name_str == "spline") {
    for (size_t attr_it = 0; attr_it < attributes.size(); ++attr_it) {
      if (std::string(attributes[attr_it].first) == "name") {
        std::string attr_val = std::string(attributes[attr_it].second);
#ifdef SPLINE_NAME_MATCHER_VERBOSE
        std::cout << "[DEBUG]: Found spline named: " << attr_val << std::endl;
#endif
        for (sp_it = 0; sp_it < SplinePatterns.size(); ++sp_it) {
#ifdef SPLINE_NAME_MATCHER_VERBOSE
          std::cout << "[DEBUG]: Checking against "
                    << SplinePatternStrings[sp_it] << std::endl;
#endif
          if (std::regex_match(attr_val, SplinePatterns[sp_it])) { // match
#ifdef SPLINE_NAME_MATCHER_VERBOSE
            std::cout << "[DEBUG]: Matched! " << std::endl;
#endif
            InSplineElement = true;
            Splines[sp_it].push_back({{}, {}, attr_val});
            break;
          }
        }
        return;
      }
    }
  }
}

void GENIESplineGetter::OnCharacters(const char *characters) {
  if (InEElement) {
    std::string ch(characters);
    fhicl::string_parsers::trim(ch);
    Splines[sp_it].back().EValues.push_back(
        fhicl::string_parsers::str2T<double>(ch));
    InEElement = false;
  }
  if (InxsecElement) {
    std::string ch(characters);
    fhicl::string_parsers::trim(ch);
    Splines[sp_it].back().XSecValues.push_back(
        fhicl::string_parsers::str2T<double>(ch));
    InxsecElement = false;
  }
}

void GENIESplineGetter::OnEndElement(const char *name) {
  if (InSplineElement && (std::string(name) == "spline")) {
    std::cout << "[GENIESplineGetter]: Finished reading spline element: "
              << Splines[sp_it].back().SplineName << std::endl;
    InSplineElement = false;
    NSplines++;
  }
}

void GENIESplineGetter::OnComment(const char *) {}

std::vector<std::vector<TGraph>> GENIESplineGetter::GetTGraphs() {
  std::vector<std::vector<TGraph>> graphs;
  for (sp_it = 0; sp_it < SplinePatterns.size(); ++sp_it) {
    graphs.push_back({});
    for (size_t spl_it = 0; spl_it < Splines[sp_it].size(); ++spl_it) {
      graphs.back().emplace_back(Splines[sp_it][spl_it].EValues.size(),
                                 Splines[sp_it][spl_it].EValues.data(),
                                 Splines[sp_it][spl_it].XSecValues.data());
      graphs.back().back().SetName(Splines[sp_it][spl_it].SplineName.c_str());
    }
  }
  return graphs;
}

} // namespace genietools
} // namespace nuis
