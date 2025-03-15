#ifndef _T2K_CC1pipNp_CH_1DSTV_NU_H_SEEN_
#define _T2K_CC1pipNp_CH_1DSTV_NU_H_SEEN_

#include "Measurement1D.h"

class T2K_CC1pipNp_CH_XSec_1DSTV_nu : public Measurement1D {
  public:
    T2K_CC1pipNp_CH_XSec_1DSTV_nu (nuiskey samplekey);
    virtual ~T2K_CC1pipNp_CH_XSec_1DSTV_nu () {};

    void FillEventVariables(FitEvent *event);
    bool isSignal(FitEvent *event);
    void SetupData();

  private:
    enum Dists { kdaT, kdpTT, kpN };
    Dists fDist;
};

#endif
