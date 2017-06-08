#include "MeasurementVariableBox2D.h"

void MeasurementVariableBox2D::Reset() {
  fX = -999.9;
  fY = -999.9;
}

void MeasurementVariableBox2D::FillBoxFromEvent(FitEvent* evt) {
  return;
}

MeasurementVariableBox* MeasurementVariableBox2D::CloneSignalBox() {
  MeasurementVariableBox2D* box = new MeasurementVariableBox2D();
  box->fX = this->fX;
  box->fY = this->fY;
  return box;
};

void MeasurementVariableBox2D::Print() {
  std::cout << "Printing Empty BOX! " << std::endl;
}

double MeasurementVariableBox2D::GetX(){
  return fX;
}

double MeasurementVariableBox2D::GetY(){
  return fY;
}

double MeasurementVariableBox2D::GetZ(){
  return -999.9;
}

int MeasurementVariableBox2D::GetMode(){
  return 0;
}

void MeasurementVariableBox2D::SetX(double x){
  fX = x;
}

void MeasurementVariableBox2D::SetY(double y){
  fY = y;
}
