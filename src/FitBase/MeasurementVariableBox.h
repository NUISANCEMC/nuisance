#ifndef MEASUREMENTVARIABLEBOX_H
#define MEASUREMENTVARIABLEBOX_H
#include "FitEvent.h"

class MeasurementVariableBox {
public:
  MeasurementVariableBox() {};
  ~MeasurementVariableBox() {};

  virtual void Reset() {
    fX = fY = fZ = -999.9;
    fMode = fEntry = -999;
    fSignal = false;
  }

  virtual void FillBoxFromEvent(FitEvent* evt){
    return;
  }

  double fX, fY, fZ;
  int fMode, fEntry;
  bool fSignal;
};

#endif