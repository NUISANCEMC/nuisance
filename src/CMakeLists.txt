SET(CoreDirs
	Reweight
	Config
	FCN
	FitBase
	InputHandler
	Logger
	MCStudies
	Routines
	# Smearceptance
	Splines
	Statistical
  HEPDATA
	Utils)

foreach(CD ${CoreDirs})
	LIST(APPEND Core_Include_Dirs ${CMAKE_SOURCE_DIR}/src/${CD})
endforeach()

add_library(CoreIncludes INTERFACE)
set_target_properties(CoreIncludes PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${Core_Include_Dirs}"
	INTERFACE_LINK_LIBRARIES GeneratorCompileDependencies)

SET(ExperimentDirs
	ANL
	ArgoNeuT
	BEBC
	BNL
	Electron
	FNAL
	GGM
	K2K
	MicroBooNE
	MINERvA
	MiniBooNE
	SciBooNE
	T2K)

#Sets the default state of all experimental subdirectories
if(NOT DEFINED EXPERIMENTS_ENABLED)
	SET(EXPERIMENTS_ENABLED TRUE)
endif()
SET(Experiment_Defines)

foreach(EXP ${ExperimentDirs})
	if(NOT DEFINED ${EXP}_ENABLED)
		set(${EXP}_ENABLED ${EXPERIMENTS_ENABLED})
		set(${EXP}_ENABLED ${EXPERIMENTS_ENABLED} PARENT_SCOPE)
	endif()

	if(${EXP}_ENABLED OR EXPERIMENTS_ENABLED)
		add_subdirectory(${EXP})
		LIST(APPEND Experiment_Defines -D${EXP}_ENABLED)
		LIST(APPEND Experiment_Include_Dirs ${CMAKE_SOURCE_DIR}/src/${EXP})
		LIST(APPEND Experiment_Targets ${EXP})
	endif()
endforeach()

add_library(Experiments INTERFACE)
set_target_properties(Experiments PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${Experiment_Include_Dirs}"
	INTERFACE_COMPILE_OPTIONS "${Experiment_Defines}"
	INTERFACE_LINK_LIBRARIES "${Experiment_Targets}")

foreach(CD ${CoreDirs})
	add_subdirectory(${CD})
	LIST(APPEND Core_Targets ${CD})
endforeach()

add_library(CoreTargets INTERFACE)
set_target_properties(CoreTargets PROPERTIES
	INTERFACE_LINK_LIBRARIES "Experiments;-Wl,--start-group;${Core_Targets};-Wl,--end-group")
