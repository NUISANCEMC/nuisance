#ifndef STACK_BASE_H
#define STACK_BASE_H

#include "FitLogger.h"
#include "GeneralUtils.h"
#include "MeasurementVariableBox.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TH3.h"
#include "THStack.h"

#include "PlotUtils.h"

class StackBase {
public:
  StackBase(){};
  ~StackBase(){};

  virtual void AddMode(std::string name, std::string title, int linecolor = 1,
                       int linewidth = 1, int fillstyle = 1001);
  virtual void AddMode(int index, std::string name, std::string title,
                       int linecolor = 1, int linewidth = 1,
                       int fillstyle = 1001);

  virtual bool IncludeInStack(TH1 *hist);
  virtual bool IncludeInStack(int index);

  virtual void SetupStack(TH1 *hist);
  virtual void Scale(double sf, std::string opt = "");
  virtual void FluxUnfold(TH1D *flux, TH1D *events, double scalefactor,
                          int nevents);
  virtual void Reset();
  virtual void FillStack(int index, double x, double y = 1.0, double z = 1.0,
                         double weight = 1.0);
  virtual void SetBinContentStack(int index, int binx, int biny, int binz,
                             double content);
  virtual void SetBinErrorStack(int index, int binx, int biny, int binz,
                           double error);
  virtual void SetTitleX(std::string title){fXTitle = title;};
  virtual void SetTitleY(std::string title){fYTitle = title;};
  virtual void SetTitleZ(std::string title){fZTitle = title;};

  virtual void Write();

  virtual void Add(StackBase *hist, double scale);
  virtual void Add(TH1 *hist, double scale);

  virtual void AddNewHist(std::string name, TH1 *hist);
  virtual void AddToCategory(std::string name, TH1 *hist);
  virtual void AddToCategory(int index, TH1 *hist);

  virtual void Divide(TH1 *hist);
  virtual void Multiply(TH1 *hist);
  virtual TH1 *GetHist(int entry);
  virtual TH1 *GetHist(std::string label);
  virtual THStack GetStack();

  std::string GetType() { return fType; };

  std::string fName;
  std::string fTitle;
  std::string fXTitle;
  std::string fYTitle;
  std::string fZTitle;
  std::string fType;

  TH1 *fTemplate;
  int fNDim;

  // Maps incase we want to be selective
  std::vector<std::vector<int> > fAllStyles;
  std::vector<std::string> fAllTitles;
  std::vector<std::string> fAllLabels;
  std::vector<TH1 *> fAllHists;
};

/*
class NuSpeciesStack : public StackBase {
public:
        SetupStack(TH1* hist) {
                AddMode("numu", "numu", kBlue, 2, 3004);
                AddMode("numubar", "numubar", kRed, 2, 3004 );
                AddMode("nue", "nue",   kGreen, 2, 3004 );
                StackBase::SetupStack(hist);
        };

        void NuSpeciesStack::FillStack(int species, double x, double y = 1.0,
double z = 1.0, double weight = 1.0) {
                Stackbase::FillStack(ConvertSpeciesToIndex(species), x, y, z,
weight);
        }

        int ConvertSpeciesToIndex(int species) {
                switch (species) {
                case 14: return 0;
                case -14: return 1;
                case 11: return 2;
                default: return -1;
                }
        };
};

class TargetStack : public StackBase {
public:
        SetupStack(TH1* hist) {
                AddMode("C", "C", kBlue, 2, 3004);
                AddMode("H", "H", kRed, 2, 3004 );
                AddMode("O", "O",   kGreen, 2, 3004 );
                StackBase::SetupStack(hist);
        };

        void NuSpeciesStack::FillStack(int species, double x,
                                       double y = 1.0, double z = 1.0,
                                       double weight = 1.0) {
                Stackbase::FillStack(ConvertTargetToIndex(species), x, y, z,
weight);
        }

        int ConvertTargetToIndex(int target) {
                switch (species) {
                case 1000010010: return 0;
                case 1000: return 1;
                case 1000: return 2;
                default: return -1;
                }
        };
}

*/

#endif
