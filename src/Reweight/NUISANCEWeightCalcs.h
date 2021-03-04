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

// SF modif

// Constant RW per shell

class SFRW_ShellNormCalc : public NUISANCEWeightCalc {
  public:
    SFRW_ShellNormCalc();
    ~SFRW_ShellNormCalc(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);

    double fNormPShellC;
    double fNormSShellC;

    double fNormP12ShellO;
    double fNormP32ShellO;
    double fNormSShellO;
};

// end Constant RW per shell

// Gaussian RW per shell

class SFGausRW_ShellCalc : public NUISANCEWeightCalc {
  public:
    SFGausRW_ShellCalc();
    ~SFGausRW_ShellCalc(){};

    double CalcWeight(BaseFitEvt* evt);
    void SetDialValue(std::string name, double val);
    void SetDialValue(int rwenum, double val);
    bool IsHandled(int rwenum);
    double GetGausWeight(double Emiss, double vals[]);

    // 3 params to describe the Gaussian:
    // 0 norm
    // 1 mean (position)
    // 2 sigma (width)
    
    static const int kPosNorm = 0;
    static const int kPosP = 1;
    static const int kPosW = 2;

    double fGaus_pShell_C[3];
    double fGaus_sShell_C[3];
    
    double fGaus_p12Shell_O[3];
    double fGaus_p32Shell_O[3];
    double fGaus_sShell_O[3];

    double fSRC_strength; // RW SRC part
    
    double fCCQE_norm;
};

// end Gaussian RW per shell

// Pmiss RW

class PmissRW_Calc : public NUISANCEWeightCalc {
 public:
  PmissRW_Calc();
  ~PmissRW_Calc(){};
    
  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);
    
  double GetWeightCarbonP(double pmiss, double dial);
  double GetWeightCarbonS(double pmiss, double dial);

  double fPmissRW_pC; // p-shell, C
  double fPmissRW_sC; // s-shell, C
    
  double fPmissRW_p12O; // p12-shell, O
  double fPmissRW_p32O; // p32-shell, O
  double fPmissRW_sO; // s-shell, O


};


// end Pmiss RW

// end SF modif


// PB

class PBRW_Calc : public NUISANCEWeightCalc {
 public:
  PBRW_Calc();
  ~PBRW_Calc(){};

  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);

  void SetHistograms(TH2D* templ_up, TH2D* templ_low);

  double GetWeightq0q3(double q0, double q3, double dial);

  double fPB_q0q3;
  TH2D* fTemplateUp;
  TH2D* fTemplateLow;


};

// end PB


// FSI RW modif

class FSIRW_Calc : public NUISANCEWeightCalc {
 public:
  FSIRW_Calc();
  ~FSIRW_Calc(){};
    
  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);
    
  bool IsNoFSI(std::vector<int> PDGvert, std::vector<TLorentzVector> pvert, std::vector<int> PDGfs, std::vector<TLorentzVector> pfs, double eps);
  bool SameParticlesVertFS(std::vector<int> PDGvert, std::vector<int> PDGfs);
  int GetNpi(std::vector<int> PDG);

  double fFSIRW_noFSI; // no FSI
  double fFSIRW_elasticFSI; // elastic FSI (change in the kinematics of primary vertex particles only)
  double fFSIRW_inelasticFSI; // extra-nucleons and same pion (if any)
  double fFSIRW_pionProdFSI; // pion production
  double fFSIRW_pionAbsFSI; // pion absorption
};


// Binary FSI

class BinaryFSIRW_Calc : public NUISANCEWeightCalc {
 public:
  BinaryFSIRW_Calc();
  ~BinaryFSIRW_Calc(){};

  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);

  bool IsNoFSI(std::vector<int> PDGvert, std::vector<TLorentzVector> pvert, std::vector<int> PDGfs, std::vector<TLorentzVector> pfs, double eps);
  bool SameParticlesVertFS(std::vector<int> PDGvert, std::vector<int> PDGfs);

  double fBinaryFSIRW_noFSI; // no FSI
  double fBinaryFSIRW_withFSI; // FSI
};

// end Binary FSI


class FSIRWPiAbs_Calc : public NUISANCEWeightCalc {
 public:
  FSIRWPiAbs_Calc();
  ~FSIRWPiAbs_Calc(){};

  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);

  int GetNumberPi(std::vector<int> PDG);

  double fFSIRWPiAbs;
};


// end FSI RW modif

// 2p2h normalization RW

class RW2p2h_Calc : public NUISANCEWeightCalc {
 public:
  RW2p2h_Calc();
  ~RW2p2h_Calc(){};
    
  double CalcWeight(BaseFitEvt* evt);
  void SetDialValue(std::string name, double val);
  void SetDialValue(int rwenum, double val);
  bool IsHandled(int rwenum);
   
  double fRW2p2h_norm;
};

// end 2p2h normalization RW


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
