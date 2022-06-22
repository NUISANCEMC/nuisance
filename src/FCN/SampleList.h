#ifndef _SAMPLE_LIST_H_
#define _SAMPLE_LIST_H_

/*!
 *  \addtogroup FCN
 *  @{
 */
#include <map>
#include <string>
#include <vector>

class FitWeight;
class nuiskey;
class MeasurementBase;

/// Expect each .so containing samples to supply 4 c-style methods.
/// int DSF_NSamples();
/// char const * DSF_GetSampleName(int);
/// MeasurementBase* DSF_GetSample(int, nuiskey *);
/// void DSF_DestroySample(MeasurementBase *);
class DynamicSampleFactory {
  size_t NSamples;
  size_t NManifests;

  DynamicSampleFactory();

  static DynamicSampleFactory* glblDSF;

  typedef int (*DSF_NSamples_ptr)(void);
  typedef char const* (*DSF_GetSampleName_ptr)(int);
  typedef MeasurementBase* (*DSF_GetSample_ptr)(int, nuiskey *);
  typedef void (*DSF_DestroySample_ptr)(MeasurementBase*);

  struct PluginManifest {
    void* dllib;

    DSF_NSamples_ptr DSF_NSamples;
    DSF_GetSampleName_ptr DSF_GetSampleName;
    DSF_GetSample_ptr DSF_GetSample;
    DSF_DestroySample_ptr DSF_DestroySample;

    std::string soloc;
    std::vector<MeasurementBase*> Instances;
    std::vector<std::string> SamplesProvided;
    size_t NSamples;
    ~PluginManifest();
  };

  std::map<std::string, PluginManifest> Manifests;
  std::map<std::string, std::pair<std::string, int> > Samples;

  void LoadPlugins();

 public:
  static DynamicSampleFactory& Get();

  void Print();

  bool HasSample(std::string const& name);
  bool HasSample(nuiskey& samplekey);

  MeasurementBase* CreateSample(nuiskey& samplekey);

  ~DynamicSampleFactory();
};

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
