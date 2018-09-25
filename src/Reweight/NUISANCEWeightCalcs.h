#ifndef NUISANCE_WEIGHT_CALCS
#define NUISANCE_WEIGHT_CALCS

#include "BaseFitEvt.h"

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
