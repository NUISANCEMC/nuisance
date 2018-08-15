#ifndef MINERvA_VARIABLES_BOX_H
#define MINERvA_VARIABLES_BOX_H

#include "MeasurementVariableBox.h"
#include "MeasurementVariableBox1D.h"
#include "MeasurementVariableBox2D.h"

/*!
 *  \addtogroup FitBase
 *  @{
 */

/// Custom box used to also save All Pion Tpi for each event.
class NTpiVariableBox1D : public MeasurementVariableBox1D {
public:
	inline NTpiVariableBox1D() { };
	inline void Reset() {  fTpiVect.clear(); } 

    inline MeasurementVariableBox* CloneSignalBox(){
        NTpiVariableBox1D* box = new NTpiVariableBox1D(); 
        box->fX = this->fX;
        box->fSampleWeight = this->fSampleWeight;

        box->fTpiVect.clear();
        for (int i = 0; i < this->fTpiVect.size(); i++){
            box->fTpiVect.push_back( this->fTpiVect[i] );
        }
        return box;
    }
    inline void Print(){
        std::cout << "Box Print Size : " << this->fTpiVect.size() << std::endl;
    }

	std::vector<double> fTpiVect;
};

/// Custom box used to also save All Pion Tpi for each event.
class NthpiVariableBox1D : public MeasurementVariableBox1D {
 public:
  inline NthpiVariableBox1D() {  };
  inline void Reset() {  fthpiVect.clear(); }
  inline MeasurementVariableBox* CloneSignalBox(){
        NthpiVariableBox1D* box = new NthpiVariableBox1D(); 
        box->fX = this->fX;
        box->fSampleWeight = this->fSampleWeight;

        box->fthpiVect.clear();
        for (int i = 0; i < this->fthpiVect.size(); i++){
            box->fthpiVect.push_back( this->fthpiVect[i] );
        }
        return box;
    }
  std::vector<double> fthpiVect;
};


#endif
