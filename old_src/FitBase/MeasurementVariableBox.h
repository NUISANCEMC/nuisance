#ifndef MEASUREMENTVARIABLEBOX_H
#define MEASUREMENTVARIABLEBOX_H
#include "FitEvent.h"

class MeasurementVariableBox {
public:
  
  MeasurementVariableBox() {};
  ~MeasurementVariableBox() {};

  virtual void Reset();
  virtual void FillBoxFromEvent(FitEvent* evt);
  virtual MeasurementVariableBox* CloneSignalBox();
  virtual void Print();

  virtual double GetX();
  virtual double GetY();
  virtual double GetZ();
  virtual int GetMode();

  virtual void SetX(double x){};
  virtual void SetY(double y){};
  virtual void SetZ(double z){};
  virtual void SetMode(int m){};

  inline virtual void SetSampleWeight(double w){fSampleWeight = w;};
  inline virtual double GetSampleWeight(){return fSampleWeight;};
  double fSampleWeight;
};

#endif
