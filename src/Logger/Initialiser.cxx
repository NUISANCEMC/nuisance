#include "Initialiser.h"

void RunNuisance(){
  std::cout << "Starting NUISANCE" << std::endl;
}

struct LetterBackronym {
  LetterBackronym(size_t n, std::string const &b, float p = 1.0,
                  std::string const &t = "") {
    NUsed = n;
    Backkie = b;
    ProbAccept = p;
    TagLine = t;
  };
  size_t NUsed;
  float ProbAccept;
  std::string Backkie;
  std::string TagLine;
};

__attribute__((constructor)) void nuisance_init(void) {
  std::vector<std::vector<LetterBackronym> > Letters;

  for (size_t i = 0; i < 8; ++i) {
    Letters.push_back(std::vector<LetterBackronym>());
  }

  Letters[0].push_back(LetterBackronym(2, "Neutrino"));
  Letters[0].push_back(LetterBackronym(3, "NUIsance", 0.2));

  Letters[2].push_back(LetterBackronym(1, "Interaction"));

  Letters[3].push_back(LetterBackronym(1, "Systematics"));
  Letters[3].push_back(LetterBackronym(
                         1, "Synthesiser", 0.2, "Playing on the comparisons you want to see"));

  Letters[4].push_back(LetterBackronym(2, "ANalyser"));
  Letters[4].push_back(LetterBackronym(1, "Aggregating", 0.5));
  Letters[4].push_back(LetterBackronym(3, "from A-Neutrino sCattering", 1,
                                       "You can always find a frame"));

  Letters[5].push_back(
    LetterBackronym(1, "New", 1, "The freshest comparisons"));

  Letters[6].push_back(LetterBackronym(1, "by Comparing"));
  Letters[6].push_back(LetterBackronym(1, "Constraints from"));

  Letters[7].push_back(LetterBackronym(1, "Experiments"));

  std::vector<std::string> TagLines;
  TagLines.push_back("Fit and compare.");

  std::stringstream back("");

  TRandom3 tr;
  tr.SetSeed();

  for (size_t i = 0; i < 8;) {
    LetterBackronym const &let = Letters[i][tr.Integer(Letters[i].size())];
    if (tr.Uniform() > let.ProbAccept) {
      continue;
    }
    back << let.Backkie << " ";
    i += let.NUsed;
    if (let.TagLine.length()) {
      TagLines.push_back(let.TagLine);
    }
  }

  std::string Name = "Nuisance";
  std::string TagL = TagLines[tr.Integer(TagLines.size())];

  std::vector<std::pair<std::string, std::pair<std::string, std::string> > >
  OneBlob;

  OneBlob.push_back(
    std::make_pair("NUISANCE", std::make_pair("", "FiXing your Neutrinos")));

  if (tr.Uniform() < 0.01) {
    std::pair<std::string, std::pair<std::string, std::string> > const &blob =
      OneBlob[tr.Integer(OneBlob.size())];
    Name = blob.first;
    back.str("");
    back << blob.second.first;
    TagL = blob.second.second;
  }

  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
            "%%%%%%%%%%%%%%%"
            "%%"
            << std::endl
            << "%%  Welcome to " << Name << ": \033[5m" << back.str()
            << "\033[0m-- " << TagL << std::endl
            << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
            "%%%%%%%%%%%%%%%"
            "%%"
            << std::endl;
}