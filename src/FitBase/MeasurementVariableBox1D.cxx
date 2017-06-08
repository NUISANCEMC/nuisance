#include "MeasurementVariableBox1D.h"

void MeasurementVariableBox1D::Reset() {
  fX = -999.9;
}

void MeasurementVariableBox1D::FillBoxFromEvent(FitEvent* evt) {
  return;
}

MeasurementVariableBox* MeasurementVariableBox1D::CloneSignalBox() {
  MeasurementVariableBox1D* box = new MeasurementVariableBox1D();
  box->fX = this->fX;
  return box;
};

void MeasurementVariableBox1D::Print() {
  std::cout << "Printing Empty BOX! " << std::endl;
}

double MeasurementVariableBox1D::GetX(){
  return fX;
}

double MeasurementVariableBox1D::GetY(){
  return -999.9;
}

double MeasurementVariableBox1D::GetZ(){
  return -999.9;
}

int MeasurementVariableBox1D::GetMode(){
  return 0;
}

void MeasurementVariableBox1D::SetX(double x){
  fX = x;
}

