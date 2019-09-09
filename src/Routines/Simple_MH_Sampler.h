#include "Math/Minimizer.h"

#include "FitLogger.h"

using ROOT::Math::Minimizer;

class Simple_MH_Sampler : public Minimizer {
  TRandom3 RNJesus;

  size_t step_i;
  int moved;

  size_t thin;
  size_t thin_ctr;

  size_t discard;

  struct Param {
    Param()
        : IsFixed(false),
          name(""),
          Val(0xdeadbeef),
          StepWidth(0xdeadbeef),
          LowLim(0xdeadbeef),
          UpLim(0xdeadbeef) {}
    Param(bool i, std::string n, double v, double s, double l, double u)
        : IsFixed(i), name(n), Val(v), StepWidth(s), LowLim(l), UpLim(u) {}
    bool IsFixed;
    std::string name;
    double Val, StepWidth, LowLim, UpLim;
  };

  std::vector<Param> start_params;

  double curr_value;
  std::vector<double> curr_params;

  double propose_value;
  std::vector<double> propose_params;

  double min_value;
  std::vector<double> min_params;

  TGraph trace;

  void RestartParams() {
    curr_params.resize(start_params.size());
    for (size_t p_it = 0; p_it < start_params.size(); ++p_it) {
      curr_params[p_it] = start_params[p_it].Val;
    }
    min_params = curr_params;
    propose_params = curr_params;
  }

  TTree *StepTree;

  void Write();

  ROOT::Math::IMultiGenFunction const *FCN;

 public:
  Simple_MH_Sampler() : Minimizer(), RNJesus(), trace() {
    thin = Config::GetParI("MCMC.thin");
    thin_ctr = 0;
    discard = Config::GetParI("MCMC.BurnInSteps");
  }

  void SetFunction(ROOT::Math::IMultiGenFunction const &func) { FCN = &func; }

  bool SetVariable(unsigned int ivar, std::string const &name, double val,
                   double step) {
    if (start_params.size() <= ivar) {
      start_params.resize(ivar + 1);
    }
    start_params[ivar] = Param(false, name, val, step, 0xdeadbeef, 0xdeadbeef);
    RestartParams();
    return true;
  }

  bool SetLowerLimitedVariable(unsigned int ivar, std::string const &name,
                               double val, double step, double lower) {
    SetVariable(ivar, name, val, step);
    start_params[ivar].LowLim = lower;
    return true;
  }
  bool SetUpperLimitedVariable(unsigned int ivar, std::string const &name,
                               double val, double step, double upper) {
    SetVariable(ivar, name, val, step);
    start_params[ivar].UpLim = upper;
    return true;
  }
  bool SetLimitedVariable(unsigned int ivar, std::string const &name,
                          double val, double step, double lower, double upper) {
    SetLowerLimitedVariable(ivar, name, val, step, lower);
    start_params[ivar].UpLim = upper;
    return true;
  }
  bool SetFixedVariable(unsigned int ivar, std::string const &name,
                        double val) {
    SetVariable(ivar, name, val, 0xdeadbeef);
    start_params[ivar].IsFixed = true;
    return true;
  }
  bool SetVariableValue(unsigned int ivar, double value) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to set uninitialised variable.");
      return false;
    }
    start_params[ivar].Val = value;
    return true;
  }
  bool SetVariableStepSize(unsigned int ivar, double value) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to set uninitialised variable.");
      return false;
    }
    start_params[ivar].StepWidth = value;
    return true;
  }
  bool SetVariableLowerLimit(unsigned int ivar, double lower) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to set uninitialised variable.");
      return false;
    }
    start_params[ivar].LowLim = lower;
    return true;
  }
  bool SetVariableUpperLimit(unsigned int ivar, double upper) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to set uninitialised variable.");
      return false;
    }
    start_params[ivar].UpLim = upper;
    return true;
  }
  bool SetVariableLimits(unsigned int ivar, double lower, double upper) {
    SetVariableLowerLimit(ivar, lower);
    SetVariableUpperLimit(ivar, upper);
    return true;
  }
  bool FixVariable(unsigned int ivar) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to fix uninitialised variable.");
      return false;
    }
    start_params[ivar].IsFixed = true;
    return true;
  }
  bool ReleaseVariable(unsigned int ivar) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to fix uninitialised variable.");
      return false;
    }
    start_params[ivar].IsFixed = false;
    return true;
  }
  bool IsFixedVariable(unsigned int ivar) {
    if (start_params.size() <= ivar) {
      QERROR(WRN, "Tried to fix uninitialised variable.");
      return false;
    }
    return start_params[ivar].IsFixed;
  }

  double MinValue() const { return min_value; }
  const double *X() const { return min_params.data(); }
  const double *Errors() const { return min_params.data(); }

  unsigned int NDim() const { return start_params.size(); }

  unsigned int NFree() const {
    unsigned int NFree = 0;

    for (size_t p_it = 0; p_it < start_params.size(); ++p_it) {
      NFree += !start_params[p_it].IsFixed;
    }
    return NFree;
  }

  void AddBranches() {
    TFile *ogf = gFile;
    if (Config::Get().out && Config::Get().out->IsOpen()) {
      Config::Get().out->cd();
    }

    StepTree = new TTree("MCMChain", "");
    StepTree->Branch("Step", &step_i, "Step/I");
    StepTree->Branch("Value", &curr_value, "Value/D");
    StepTree->Branch("Moved", &moved, "Moved/I");

    std::stringstream ss("");
    for (size_t p_it = 0; p_it < curr_params.size(); ++p_it) {
      ss.str("");
      ss << "param_" << p_it;
      StepTree->Branch(ss.str().c_str(), &curr_params[p_it],
                       (ss.str() + "/D").c_str());
    }

    if (ogf && ogf->IsOpen()) {
      ogf->cd();
    }
  }

  void Fill() { StepTree->Fill(); }

  void Propose() {
    for (size_t p_it = 0; p_it < start_params.size(); ++p_it) {
      double propose_param = curr_params[p_it];

      if (!start_params[p_it].IsFixed) {
        size_t attempts = 0;
        do {
          if (attempts > 1000) {
            QTHROW("After 1000 attempts, failed to throw Gaus("
                  << start_params[p_it].Val << ", "
                  << start_params[p_it].StepWidth << ") inside limits: [ "
                  << start_params[p_it].LowLim << " -- "
                  << start_params[p_it].UpLim << " ]");
          }

          double thr =
              RNJesus.Gaus(curr_params[p_it], start_params[p_it].StepWidth);

          if ((start_params[p_it].LowLim != 0xdeadbeef) &&
              (thr < start_params[p_it].LowLim)) {
            attempts++;
            continue;
          }

          if ((start_params[p_it].UpLim != 0xdeadbeef) &&
              (thr > start_params[p_it].UpLim)) {
            attempts++;
            continue;
          }

          propose_param = thr;
          break;

        } while (true);
      }

      propose_params[p_it] = propose_param;
    }
  }

  void Evaluate() {
    propose_value = exp(-(*FCN)(propose_params.data()) / 10000.0);
    if (propose_value < min_value) {
      min_params = propose_params;
    }
  }

  void PrintResults() {
    QLOG(FIT, "Simple_MH_Sampler State: ");
    for (size_t p_it = 0; p_it < start_params.size(); ++p_it) {
      QLOG(FIT, "\t[" << p_it
                      << "]: " << (start_params[p_it].IsFixed ? " FIX" : "FREE")
                      << " " << curr_params[p_it]);
    }
    QLOG(FIT, "Curr LHood: " << curr_value << ", Min LHood: " << min_value);
  }

  void Step() {
    moved = false;

    if (propose_value != propose_value) {
      curr_params = propose_params;
      curr_value = propose_value;
      PrintResults();
      QTHROW("Proposed a NAN value.");
    }

    std::cout << "[" << step_i << "] proposed: " << propose_value
              << " | current: " << curr_value << std::endl;
    double a = propose_value / curr_value;
    std::cout << "\ta = " << a << std::endl;
    if (a >= 1.0) {
      moved = true;
      std::cout << "\tMoved." << std::endl;
    } else {
      double b = RNJesus.Uniform(1);
      if (b < a) {
        moved = true;
        std::cout << "\tMoved (" << b << ")" << std::endl;
      } else {
        std::cout << "\tStayed. (" << b << ")" << std::endl;
      }
    }

    if (moved) {
      curr_params = propose_params;
      curr_value = propose_value;
    }
  }

  bool Minimize() {
    if (!start_params.size()) {
      QERROR(FTL, "No Parameters passed to Simple_MH_Sampler.");
      return false;
    }

    RestartParams();

    AddBranches();

    size_t NSteps = Options().MaxIterations();
    trace.Set(NSteps);
    QLOG(FIT, "Running chain for " << NSteps << " steps.");
    step_i = 0;
    while (step_i < NSteps) {
      Propose();

      Evaluate();

      Step();

      trace.SetPoint(step_i, step_i, curr_value);

      if (step_i >= discard) {
        thin_ctr++;
        if (thin_ctr == thin) {
          Fill();
          thin_ctr = 0;
        }
      }
      step_i++;
    }

    StepTree->Write();
    trace.Write("MCMCTrace");

    return true;
  };
};
