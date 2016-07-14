set(IMPLFILES
BaseFitEvt.cxx
FitParticle.cxx
FitWeight.cxx
Measurement1D.cxx
EventManager.cxx
FitSpline.cxx
InputHandler.cxx
Measurement2D.cxx
FitEvent.cxx
FitSplineHead.cxx
JointMeas1D.cxx
MeasurementBase.cxx
GeneratorUtils.cxx
)

set(HEADERFILES
BaseFitEvt.h    FitEvent.h     FitSplineHead.h  JointMeas1D.h    Measurement2D.h
EventManager.h  FitParticle.h  FitWeight.h      LinkDef.h        MeasurementBase.h
FitSpline.h    InputHandler.h   Measurement1D.h GeneratorUtils.h
)

set(LIBNAME FitBase)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(${LIBNAME} STATIC ${IMPLFILES})
else(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_library(${LIBNAME} SHARED ${IMPLFILES})
endif()

include_directories(${RWENGINE_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/src/Utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${LIBNAME} PROPERTIES VERSION
  "${ExtFit_VERSION_MAJOR}.${ExtFit_VERSION_MINOR}.${ExtFit_VERSION_REVISION}")
set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS ${ROOT_LD_FLAGS})

install(TARGETS ${LIBNAME} DESTINATION lib)
#Can uncomment this to install the headers... but is it really neccessary?
install(FILES ${HEADERFILES} DESTINATION include)

set(MODULETargets ${MODULETargets} ${LIBNAME} PARENT_SCOPE)
