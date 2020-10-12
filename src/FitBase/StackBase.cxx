#include "StackBase.h"

void StackBase::AddMode(std::string name, std::string title, int linecolor,
                        int linewidth, int fillstyle) {

  // int ncur = fAllLabels.size();
  fAllLabels.push_back(name);
  fAllTitles.push_back(title);

  std::vector<int> temp;
  temp.push_back(linecolor);
  temp.push_back(linewidth);
  temp.push_back(fillstyle);

  fAllStyles.push_back(temp);
}

void StackBase::FluxUnfold(TH1D *flux, TH1D *events, double scalefactor,
                           int nevents) {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    if (fNDim == 1) {
      PlotUtils::FluxUnfoldedScaling((TH1D *)fAllHists[i], flux, events,
                                     scalefactor, nevents);
    } else if (fNDim == 2) {
      PlotUtils::FluxUnfoldedScaling((TH2D *)fAllHists[i], flux, events,
                                     scalefactor);
    }
  }
}

void StackBase::AddMode(int index, std::string name, std::string title,
                        int linecolor, int linewidth, int fillstyle) {

  while (fAllLabels.size() <= (UInt_t)index) {
    fAllLabels.push_back("");
    fAllTitles.push_back("");
    fAllStyles.push_back(std::vector<int>(1, 1));
  }

  fAllLabels[index] = (name);
  fAllTitles[index] = (title);

  std::vector<int> temp;
  temp.push_back(linecolor);
  temp.push_back(linewidth);
  temp.push_back(fillstyle);

  fAllStyles[index] = temp;
}

bool StackBase::IncludeInStack(TH1 *hist) {
  if (!FitPar::Config().GetParB("includeemptystackhists") and
      hist->Integral() == 0.0)
    return false;
  return true;
}

bool StackBase::IncludeInStack(int index) { return true; }

void StackBase::SetupStack(TH1 *hist) {
  fTemplate = (TH1 *)hist->Clone(fName.c_str());
  fTemplate->Reset();

  // Determine template dim
  fNDim = fTemplate->GetDimension();

  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists.push_back(
        (TH1 *)fTemplate->Clone((fName + "_" + fAllLabels[i]).c_str()));
  }
};

void StackBase::Scale(double sf, std::string opt) {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    // std::cout << "Scaling Stack Hist " << i << " by " << sf << std::endl;
    fAllHists[i]->Scale(sf, opt.c_str());
  }
};

void StackBase::Reset() {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists[i]->Reset();
  }
};

void StackBase::FillStack(int index, double x, double y, double z,
                          double weight) {
  if (index < 0 or (UInt_t) index >= fAllLabels.size()) {
    NUIS_ERR(WRN, "Returning Stack Fill Because Range = " << index << " "
                                                          << fAllLabels.size());
    return;
  }

  if (fNDim == 1)
    fAllHists[index]->Fill(x, y);
  else if (fNDim == 2) {
    // std::cout << "Filling 2D Stack " << index << " " << x << " " << y << " "
    // << z << std::endl;
    ((TH2 *)fAllHists[index])->Fill(x, y, z);
  }

  else if (fNDim == 3)
    ((TH3 *)fAllHists[index])->Fill(x, y, z, weight);
}

void StackBase::SetBinContentStack(int index, int binx, int biny, int binz,
                              double content) {
  if (index < 0 or (UInt_t) index >= fAllLabels.size()) {
    NUIS_ERR(WRN, "Returning Stack Fill Because Range = " << index << " "
                                                          << fAllLabels.size());
    return;
  }

  if (fNDim == 1) {
    fAllHists[index]->SetBinContent(binx, content);
  } else if (fNDim == 2) {
    ((TH2 *)fAllHists[index])->SetBinContent(binx, biny, content);
  } else if (fNDim == 3) {
    ((TH3 *)fAllHists[index])->SetBinContent(binx, biny, binz, content);
  }
}

void StackBase::SetBinErrorStack(int index, int binx, int biny, int binz,
                            double error) {
  if (index < 0 or (UInt_t) index >= fAllLabels.size()) {
    NUIS_ERR(WRN, "Returning Stack Fill Because Range = " << index << " "
                                                          << fAllLabels.size());
    return;
  }

  if (fNDim == 1) {
    fAllHists[index]->SetBinError(binx, error);
  } else if (fNDim == 2) {
    ((TH2 *)fAllHists[index])->SetBinError(binx, biny, error);
  } else if (fNDim == 3) {
    ((TH3 *)fAllHists[index])->SetBinError(binx, biny, binz, error);
  }
}

void StackBase::Write() {
  THStack *st = new THStack();

  // Loop and add all histograms
  bool saveseperate = FitPar::Config().GetParB("WriteSeperateStacks");
  for (size_t i = 0; i < fAllLabels.size(); i++) {

    if (!IncludeInStack(fAllHists[i]))
      continue;
    if (!IncludeInStack(i))
      continue;

    fAllHists[i]->SetTitle(fAllTitles[i].c_str());
    fAllHists[i]->GetXaxis()->SetTitle(fXTitle.c_str());
    fAllHists[i]->GetYaxis()->SetTitle(fYTitle.c_str());
    fAllHists[i]->GetZaxis()->SetTitle(fZTitle.c_str());
    fAllHists[i]->SetLineColor(fAllStyles[i][0]);
    fAllHists[i]->SetLineWidth(fAllStyles[i][1]);
    fAllHists[i]->SetFillStyle(fAllStyles[i][2]);
    fAllHists[i]->SetFillColor(fAllStyles[i][0]);
    if (saveseperate)
      fAllHists[i]->Write();

    st->Add(fAllHists[i]);
  }
  st->SetTitle(fTitle.c_str());
  st->SetName(fName.c_str());
  st->Write();
  delete st;
};

void StackBase::Multiply(TH1 *hist) {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists[i]->Multiply(hist);
  }
}

void StackBase::Divide(TH1 *hist) {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists[i]->Divide(hist);
  }
}

void StackBase::Add(TH1 *hist, double scale) {
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists[i]->Add(hist, scale);
  }
}

void StackBase::Add(StackBase *hist, double scale) {

  if (hist->GetType() != fType) {
    NUIS_ERR(WRN, "Trying to add two StackBases of different types!");
    NUIS_ERR(WRN, fType << " + " << hist->GetType() << " = Undefined.");
    NUIS_ERR(WRN, "Doing nothing...");
    return;
  }

  for (size_t i = 0; i < fAllLabels.size(); i++) {
    fAllHists[i]->Add(hist->GetHist(i));
  }
}

TH1 *StackBase::GetHist(int entry) { return fAllHists[entry]; }

TH1 *StackBase::GetHist(std::string label) {

  TH1 *hist = NULL;
  std::vector<std::string> splitlabels = GeneralUtils::ParseToStr(label, "+");
  for (size_t j = 0; j < splitlabels.size(); j++) {
    std::string newlabel = splitlabels[j];

    for (size_t i = 0; i < fAllLabels.size(); i++) {
      if (newlabel == fAllLabels[i]) {
        if (!hist)
          hist = (TH1 *)fAllHists[i]->Clone();
        else
          hist->Add(fAllHists[i]);
      }
    }
  }

  return hist;
}

THStack StackBase::GetStack() {
  THStack st = THStack();
  for (size_t i = 0; i < fAllLabels.size(); i++) {
    st.Add(fAllHists[i]);
  }
  return st;
}

void StackBase::AddNewHist(std::string name, TH1 *hist) {
  AddMode(fAllLabels.size(), name, hist->GetTitle(), hist->GetLineColor());
  fAllHists.push_back((TH1 *)hist->Clone());
}

void StackBase::AddToCategory(std::string name, TH1 *hist) {

  for (size_t i = 0; i < fAllLabels.size(); i++) {
    if (name == fAllLabels[i]) {
      fAllHists[i]->Add(hist);
      break;
    }
  }
}

void StackBase::AddToCategory(int index, TH1 *hist) {
  fAllHists[index]->Add(hist);
}
