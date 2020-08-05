#ifndef NUISANCE_WEIGHT_CALCS
#define NUISANCE_WEIGHT_CALCS

#include "BaseFitEvt.h"
#include "BeRPA.h"

class NUISANCEWeightCalc {
  public:
    NUISANCEWeightCalc() {};
    virtual ~NUISANCEWeightCalc() {};

    virtual double CalcWeight(BaseFitEvt* evt){return 1.0;};
    virtual void SetDialValue(std::string name, double val){};
    virtual void SetDialValue(int rwenum, double val){};
    virtual bool IsHandled(int rwenum){return false;};

    virtual void Print(){};

    std::map<std::string, int> fDialNameIndex;
    std::map<int, int> fDialEnumIndex;
    std::vector<double> fDialValues;

    std::string fName;
};

class ModeNormCalc : public NUISANCEWeightCalc {
  public:
    ModeNormCalc();
    ~ModeNormCalc(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

    double fNormRES;
};

// MINOS pion tuning, https://arxiv.org/pdf/1903.01558.pdf
class MINOSRPA : public NUISANCEWeightCalc {
  public:
    MINOSRPA();
    ~MINOSRPA(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

    double GetRPAWeight(double Q2);

    bool fTweaked;

    bool fApply_MINOSRPA;

    double fTwk_MINOSRPA_A;
    double fDef_MINOSRPA_A;
    double fCur_MINOSRPA_A;

    double fTwk_MINOSRPA_B;
    double fDef_MINOSRPA_B;
    double fCur_MINOSRPA_B;

};

// MINERvA pion tuning, https://arxiv.org/pdf/1903.01558.pdf
class LagrangeRPA : public NUISANCEWeightCalc {
  public:
    LagrangeRPA();
    ~LagrangeRPA(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

    double GetRPAWeight(double Q2);

    bool fTweaked;

    bool fApplyRPA;

    double fR1;
    double fR2;
    double fR1_Def;
    double fR2_Def;
};

class BeRPACalc : public NUISANCEWeightCalc {
  public:
    BeRPACalc();
    ~BeRPACalc(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

  private:
    // Parameter values
    double fBeRPA_A;
    double fBeRPA_B;
    double fBeRPA_D;
    double fBeRPA_E;
    double fBeRPA_U;
    // Counts of enabled parameters
    int nParams;

};

class SBLOscWeightCalc : public NUISANCEWeightCalc {
  public:
    SBLOscWeightCalc();
    ~SBLOscWeightCalc(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

    double GetSBLOscWeight(double E);

    double fDistance;
    double fMassSplitting;
    double fSin2Theta;

};

class GaussianModeCorr : public NUISANCEWeightCalc {
  public:

    GaussianModeCorr();
    ~GaussianModeCorr(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);
    double GetGausWeight(double q0, double q3, double vals[]);
    // Set the Gaussian method (tilt-shift or normal Gaussian parameters)
    void SetMethod(bool method);

    // 5 pars describe the Gaussain
    // 0 norm.
    // 1 q0 pos
    // 2 q0 width
    // 3 q3 pos
    // 4 q3 width
    static const int kPosNorm = 0;
    static const int kPosTilt = 1;
    static const int kPosPq0  = 2;
    static const int kPosWq0  = 3;
    static const int kPosPq3  = 4;
    static const int kPosWq3  = 5;

    bool fApply_CCQE;
    double fGausVal_CCQE[6];

    bool fApply_2p2h;
    double fGausVal_2p2h[6];

    bool fApply_2p2h_PPandNN;
    double fGausVal_2p2h_PPandNN[6];

    bool fApply_2p2h_NP;
    double fGausVal_2p2h_NP[6];

    bool fApply_CC1pi;
    double fGausVal_CC1pi[6];

    bool fAllowSuppression;

    bool fDebugStatements;

    bool fMethod;
};


#endif
