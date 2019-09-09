#include "TargetUtils.h"

std::vector<int> TargetUtils::ParseTargetsToIntVect(std::string targets) {

  std::vector<std::string> splittgt = GeneralUtils::ParseToStr(targets, ",");
  std::vector<int> convtgt;

  for (size_t i = 0; i < splittgt.size(); i++) {
    std::string type = splittgt[i];
    convtgt.push_back(GetTargetPDGFromString(type));
  }

  return convtgt;
}

int TargetUtils::GetTargetPDGFromString(std::string target) {

  if (!target.compare("H"))
    return 1000010010;
  else if (!target.compare("C"))
    return 1000060120;
  else if (!target.compare("O"))
    return 1000080160;
  else {
    int conv = GeneralUtils::StrToInt(target);
    if (abs(conv) > 1)
      return conv;
  }
  return 0;
}

int TargetUtils::GetTargetPDGFromZA(int Z, int A) {
  return 1000000000 + A * 10 + Z * 10000;
}

int TargetUtils::GetTargetAFromPDG(int PDG) { return ((PDG % 10000)) / 10; }

int TargetUtils::GetTargetZFromPDG(int PDG) {
  return (PDG % 1000000000 - PDG % 10000) / 10000;
}

///____________________________________________________________________________
void TargetUtils::ListTargetIDs() {

  // Keep in sync with ConvertTargetIDs
  QLOG(FIT, "Possible Target IDs: \n"
                << "\n H  : " << TargetUtils::ConvertTargetIDs("H")
                << "\n C  : " << TargetUtils::ConvertTargetIDs("C")
                << "\n CH  : " << TargetUtils::ConvertTargetIDs("CH")
                << "\n CH2 : " << TargetUtils::ConvertTargetIDs("CH2")
                << "\n H2O : " << TargetUtils::ConvertTargetIDs("H2O")
                << "\n Fe  : " << TargetUtils::ConvertTargetIDs("Fe")
                << "\n Pb  : " << TargetUtils::ConvertTargetIDs("Pb")
                << "\n D2  : " << TargetUtils::ConvertTargetIDs("D2")
                << "\n D2-free : " << TargetUtils::ConvertTargetIDs("D2-free"));
}

//____________________________________________________________________________
std::string TargetUtils::ConvertTargetIDs(std::string id) {

  if (!id.compare("H"))
    return "1000010010";
  else if (!id.compare("C"))
    return "1000060120";
  else if (!id.compare("CH"))
    return "13,1000060120[0.9231],1000010010[0.0769]";
  else if (!id.compare("CH2"))
    return "14,1000060120[0.8571],1000010010[0.1429]";
  else if (!id.compare("H2O"))
    return "18,1000080160[0.8888],1000010010[0.1111]";
  else if (!id.compare("Fe"))
    return "1000260560";
  else if (!id.compare("Pb"))
    return "1000822070";
  else if (!id.compare("D2"))
    return "1000010020";
  else if (!id.compare("D2-free"))
    return "2,1000010010[0.5],1000000010[0.5]";
  else
    return "";
};
