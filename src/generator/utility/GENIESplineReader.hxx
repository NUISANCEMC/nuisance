#pragma once

#include "TGraph.h"

#include <fstream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace nuis {
namespace genietools {

class SSAX {
  bool EventOnAllTags;
  bool LeavingAlready;
  std::vector<std::string> InterestingTags;

  size_t buf_size;
  size_t buf_usage;
  char *buf;
  std::vector<std::pair<const char *, const char *>> attrs;

  size_t depth;

public:
  SSAX();

  virtual ~SSAX();

  virtual void OnStartDocument();
  virtual void OnEndDocument();
  virtual void OnStartElement(
      const char *name,
      std::vector<std::pair<const char *, const char *>> &attributes);
  virtual void OnCharacters(const char *characters);
  virtual void OnEndElement(const char *name);
  virtual void OnComment(const char *);

  void NotifyInterestingTags(std::vector<std::string> it);
  void NotifyEarlyExit();

private:
  void ExpandBuf();

  void AddToBuf(char character);
  void NullTermBuf();

  bool IsNameStartChar(char c);
  bool IsNameChar(char c);

  void IgnoreAllToRightBracket(std::ifstream &ifs);

  char IgnoreSpace(std::ifstream &ifs);

  char BufNameToBoundary(std::ifstream &ifs);

  char BufAttrValueToMatchingQuote(std::ifstream &ifs, char quot);

  std::pair<char const *, char const *> HandleAttribute(std::ifstream &ifs);

  void HandleOpenTag(std::ifstream &ifs);
  void HandleCloseTag(std::ifstream &ifs);
  void HandleCommentTag(std::ifstream &ifs);

  void FlushBufferToCharacterHandler();

  void ClearBuf();

public:
  void ParseFile(const char *filename);
};

struct spline {
  std::vector<double> EValues;
  std::vector<double> XSecValues;
  std::string SplineName;
};

class GENIESplineGetter : public SSAX {
public:
  std::vector<std::string> SplinePatternStrings;
  std::vector<std::regex> SplinePatterns;
  std::vector<std::vector<spline>> Splines;

  bool InEElement;
  bool InxsecElement;
  bool InSplineElement;

  bool ReadingDocument;

  size_t sp_it;
  size_t NSplines;

  GENIESplineGetter(std::vector<std::string> sn);

  void OnStartDocument();
  void OnEndDocument();
  void OnStartElement(
      const char *name,
      std::vector<std::pair<const char *, const char *>> &attributes);
  void OnCharacters(const char *characters);
  void OnEndElement(const char *name);
  void OnComment(const char *);

  std::vector<std::vector<TGraph>> GetTGraphs();
};

} // namespace genietools
} // namespace nuis
