#ifndef _SAMPLE_LIST_H_
#define _SAMPLE_LIST_H_

/*!
 *  \addtogroup FCN
 *  @{
 */
#include <string>

class FitWeight;
class nuiskey;
class MeasurementBase;

//! Functions to make it easier for samples to be created and handled.
namespace SampleUtils {

//! Create a given sample given its name, file, type, fakdata(fkdt) file and the
//! current rw engine and push it back into the list fChain.
/*bool LoadSample(std::list<MeasurementBase*>* fChain, std::string name,
                std::string file, std::string type, std::string fkdt,
                FitWeight* rw);*/
MeasurementBase* CreateSample(std::string name, std::string file,
                              std::string type, std::string fkdt,
                              FitWeight* rw);
MeasurementBase* CreateSample(nuiskey samplekey);
}

/*! @} */
#endif
