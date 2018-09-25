#ifndef MINERVA_WEIGHT_CALCS
#define MINERVA_WEIGHT_CALCS

#include <string>

#ifdef __MINERVA_RW_ENABLED__
#ifdef __GENIE_ENABLED__
#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "FitEvent.h"

#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "GHEP/GHepUtils.h"
#include "GeneralUtils.h"
#include "NUISANCESyst.h"
#include "NUISANCEWeightCalcs.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "PDG/PDGUtils.h"
#include "WeightUtils.h"
#include "weightRPA.h"
using namespace genie;
class BaseFitEvt;


namespace nuisance {
  namespace reweight {

    // MEC Dials
    class MINERvAReWeight_QE : public NUISANCEWeightCalc {
      public:
        MINERvAReWeight_QE();
        virtual ~MINERvAReWeight_QE();

        double CalcWeight(BaseFitEvt* evt);
        void SetDialValue(std::string name, double val);
        void SetDialValue(int rwenum, double val);
        bool IsHandled(int rwenum);

        double fTwk_NormCCQE;
        double fCur_NormCCQE;
        double fDef_NormCCQE;
        bool fTweaked;

    };


    // MEC Dials
    class MINERvAReWeight_MEC : public NUISANCEWeightCalc {
      public:
        MINERvAReWeight_MEC();
        virtual ~MINERvAReWeight_MEC();

        double CalcWeight(BaseFitEvt* evt);
        void SetDialValue(std::string name, double val);
        void SetDialValue(int rwenum, double val);
        bool IsHandled(int rwenum);

        double fTwk_NormCCMEC;
        double fCur_NormCCMEC;
        double fDef_NormCCMEC;
        bool fTweaked;

    };

    // RES Dials
    class MINERvAReWeight_RES : public NUISANCEWeightCalc {
      public:
        MINERvAReWeight_RES();
        virtual ~MINERvAReWeight_RES();

        double CalcWeight(BaseFitEvt* evt);
        void SetDialValue(std::string name, double val);
        void SetDialValue(int rwenum, double val);
        bool IsHandled(int rwenum);

        double fTwk_NormCCRES;
        double fCur_NormCCRES;
        double fDef_NormCCRES;
        bool fTweaked;

    };

    /// RPA Weight Calculator that applies RPA systematics
    /// to GENIE events. GENIE EVENTS ONLY!
    class RikRPA : public NUISANCEWeightCalc {
      public:
        RikRPA();
        ~RikRPA();

        double CalcWeight(BaseFitEvt* evt);
        void SetDialValue(std::string name, double val);
        void SetDialValue(int rwenum, double val);
        bool IsHandled(int rwenum);

        void SetupRPACalculator(int calcenum);
        int GetRPACalcEnum(int bpdg, int tpdg);

        bool   fApplyDial_RPACorrection;

        double fTwkDial_RPALowQ2;
        double fDefDial_RPALowQ2;
        double fCurDial_RPALowQ2;
        double fErrDial_RPALowQ2;

        double fTwkDial_RPAHighQ2;
        double fDefDial_RPAHighQ2;
        double fCurDial_RPAHighQ2;
        double fErrDial_RPAHighQ2;

        bool   fApplyDial_RESRPACorrection;

        double fTwkDial_RESRPALowQ2;
        double fDefDial_RESRPALowQ2;
        double fCurDial_RESRPALowQ2;
        double fErrDial_RESRPALowQ2;

        double fTwkDial_RESRPAHighQ2;
        double fDefDial_RESRPAHighQ2;
        double fCurDial_RESRPAHighQ2;
        double fErrDial_RESRPAHighQ2;

        double* fEventWeights;
        bool fTweaked;

        const static int kMaxCalculators = 10;
        enum rpacalcenums {
          kNuMuC12,
          kNuMuO16,
          kNuMuAr40,
          kNuMuCa40,
          kNuMuFe56,
          kNuMuBarC12,
          kNuMuBarO16,
          kNuMuBarAr40,
          kNuMuBarCa40,
          kNuMuBarFe56
        };
        weightRPA* fRPACalculators[kMaxCalculators];
    };

  };  // namespace reweight
};  // namespace nuisance

#endif // __GENIE_ENABLED__
#endif //__MINERVA_RW_ENABLED__

#endif
