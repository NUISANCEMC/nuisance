#include "StandardStacks.h"

/// Fake stack functions
FakeStack::FakeStack(TH1 *hist) {
  fTemplate = (TH1 *)hist;
  fNDim = fTemplate->GetDimension();
  fFakeType = "TH1";
  fTGraphObject = NULL;
  fTF1Object = NULL;
}

FakeStack::FakeStack(TF1 *f) {
  fTemplate = NULL;
  fNDim = 1;
  fTF1Object = f;
  fFakeType = "TF1";
  fTGraphObject = NULL;
}

FakeStack::FakeStack(TGraph *gr) {
  fTemplate = NULL;
  fNDim = 1;
  fTGraphObject = gr;
  fFakeType = "TGRAPH";
  fTF1Object = NULL;
}

FakeStack::~FakeStack() {
  fTemplate = NULL;
  fNDim = 0;
  fFakeType = "NULL";
  fTF1Object = NULL;
  fTGraphObject = NULL;
}

void FakeStack::Fill(double x, double y, double z, double weight) {
  if (fTemplate) {
    if (fNDim == 1)
      fTemplate->Fill(x, y);
    else if (fNDim == 2)
      ((TH2 *)fTemplate)->Fill(x, y, z);
    else if (fNDim == 3)
      ((TH3 *)fTemplate)->Fill(x, y, z, weight);
  }
  if (fTGraphObject) {
    fTGraphObject->SetPoint(fTGraphObject->GetN(), x, y);
  }
}

void FakeStack::Scale(double norm, std::string opt) {
  if (fTemplate) {
    fTemplate->Scale(norm, opt.c_str());
  }
  if (fTGraphObject) {
    for (int i = 0; i < fTGraphObject->GetN(); i++) {
      const double *x = fTGraphObject->GetX();
      const double *y = fTGraphObject->GetY();
      fTGraphObject->SetPoint(i, x[i], y[i] * norm);
    }
  }
}

void FakeStack::Reset() {
  if (fTemplate) {
    fTemplate->Reset();
  }
}

void FakeStack::Write() {
  if (fTemplate) {
    fTemplate->Write();
  }
  if (fTF1Object) {
    fTF1Object->Write();
  }
  if (fTGraphObject) {
    fTGraphObject->Write();
  }
}

/// TrueModeStack Functions
TrueModeStack::TrueModeStack(std::string name, std::string title, TH1 *hist)
    : TrueModeStack(name, title) {

  TrueModeStack::AddMode(0, "UNDEFINED", "Undefined", kRed + 1, 2, 3000);

  // CC
  TrueModeStack::AddMode(1, "CCQE", "CCQE", kBlue, 2, 1001);
  TrueModeStack::AddMode(2, "CC2p2h", "2p2h", kRed, 2, 1001);
  TrueModeStack::AddMode(11, "CC1piponp", "CC1#pi^{+} on p", kGreen, 2, 1001);
  TrueModeStack::AddMode(12, "CC1pi0onn", "CC1#pi^{0} on n", kGreen + 3, 2,
                         1001);
  TrueModeStack::AddMode(13, "CC1piponn", "CC1#pi^{+} on n", kGreen - 2, 2,
                         1001);
  TrueModeStack::AddMode(16, "CCcoh", "CC coherent", kBlue, 2, 1001);
  TrueModeStack::AddMode(17, "CC1gamma", "CC1#gamma", kMagenta, 2, 1001);
  TrueModeStack::AddMode(21, "CCMultipi", "Multi #pi (1.3 < W < 2.0)", kYellow,
                         2, 1001);
  TrueModeStack::AddMode(22, "CC1eta", "CC1#eta^{0} on n", kYellow - 2, 2,
                         1001);
  TrueModeStack::AddMode(23, "CC1lamkp", "CC1#Lambda1K^{+}", kYellow - 6, 2,
                         1001);
  TrueModeStack::AddMode(26, "CCDIS", "DIS (W > 2.0)", kRed, 2, 1001);

  // NC
  TrueModeStack::AddMode(31, "NC1pi0onn", "NC1#pi^{0} on n", kBlue, 2, 3004);
  TrueModeStack::AddMode(32, "NC1pi0onp", "NC1#pi^{0} on p", kBlue + 3, 2,
                         3004);
  TrueModeStack::AddMode(33, "NC1pimonn", "NC1#pi^{-} on n", kBlue - 2, 2,
                         3004);
  TrueModeStack::AddMode(34, "NC1piponp", "NC1#pi^{+} on p", kBlue - 8, 2,
                         3004);
  TrueModeStack::AddMode(36, "NCcoh", "NC Coherent", kBlue + 8, 2, 3004);
  TrueModeStack::AddMode(38, "NC1gammaonn", "NC1#gamma on n", kMagenta, 2,
                         3004);
  TrueModeStack::AddMode(39, "NC1gammaonp", "NC1#gamma on p", kMagenta - 10, 2,
                         3004);
  TrueModeStack::AddMode(41, "NCMultipi", "Multi #pi (1.3 < W < 2.0)",
                         kBlue - 10, 2, 3004);
  TrueModeStack::AddMode(42, "NC1etaonn", "NC1#eta^{0} on n", kYellow - 2, 2,
                         3004);
  TrueModeStack::AddMode(43, "NC1etaonp", "NC1#eta^{0} on p", kYellow - 4, 2,
                         3004);
  TrueModeStack::AddMode(44, "NC1kamk0", "NC1#Lambda1K^{0} on n", kYellow - 6,
                         2, 3004);
  TrueModeStack::AddMode(45, "NC1lamkp", "NC1#Lambda1K^{+}", kYellow - 10, 2,
                         3004);
  TrueModeStack::AddMode(46, "NCDIS", "DIS (W > 2.0)", kRed, 2, 3004);
  TrueModeStack::AddMode(51, "NCELonp", "NCEL on p", kBlack, 2, 3004);
  TrueModeStack::AddMode(52, "NCELonn", "NCEL on n", kGray, 2, 3004);

  TrueModeStack::AddMode(53, "NC2p2h", "NC 2p2h", kRed + 1, 2, 3004);

  StackBase::SetupStack(hist);
};

TrueModeStack::TrueModeStack(std::string name, std::string title) {
  fName = name;
  fTitle = title;
}
/// TrueModeStack Functions
TrueModeStack::TrueModeStack(
    std::string name, std::string title, TH1 *hist,
    std::map<int, std::pair<std::string, std::string> > const &process_ids)
    : TrueModeStack(name, title) {

  TrueModeStack::AddMode(0, "UNDEFINED", "Undefined", kRed + 1, 2, 3000);

  for (auto const &pid : process_ids) {
    TrueModeStack::AddMode(pid.first, "process_id_" + std::to_string(pid.first),
                           pid.second.first, kBlack, 1, 1001);
  }

  StackBase::SetupStack(hist);
};

void TrueModeStack::AddMode(int mode, std::string name, std::string title,
                            int linecolor, int linewidth, int fillstyle) {

  auto mode_it = std::find(fmodes.begin(), fmodes.end(), mode);
  if (mode_it != fmodes.end()) {
    NUIS_ABORT(
        "MODE: " << mode
                 << ", added to TrueModeStack twice... this shouldn't happen.");
  }

  StackBase::AddMode(fmodes.size(), name, title, linecolor, linewidth,
                     fillstyle);
  fmodes.push_back(mode);
}

int TrueModeStack::ConvertModeToIndex(int mode) {
  auto mode_it = std::find(fmodes.begin(), fmodes.end(), mode);
  if (mode_it == fmodes.end()) {
    return 0;
  }
  return std::distance(fmodes.begin(), mode_it);
};

void TrueModeStack::Fill(int mode, double x, double y, double z,
                         double weight) {
  StackBase::FillStack(ConvertModeToIndex(mode), x, y, z, weight);
};

void TrueModeStack::SetBinContent(int mode, int binx, int biny, int binz,
                                  double content) {
  StackBase::SetBinContentStack(ConvertModeToIndex(mode), binx, biny, binz,
                                content);
}
void TrueModeStack::SetBinError(int mode, int binx, int biny, int binz,
                                double error) {
  StackBase::SetBinErrorStack(ConvertModeToIndex(mode), binx, biny, binz,
                              error);
}

void TrueModeStack::Fill(FitEvent *evt, double x, double y, double z,
                         double weight) {
  StackBase::FillStack(ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

void TrueModeStack::Fill(BaseFitEvt *evt, double x, double y, double z,
                         double weight) {
  StackBase::FillStack(ConvertModeToIndex(evt->Mode), x, y, z, weight);
};

/// TrueModeStack Functions
NuNuBarTrueModeStack::NuNuBarTrueModeStack(std::string name, std::string title,
                                           TH1 *hist)
    : TrueModeStack(name, title) {

  TrueModeStack::AddMode(0, "UNDEFINED", "Undefined", kRed + 2, 2, 3000);

  // Neutrino
  // CC
  TrueModeStack::AddMode(1, "NU_CCQE", "#nu CCQE", kBlue, 2, 1001);
  TrueModeStack::AddMode(2, "NU_CC2p2h", "#nu 2p2h", kRed, 2, 1001);
  TrueModeStack::AddMode(11, "NU_CC1piponp", "#nu CC1#pi^{+} on p", kGreen, 2,
                         1001);
  TrueModeStack::AddMode(12, "NU_CC1pi0onn", "#nu CC1#pi^{0} on n", kGreen + 3,
                         2, 1001);
  TrueModeStack::AddMode(13, "NU_CC1piponn", "#nu CC1#pi^{+} on n", kGreen - 2,
                         2, 1001);
  TrueModeStack::AddMode(16, "NU_CCcoh", "#nu CC coherent", kBlue, 2, 1001);
  TrueModeStack::AddMode(17, "NU_CC1gamma", "#nu CC1#gamma", kMagenta, 2, 1001);
  TrueModeStack::AddMode(21, "NU_CCMultipi", "#nu Multi #pi (1.3 < W < 2.0)",
                         kYellow, 2, 1001);
  TrueModeStack::AddMode(22, "NU_CC1eta", "#nu CC1#eta^{0} on n", kYellow - 2,
                         2, 1001);
  TrueModeStack::AddMode(23, "NU_CC1lamkp", "#nu CC1#Lambda1K^{+}", kYellow - 6,
                         2, 1001);
  TrueModeStack::AddMode(26, "NU_CCDIS", "#nu DIS (W > 2.0)", kRed, 2, 1001);

  // NC
  TrueModeStack::AddMode(31, "NU_NC1pi0onn", "#nu NC1#pi^{0} on n", kBlue, 2,
                         3004);
  TrueModeStack::AddMode(32, "NU_NC1pi0onp", "#nu NC1#pi^{0} on p", kBlue + 3,
                         2, 3004);
  TrueModeStack::AddMode(33, "NU_NC1pimonn", "#nu NC1#pi^{-} on n", kBlue - 2,
                         2, 3004);
  TrueModeStack::AddMode(34, "NU_NC1piponp", "#nu NC1#pi^{+} on p", kBlue - 8,
                         2, 3004);
  TrueModeStack::AddMode(36, "NU_NCcoh", "#nu NC Coherent", kBlue + 8, 2, 3004);
  TrueModeStack::AddMode(38, "NU_NC1gammaonn", "#nu NC1#gamma on n", kMagenta,
                         2, 3004);
  TrueModeStack::AddMode(39, "NU_NC1gammaonp", "#nu NC1#gamma on p",
                         kMagenta - 10, 2, 3004);
  TrueModeStack::AddMode(41, "NU_NCMultipi", "#nu Multi #pi (1.3 < W < 2.0)",
                         kBlue - 10, 2, 3004);
  TrueModeStack::AddMode(42, "NU_NC1etaonn", "#nu NC1#eta^{0} on n",
                         kYellow - 2, 2, 3004);
  TrueModeStack::AddMode(43, "NU_NC1etaonp", "#nu NC1#eta^{0} on p",
                         kYellow - 4, 2, 3004);
  TrueModeStack::AddMode(44, "NU_NC1kamk0", "#nu NC1#Lambda1K^{0} on n",
                         kYellow - 6, 2, 3004);
  TrueModeStack::AddMode(45, "NU_NC1lamkp", "#nu NC1#Lambda1K^{+}",
                         kYellow - 10, 2, 3004);
  TrueModeStack::AddMode(46, "NU_NCDIS", "#nu DIS (W > 2.0)", kRed, 2, 3004);
  TrueModeStack::AddMode(51, "NU_NCELonp", "#nu NCEL on p", kBlack, 2, 3004);
  TrueModeStack::AddMode(52, "NU_NCELonn", "#nu NCEL on n", kGray, 2, 3004);

  // CC
  TrueModeStack::AddMode(-1, "ANTINU_CCQE", "#bar{#nu} CCQE", kBlue, 2, 1001);
  TrueModeStack::AddMode(-2, "ANTINU_CC2p2h", "#bar{#nu} 2p2h", kRed, 2, 1001);
  TrueModeStack::AddMode(-11, "ANTINU_CC1piponp", "#bar{#nu} CC1#pi^{+} on p",
                         kGreen, 2, 1001);
  TrueModeStack::AddMode(-12, "ANTINU_CC1pi0onn", "#bar{#nu} CC1#pi^{0} on n",
                         kGreen + 3, 2, 1001);
  TrueModeStack::AddMode(-13, "ANTINU_CC1piponn", "#bar{#nu} CC1#pi^{+} on n",
                         kGreen - 2, 2, 1001);
  TrueModeStack::AddMode(-16, "ANTINU_CCcoh", "#bar{#nu} CC coherent", kBlue, 2,
                         1001);
  TrueModeStack::AddMode(-17, "ANTINU_CC1gamma", "#bar{#nu} CC1#gamma",
                         kMagenta, 2, 1001);
  TrueModeStack::AddMode(-21, "ANTINU_CCMultipi",
                         "#bar{#nu} Multi #pi (1.3 < W < 2.0)", kYellow, 2,
                         1001);
  TrueModeStack::AddMode(-22, "ANTINU_CC1eta", "#bar{#nu} CC1#eta^{0} on n",
                         kYellow - 2, 2, 1001);
  TrueModeStack::AddMode(-23, "ANTINU_CC1lamkp", "#bar{#nu} CC1#Lambda1K^{+}",
                         kYellow - 6, 2, 1001);
  TrueModeStack::AddMode(-26, "ANTINU_CCDIS", "#bar{#nu} DIS (W > 2.0)", kRed,
                         2, 1001);

  // NC
  TrueModeStack::AddMode(-31, "ANTINU_NC1pi0onn", "#bar{#nu} NC1#pi^{0} on n",
                         kBlue, 2, 3004);
  TrueModeStack::AddMode(-32, "ANTINU_NC1pi0onp", "#bar{#nu} NC1#pi^{0} on p",
                         kBlue + 3, 2, 3004);
  TrueModeStack::AddMode(-33, "ANTINU_NC1pimonn", "#bar{#nu} NC1#pi^{-} on n",
                         kBlue - 2, 2, 3004);
  TrueModeStack::AddMode(-34, "ANTINU_NC1piponp", "#bar{#nu} NC1#pi^{+} on p",
                         kBlue - 8, 2, 3004);
  TrueModeStack::AddMode(-36, "ANTINU_NCcoh", "#bar{#nu} NC Coherent",
                         kBlue + 8, 2, 3004);
  TrueModeStack::AddMode(-38, "ANTINU_NC1gammaonn", "#bar{#nu} NC1#gamma on n",
                         kMagenta, 2, 3004);
  TrueModeStack::AddMode(-39, "ANTINU_NC1gammaonp", "#bar{#nu} NC1#gamma on p",
                         kMagenta - 10, 2, 3004);
  TrueModeStack::AddMode(-41, "ANTINU_NCMultipi",
                         "#bar{#nu} Multi #pi (1.3 < W < 2.0)", kBlue - 10, 2,
                         3004);
  TrueModeStack::AddMode(-42, "ANTINU_NC1etaonn", "#bar{#nu} NC1#eta^{0} on n",
                         kYellow - 2, 2, 3004);
  TrueModeStack::AddMode(-43, "ANTINU_NC1etaonp", "#bar{#nu} NC1#eta^{0} on p",
                         kYellow - 4, 2, 3004);
  TrueModeStack::AddMode(-44, "ANTINU_NC1kamk0",
                         "#bar{#nu} NC1#Lambda1K^{0} on n", kYellow - 6, 2,
                         3004);
  TrueModeStack::AddMode(-45, "ANTINU_NC1lamkp", "#bar{#nu} NC1#Lambda1K^{+}",
                         kYellow - 10, 2, 3004);
  TrueModeStack::AddMode(-46, "ANTINU_NCDIS", "#bar{#nu} DIS (W > 2.0)", kRed,
                         2, 3004);
  TrueModeStack::AddMode(-51, "ANTINU_NCELonp", "#bar{#nu} NCEL on p", kBlack,
                         2, 3004);
  TrueModeStack::AddMode(-52, "ANTINU_NCELonn", "#bar{#nu} NCEL on n", kGray, 2,
                         3004);

  StackBase::SetupStack(hist);
};

// Species Stack Functions
BeamSpeciesStack::BeamSpeciesStack(std::string name, std::string title,
                                   TH1 *hist) {
  fName = name;
  fTitle = title;

  // charged eptons
  AddMode(0, "electron", "e^{-}", kBlue, 2, 1001);
  AddMode(1, "positron", "e^{+}", kBlue - 2, 2, 3004);
  AddMode(2, "muon", "#mu^{-}", kRed, 2, 1001);
  AddMode(3, "antimuon", "#mu^{+}", kRed - 2, 2, 3004);
  AddMode(4, "tau", "#tau^{-}", kGreen, 2, 1001);
  AddMode(5, "antitau", "#tau^{+}", kGreen - 2, 2, 3004);

  // neutrinos
  AddMode(6, "nue", "#nu_e", kBlue, 2, 1001);
  AddMode(7, "antinue", "#bar{#nu}_e", kBlue - 2, 2, 3004);
  AddMode(8, "numu", "#nu_#mu", kRed, 2, 1001);
  AddMode(9, "antinumu", "#bar{#nu}_#mu", kRed - 2, 2, 3004);
  AddMode(10, "nutau", "#nu_#tau", kGreen, 2, 1001);
  AddMode(11, "antinutau", "#bar{#nu}_#tau", kGreen - 2, 2, 3004);

  StackBase::SetupStack(hist);
};

int BeamSpeciesStack::ConvertSpeciesToIndex(int species) {
  switch (species) {
  case 11:
    return 0; // e-
  case -11:
    return 1; // e+
  case 13:
    return 2; // mu-
  case -13:
    return 3; // mu+
  case 15:
    return 4; // tau-
  case -15:
    return 5; // tau+
  case 12:
    return 6; // nue
  case -12:
    return 7; // nuebar
  case 14:
    return 8; // numu
  case -14:
    return 9; // numubar
  case 16:
    return 10; // nutau
  case -16:
    return 11; // nutaubar
  default:
    return 12;
  }
};

void BeamSpeciesStack::Fill(int species, double x, double y, double z,
                            double weight) {
  StackBase::FillStack(ConvertSpeciesToIndex(species), x, y, z, weight);
}

// Target Stack Functions
TargetTypeStack::TargetTypeStack(std::string name, std::string title,
                                 TH1 *hist) {
  fName = name;
  fTitle = title;

  AddMode(0, "H", "Hydrogen", kBlue, 2, 1001);
  AddMode(1, "C", "Carbon", kRed, 2, 1001);
  AddMode(2, "O", "Oxygen", kViolet, 2, 1001);
  AddMode(3, "UNDEFINED", "Undefined", kBlack, 2, 1001);

  StackBase::SetupStack(hist);
}

void TargetTypeStack::Fill(int pdg, double x, double y, double z,
                           double weight) {
  int index = ConvertPDGToIndex(pdg);
  StackBase::FillStack(index, x, y, z, weight);
}

int TargetTypeStack::ConvertPDGToIndex(int pdg) {
  switch (pdg) {
  case 1000010010:
    return 0; // H
  case 1000060120:
    return 1; // C
  case 1000080160:
    return 2; // O
  default:
    return 3; // Undef
  }
}

bool TargetTypeStack::IncludeInStack(TH1 *hist) {
  return (hist->Integral() > 0.0);
}

// CC Topology Stack Functions
CCTopologyStack::CCTopologyStack(std::string name, std::string title,
                                 TH1 *hist) {
  (void)hist;
  fName = name;
  fTitle = title;

  AddMode(0, "CC0pi", "CC-0#pi", kBlue, 2, 1001);
  AddMode(1, "CC1pip", "CC-1#pi^{+}", kRed, 2, 1001);
  AddMode(2, "CC1pim", "CC-1#pi^{-}", kGreen, 2, 1001);
  AddMode(3, "CC1pi0", "CC-1#pi^{0}", kYellow, 2, 1001);
  AddMode(4, "CCNpi", "CC-N#pi", kGray, 2, 1001);
  AddMode(5, "CCOther", "CC-Other", kViolet, 2, 1001);
  AddMode(6, "NC", "NC", kMagenta, 2, 1001);
  AddMode(7, "UNDEFINED", "Undefined", kBlack, 2, 1001);
}

void CCTopologyStack::Fill(FitEvent *evt, double x, double y, double z,
                           double weight) {
  int index = GetIndexFromEventParticles(evt);
  StackBase::FillStack(index, x, y, z, weight);
}

int CCTopologyStack::GetIndexFromEventParticles(FitEvent *evt) {

  int nleptons = evt->NumFSLeptons();
  int npiplus = evt->NumFSParticle(211);
  int npineg = evt->NumFSParticle(-211);
  int npi0 = evt->NumFSParticle(111);
  int npions = npiplus + npineg + npi0;

  if (nleptons == 1) {
    if (npions == 0) {
      return 0; // CC0pi
    } else if (npions == 1) {
      if (npiplus == 1)
        return 1; // CC1pi+
      else if (npineg == 1)
        return 2; // CC1pi-
      else if (npi0 == 1)
        return 3; // CC1pi0
    } else if (npions > 1) {
      return 4; // CCNpi
    }
  } else if (nleptons > 1) {
    return 5; // CCOther
  } else if (nleptons < 1) {
    return 6;
  }

  return 7; // Undefined?
}
