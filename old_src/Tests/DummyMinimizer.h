#include "MinimizerRoutines.h"

struct DummyMinimizer : public MinimizerRoutines {
  DummyMinimizer() : MinimizerRoutines() {
    nuiskey p_m1 = Config::CreateKey("parameter");

    p_m1.SetS("type", "modenorm_parameter");
    p_m1.SetS("name", "mode_1");
    p_m1.SetD("nominal", 1);
    p_m1.SetS("state", "FREE");
    p_m1.SetD("low", 0);
    p_m1.SetD("high", 3);
    p_m1.SetD("step", 0.1);

    nuiskey p_m2 = Config::CreateKey("parameter");

    p_m2.SetS("type", "modenorm_parameter");
    p_m2.SetS("name", "mode_2");
    p_m2.SetD("nominal", 1);
    p_m2.SetS("state", "FREE");
    p_m2.SetD("low", 0);
    p_m2.SetD("high", 3);
    p_m2.SetD("step", 0.1);

    nuiskey p_m3 = Config::CreateKey("parameter");

    p_m3.SetS("type", "modenorm_parameter");
    p_m3.SetS("name", "mode_3");
    p_m3.SetD("nominal", 1);
    p_m3.SetS("state", "FREE");
    p_m3.SetD("low", 0);
    p_m3.SetD("high", 3);
    p_m3.SetD("step", 0.1);

    nuiskey dummysample = Config::CreateKey("sample");
    dummysample.SetS("name", "DummySample");

    Config::SetPar("dynamic_sample.path",
                   std::string(getenv("NUISANCE")) + "/build/Linux/tests");

    Config::SetPar("EventManager", false);
    // SETVERBOSITY(DEB);



    std::cout << "[INFO]: Parameter config:" << std::endl;
    nuisconfig::GetConfig().PrintXML(NULL);
    std::cout << "================" << std::endl;

    fOutputRootFile = new TFile("TestOutput.root", "RECREATE");

    SetupMinimizerFromXML();
    SetupRWEngine();
    SetupFCN();
  }

  std::string GetParamName (int i) { return fParams[i]; }
  double GetParamVal (int i) { return fCurVals[GetParamName(i)]; }
  double GetParamError (int i) { return fErrorVals[GetParamName(i)]; }

};
