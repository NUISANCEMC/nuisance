#ifndef MEASUREMENTVARIABLEBOX1D_H
#define MEASUREMENTVARIABLEBOX1D_H
#include "FitEvent.h"
#include "MeasurementVariableBox.h"

class MeasurementVariableBox1D : public MeasurementVariableBox {
public:
  MeasurementVariableBox1D() {};
  ~MeasurementVariableBox1D() {};

  virtual void Reset();
  virtual void FillBoxFromEvent(FitEvent* evt);
  virtual MeasurementVariableBox* CloneSignalBox();
  virtual void Print();

  virtual double GetX();
  virtual double GetY();
  virtual double GetZ();
  virtual int GetMode();

  virtual void SetX(double x) {fX = x;};
  virtual void SetY(double y) {(void)y;};
  virtual void SetZ(double z) {(void)z;};
  virtual void SetMode(int m) {(void)m;};

  double fX;

};

#endif
